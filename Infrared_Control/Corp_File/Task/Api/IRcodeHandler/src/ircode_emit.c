#include "ircode_emit.h"
#include "stm32g0xx_ll_tim.h"
#include "ircode_catch.h"
#include "tim.h"
#include "serialctrl_memopt.h"
#include "25QFlash.h"
#include "Task_IREmit.h"
static uint16_t ircode_send_len = 0;
static uint16_t ircode_send_position = 0;
static uint8_t ircode_send_busy = 0;



uint8_t ircode_emit_addcode(uint8_t devnum,uint8_t* offset,uint8_t offset_total)
{
	uint32_t offset_sum = 0 ;
	uint16_t offset_cnt[INDEX_MAX_OFFSER];
	
	uint32_t read_addr = 0;
	uint16_t max_send_len = 0;
	uint8_t pack_total = 0;
	uint16_t send_len = 0;
	uint8_t* readflash_pt = NULL;
	uint8_t err_cnt = 0;
	
	union pkg_index_if read_buf;
	
	if (memopt_index_read(devnum,&(read_buf.g_pkg_index)) == 0xee)
		return 0xE1;
	if (read_buf.g_pkg_index.device_offset_total != offset_total)
		return 0xE2;
	if (ircode_catch_init_data()!= 0xCC)
		return 0xE4;
	
	for(uint8_t i = 0; i < read_buf.g_pkg_index.device_offset_total; i ++ ){
		if ( offset[i] > read_buf.g_pkg_index.offset[i]){
			ircode_catch_free_data();
			return 0xE7;
		}
	}
	
	for(uint8_t i = 0; i < INDEX_MAX_OFFSER; i++)
		offset_cnt[i] = 1;
	
	for (uint8_t i = 0; i < read_buf.g_pkg_index.device_offset_total - 1; i ++){
			for(uint8_t j = i + 1; j< read_buf.g_pkg_index.device_offset_total; j++)
			{
				offset_cnt[i] *= read_buf.g_pkg_index.offset[j];
			}
			offset_sum += (offset[i]*offset_cnt[i]);
		}
		
	offset_sum += offset[read_buf.g_pkg_index.device_offset_total - 1];
		
	max_send_len = 100;	
	pack_total = read_buf.g_pkg_index.device_ircode_len / max_send_len + 1;
	
	
	
	for (uint8_t i = 0; i < pack_total; i ++)
	{
		if (max_send_len > read_buf.g_pkg_index.device_ircode_len){
			send_len = read_buf.g_pkg_index.device_ircode_len;
		}else{
		if (max_send_len > read_buf.g_pkg_index.device_ircode_len - i * max_send_len)
			send_len = read_buf.g_pkg_index.device_ircode_len - i * max_send_len;
		else 
			send_len = max_send_len;
		}
		
		read_addr = read_buf.g_pkg_index.ircode_addr + offset_sum * read_buf.g_pkg_index.device_ircode_len + i * max_send_len;
		readflash_pt = flash_fread(read_addr, send_len);
		
		for (uint16_t i = 0; i < send_len / 4 ; i += 2 ){
			uint16_t tmp_data = 0xFFFF;
			if (memcmp(readflash_pt + i,&tmp_data,2) == 0 && memcmp(readflash_pt + i + 2 ,&tmp_data,2) == 0)
				err_cnt ++ ;
			else
				err_cnt = 0;
			
			if (err_cnt >= 3){
				ircode_catch_free_data();
				ircode_send_len = 0;
				flash_release_buf();
				return 0xE5;
			}
		}
		
		if (ircode_send_busy == 0xBB)
			memcpy(ircode_catch_get_p() + i * max_send_len/2 + ircode_send_len, readflash_pt, send_len);
		else
			memcpy(ircode_catch_get_p() + i * max_send_len/2, readflash_pt, send_len);
		
		ircode_send_len += send_len/2;
	}
	
	flash_release_buf();
	
	if (ircode_send_busy == 0x00){
		ircode_emit_start();
		return 0xCC;
	}
	return 0xE6;
}


//void ircode_emit_test (void )
//{
//	if (ircode_catch_init_data()!= 0xCC)
//		return ;
//	memcpy(ircode_catch_get_p(),test ,sizeof(test));
//	ircode_send_len = 11;
//	ircode_emit_start();
//}

static uint16_t transmissionLen = 0;
uint8_t ircode_emit_inarray(uint8_t status,uint8_t* inarrayCode,uint16_t ircodeTotalLen,uint8_t len)
{
	
	switch(status)
	{
		case 1:
		{
			if (transmissionLen < ircodeTotalLen && transmissionLen != 0 && ircode_catch_get_p() != NULL)
			{
					memcpy((uint8_t*)ircode_catch_get_p() + transmissionLen, inarrayCode, len);
					transmissionLen += len;
			}else{
				transmissionLen = 0;
				ircode_catch_free_data();
				return 0xE3;
			}
		}
			break;
		default:
		case 2:
		{
			if (ircodeTotalLen == transmissionLen && ircode_catch_get_p() != NULL )
			{
					ircode_send_len = transmissionLen/2;
					ircode_emit_start();
			}else{
					transmissionLen = 0;
					ircode_catch_free_data();
					return 0xE4;
			}
		}
			break;
		case 0:
		{
			transmissionLen = 0;
			if (ircode_catch_init_data()!= 0xCC)
				return 0xE1;
			if (ircodeTotalLen >= MAX_IRCODE_LEN || ircodeTotalLen == 0)
			{
				ircode_catch_free_data();
				return 0xE2;
			}
			memcpy((uint8_t*)ircode_catch_get_p(), inarrayCode, len);
			transmissionLen += len;
		}
			break;
	}
	return 0xCC;
}
uint16_t ircode_ircode_emit_getInArrayNum(void){
		return transmissionLen;
}
 
uint8_t ircode_emit_directSend(void){
		if (ircode_catch_get_p()== NULL || ircode_catch_data_len == 0){
			return 0xE1;
		}else{
			ircode_send_len = ircode_catch_data_len;
			ircode_emit_start();
			return 0xCC; 
		}
}

void ircode_emit_handler_timeout(){
		ircode_send_busy = 0;
		ircode_send_len = 0;
		ircode_send_position = 0;
		mx_tim14_stop();
		mx_tim3_pwm_deinit();
		ircode_catch_free_data();
}

void ircode_emit_start(void){
	
	HAL_INTASK_TIME_S TempTimeFuncNode;
	
	if (ircode_catch_get_p() == NULL)
		return ;
	
	Del_SubIREmitTask(ircode_emit_handler_timeout);
	memset(&TempTimeFuncNode, 0, sizeof(HAL_INTASK_TIME_S));
	TempTimeFuncNode.RunFuncPoint = ircode_emit_handler_timeout;
	TempTimeFuncNode.u32DelayTime = 30000;
	Add_SubIREmitTask(&TempTimeFuncNode);
	
	ircode_send_position = 0;
	ircode_send_busy = 0xBB;
	mx_tim3_pwm_init();
	
	ircode_nctoh(ircode_catch_get_p(),ircode_send_len);
	mx_tim14_setime(ircode_catch_get_p()[ircode_send_position]);
	((ircode_send_position % 2) == 0 ) ? mx_tim3_pwm_duty(30) : mx_tim3_pwm_duty(0);
	ircode_send_position++;
}




void ircode_emit_it_handler(void){
	if (LL_TIM_IsActiveFlag_UPDATE(TIM14)){
		LL_TIM_ClearFlag_UPDATE(TIM14);
		
		if(ircode_send_len > ircode_send_position){
			if (ircode_catch_get_p()[ircode_send_position] == 0xFFFF && ircode_catch_get_p()[ircode_send_position + 1] != 0xFFFF)
			{
				ircode_send_position++;
			}
			mx_tim14_setime(ircode_catch_get_p()[ircode_send_position]);
			((ircode_send_position % 2) == 0 ) ? mx_tim3_pwm_duty(30) : mx_tim3_pwm_duty(0);
			ircode_send_position++;
		}
		
		if (ircode_send_len <= ircode_send_position)
		{
			ircode_send_busy = 0;
			ircode_send_len = 0;
			ircode_send_position = 0;
			mx_tim14_stop();
			mx_tim3_pwm_deinit();
			ircode_catch_free_data();
			Del_SubIREmitTask(ircode_emit_handler_timeout);
		}
	}
}
