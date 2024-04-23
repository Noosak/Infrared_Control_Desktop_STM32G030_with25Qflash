#include "ircode_catch.h"
#include "serialctrl_handler.h"


uint8_t ircode_catch_stat = 0;
uint8_t ircode_catch_outime_cnt = 0;
uint16_t* ircode_catch_data_pt;
uint16_t ircode_catch_data_len = 0;

uint8_t ircode_send_pkgnum = 0;
uint8_t ircode_send_pkgtotal = 0;
void ircode_catch_free_data(void)
{
	if (ircode_catch_data_pt != NULL)
	{
		free(ircode_catch_data_pt);
		ircode_catch_data_pt = NULL;
	}
}

uint8_t ircode_catch_init_data(void)
{
	ircode_catch_free_data();
	
	ircode_catch_data_pt = (uint16_t*)malloc(MAX_IRCODE_LEN*(sizeof(uint16_t)));
	if (ircode_catch_data_pt != NULL){
		memset(ircode_catch_data_pt,0,MAX_IRCODE_LEN*(sizeof(uint16_t)));
		return 0xCC;
	}else
		return 0xEE;
}

uint16_t* ircode_catch_get_p (void)
{
	return ircode_catch_data_pt;
}

void ircode_htonc(uint16_t* data,uint16_t len){
	uint16_t tmp = 0;
	for (uint16_t i = 0; i < len; i++)
	{
		tmp = ((data[i] & 0x00FF) << 8) + ((data[i] & 0xFF00) >> 8);
		data[i] = tmp;
	}
}

void ircode_nctoh(uint16_t* data,uint16_t len){
	ircode_htonc(data,len);
}


void ircode_outime_it_handler (void){
	if (ircode_catch_stat == 2){
		ircode_catch_outime_cnt ++ ;
	}
	if (ircode_catch_outime_cnt > 1){
		
		
		LL_TIM_ClearFlag_UPDATE(TIM1);
		LL_TIM_ClearFlag_CC3(TIM1);
		
		LL_TIM_DisableIT_CC3(TIM1);
		LL_TIM_DisableIT_UPDATE(TIM1);
		LL_TIM_CC_DisableChannel(TIM1,LL_TIM_CHANNEL_CH3);
		LL_TIM_DisableCounter(TIM1);
		
		ircode_catch_outime_cnt = 0;
		ircode_catch_stat = 3;
		ircode_htonc(ircode_catch_data_pt,ircode_catch_data_len);
		ircode_catch_send();
	}
}

void ircode_catch_it_handler (uint16_t ircode_catch_data){
	
	if (ircode_catch_stat == 0)
	{
		ircode_catch_outime_cnt = 0;
		ircode_catch_data_len = 0;
		ircode_send_pkgnum = 0;
		ircode_send_pkgtotal = 0;
		ircode_catch_stat = 0;
		LL_TIM_ClearFlag_UPDATE(TIM1);
		LL_TIM_ClearFlag_CC3(TIM1);
		
		LL_TIM_DisableIT_CC3(TIM1);
		LL_TIM_DisableIT_UPDATE(TIM1);
		LL_TIM_CC_DisableChannel(TIM1,LL_TIM_CHANNEL_CH3);
		LL_TIM_DisableCounter(TIM1);
		return ;
	}
		
	
	ircode_catch_stat = 2;
	switch (ircode_catch_outime_cnt)
	{
		case 0 :{
			ircode_catch_data_pt[ircode_catch_data_len] = ircode_catch_data;
			ircode_catch_data_len ++ ;
			}
			break;
		case 1 :{
			ircode_catch_data_pt[ircode_catch_data_len] =0xFFFF;
			ircode_catch_data_len ++ ;
			ircode_catch_data_pt[ircode_catch_data_len] =0xFFFF;
			ircode_catch_data_len ++ ;
			ircode_catch_data_pt[ircode_catch_data_len] = ircode_catch_data;
			ircode_catch_data_len ++ ;
			}
			break;
		
		default:
			break;
	}
	if(ircode_catch_data_len%2 == 1)
		LL_TIM_IC_SetPolarity(TIM1,LL_TIM_CHANNEL_CH3,LL_TIM_IC_POLARITY_RISING);
	else
		LL_TIM_IC_SetPolarity(TIM1,LL_TIM_CHANNEL_CH3,LL_TIM_IC_POLARITY_FALLING);
}

uint8_t ircode_catch_enable(uint8_t enableState){
	ircode_catch_outime_cnt = 0;
	ircode_catch_data_len = 0;
	ircode_send_pkgnum = 0;
	ircode_send_pkgtotal = 0;
	if (enableState == 0x00){
		LL_TIM_ClearFlag_UPDATE(TIM1);
		LL_TIM_ClearFlag_CC3(TIM1);
		
		LL_TIM_DisableIT_CC3(TIM1);
		LL_TIM_DisableIT_UPDATE(TIM1);
		LL_TIM_CC_DisableChannel(TIM1,LL_TIM_CHANNEL_CH3);
		LL_TIM_DisableCounter(TIM1);
		
		ircode_catch_stat = 0;
		ircode_catch_free_data();
		return 0xCC;
	}else{
		if(ircode_catch_init_data() == 0xCC)
		{
			ircode_catch_stat = 1;
			
			LL_TIM_ClearFlag_UPDATE(TIM1);
			LL_TIM_ClearFlag_CC3(TIM1);
		
			LL_TIM_DisableIT_CC3(TIM1);
			LL_TIM_DisableIT_UPDATE(TIM1);
			LL_TIM_CC_DisableChannel(TIM1,LL_TIM_CHANNEL_CH3);
			LL_TIM_DisableCounter(TIM1);
			 /************************/
			
			LL_TIM_ClearFlag_UPDATE(TIM1);
			LL_TIM_ClearFlag_CC3(TIM1);
			
			LL_TIM_EnableIT_CC3(TIM1);
			LL_TIM_EnableIT_UPDATE(TIM1);
			LL_TIM_CC_EnableChannel(TIM1,LL_TIM_CHANNEL_CH3);
			LL_TIM_SetCounter(TIM1,0);
			LL_TIM_EnableCounter(TIM1);
			return 0xCC;
		}
		else
			return 0xEE;
	}
}

void ircode_catch_send (void)
{
	uint8_t max_send_len = 0;
	uint8_t send_len = 0;
	uint16_t real_sene_ircode_len = 0;
//	uint8_t send_pac_total = 0;
	real_sene_ircode_len = (ircode_catch_data_len - 1) * 2  ;
	uint8_t send_data[UINT8_MAX];
	if (ircode_catch_stat == 3){
		max_send_len = UINT8_MAX - 3;
		
		if (max_send_len > real_sene_ircode_len){
			send_len = ircode_catch_data_len;
		}else{
			if (max_send_len > real_sene_ircode_len - ircode_send_pkgnum * max_send_len)
				send_len = real_sene_ircode_len - ircode_send_pkgnum * max_send_len;
			else 
				send_len = max_send_len;
		}
		
		ircode_send_pkgtotal = (ircode_catch_data_len * 2 / max_send_len) + (((ircode_catch_data_len * 2 % max_send_len) > 0)? 1:0);
		send_data[0] = ircode_send_pkgnum;
		send_data[1] = ircode_send_pkgtotal;
		send_data[2] = send_len;
		memcpy(send_data+3,(uint8_t*)ircode_catch_data_pt + 2  + ircode_send_pkgnum * max_send_len, send_len);
		serial_inpkg(0x09,send_data,send_len + 3);
	}
	
}
