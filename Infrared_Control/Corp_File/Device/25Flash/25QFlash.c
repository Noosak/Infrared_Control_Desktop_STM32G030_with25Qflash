#include "25QFlash.h"
#include "HAL_Control_SPI.h"
#include "gpio.h"
#include "string.h"
#include "stdlib.h"
static uint32_t su32_flash_jedec;
static uint8_t* tmp_flash_opbuf = 0;
static uint32_t tmp_flash_opbuf_len = 0;
static volatile uint16_t tmp_flash_optimeout = 0;
static volatile uint8_t tmp_flash_optimeout_stat = 0;
static uint8_t* tmp_flash_wbuf = 0;


static uint32_t flash_busy_stat_outime_checker(void);

enum {
	FLASH_INTIMEOUT,
	FLASH_TIMEOUT,
};

static void flash_spi_wr(const uint8_t* tx_buf, uint8_t* rx_buf,uint32_t rw_len){
	LL_GPIO_ResetOutputPin(Flash_CS_GPIO_Port,Flash_CS_Pin);
	hal_control_spi_rw(tx_buf,rx_buf,rw_len);
	LL_GPIO_SetOutputPin(Flash_CS_GPIO_Port,Flash_CS_Pin);
}

uint32_t flash_read_jedec(void){
	uint8_t op_code[] = {0x9f,0x00,0x00,0x00};
	uint8_t rcv_code[sizeof(op_code)];
	flash_spi_wr(op_code,rcv_code,sizeof(op_code));
	su32_flash_jedec = (uint32_t)((rcv_code[1]<<8*2) + (rcv_code[2]<<8*1) + (rcv_code[3]));
	return su32_flash_jedec;
}

uint16_t flash_read_status(void){
	uint8_t op_code[] = {0x35,0x00};
	uint8_t rcv_code[sizeof(op_code)*2];
	flash_spi_wr(op_code,rcv_code,sizeof(op_code));
	op_code[0] = 0x05;
	op_code[1] = 0x00;
	flash_spi_wr(op_code,rcv_code + 2,sizeof(op_code));
	
	return (uint16_t)(((uint16_t)rcv_code[1] << 8) + rcv_code[3]);
}

void flash_reset(void){
	
	uint8_t op_code[] = {0x66};
	if(flash_busy_stat_outime_checker()!=0)
		return ;
	flash_spi_wr(op_code,op_code,sizeof(op_code));
	op_code[0] = 0x99;
	flash_spi_wr(op_code,op_code,sizeof(op_code));
}

uint64_t flash_read_uniqueid(void){
	uint8_t op_code[] = {0x4b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	if(flash_busy_stat_outime_checker()!=0)
		return 0;
	flash_spi_wr(op_code,op_code,sizeof(op_code));
	return (uint64_t)( ( ( (uint64_t)op_code[5] ) << 8*7) + ( ( (uint64_t)op_code[6]) << 8*6) + ( ( (uint64_t)op_code[7]) << 8*5) + ( ( (uint64_t)op_code[8]) << 8*4) \
			+ ( ( (uint64_t)op_code[9] ) << 8*3) + ( ( (uint64_t)op_code[10] ) << 8*2) + ( ( (uint64_t)op_code[11] ) << 8*1) + ( ( (uint64_t) op_code[12] ) ) );
}

void flash_writen (uint8_t enstat){
	uint8_t op_code[] = {0x00};
	if (enstat != 0)
		op_code[0] = 0x06;
	else
		op_code[0] = 0x04;
	flash_spi_wr(op_code,op_code,sizeof(op_code));
}

void flash_powdown (void){
	uint8_t op_code[] = {0xB9};
	if(flash_busy_stat_outime_checker()!=0)
		return ;
	flash_spi_wr(op_code,op_code,sizeof(op_code));
}

void flash_wakeup (void){
	uint8_t op_code[] = {0xAB,0x00,0x00,0x00,0x00};
	flash_spi_wr(op_code,op_code,sizeof(op_code));
}

uint8_t* flash_nread(uint32_t memaddr,uint16_t len){
	uint32_t tmp_lencnt = len + 4;
	
	if(flash_busy_stat_outime_checker()!=0)
		return NULL;
	
	if(tmp_flash_opbuf==0)
		tmp_flash_opbuf = (uint8_t*)malloc(tmp_lencnt);
	else
	{
		free(tmp_flash_opbuf);
		tmp_flash_opbuf = (uint8_t*)malloc(tmp_lencnt);
	}
	
	if (tmp_flash_opbuf == NULL)
		return NULL;
	
	
	memset(tmp_flash_opbuf, 0, tmp_lencnt);
	tmp_flash_opbuf[0] = 0x03;
	tmp_flash_opbuf[1] = (uint8_t)(( memaddr & 0x00FF0000 ) >> 8*2);
	tmp_flash_opbuf[2] = (uint8_t)(( memaddr & 0x0000FF00 ) >> 8*1);
	tmp_flash_opbuf[3] = (uint8_t)( memaddr & 0x000000FF ) ;
	
	flash_spi_wr(tmp_flash_opbuf, tmp_flash_opbuf, tmp_lencnt);
	tmp_flash_opbuf_len = len;
	return tmp_flash_opbuf + 4;
}	

uint8_t* flash_fread(uint32_t memaddr,uint16_t len){
	
#ifdef FINFO_READ_CANT_ACROSS_PAGE
	volatile uint8_t memaddr_align_fleft = memaddr % FINFO_PAGE_SIZE;
	volatile uint8_t memaddr_align_bleft = (memaddr_align_fleft + len) % FINFO_PAGE_SIZE;
	volatile uint8_t writeloop = (memaddr_align_fleft + len) / FINFO_PAGE_SIZE  + ((((memaddr_align_fleft + len) % FINFO_PAGE_SIZE) != 0) ? 1:0);
	volatile uint16_t addr_offset = 0;
	volatile uint16_t tem_wrt_len = 0;
	uint32_t ass_readbuf_len = 0;
	uint8_t* ass_readbuf = NULL;
	
	if(flash_busy_stat_outime_checker()!=0){
		return NULL;
	}
	if (writeloop == 1){
		ass_readbuf_len = len + 5;
	}else{
		ass_readbuf_len = FINFO_PAGE_SIZE + 5;
	}
	
	ass_readbuf = (uint8_t*)malloc(ass_readbuf_len);
	
	if (tmp_flash_opbuf == NULL)
		tmp_flash_opbuf = (uint8_t*)malloc(len);
	else
	{
		free(tmp_flash_opbuf);
		tmp_flash_opbuf = (uint8_t*)malloc(len);
	}
	
	if (tmp_flash_opbuf == NULL || ass_readbuf ==NULL){
		free(tmp_flash_opbuf);
		free(ass_readbuf);
		return NULL;
	}
	
	memset(ass_readbuf,0,ass_readbuf_len);
	memset(tmp_flash_opbuf,0,len);
	
	for (uint8_t i = 0; i < writeloop; i++ )
	{
		ass_readbuf[0] = 0x0B;
		ass_readbuf[1] = (uint8_t)(( (memaddr + FINFO_PAGE_SIZE * i - ((i != 0)? memaddr_align_fleft:0)) & 0x00FF0000 ) >> 8*2);
		ass_readbuf[2] = (uint8_t)(( (memaddr + FINFO_PAGE_SIZE * i - ((i != 0)? memaddr_align_fleft:0)) & 0x0000FF00 ) >> 8*1);
		ass_readbuf[3] = (uint8_t)(( (memaddr + FINFO_PAGE_SIZE * i - ((i != 0)? memaddr_align_fleft:0)) & 0x000000FF ) >> 8*0);
		ass_readbuf[4] = 0x00;
		
		if (i == 0){
			tem_wrt_len = (len < (FINFO_PAGE_SIZE - memaddr_align_fleft)) ? len:(FINFO_PAGE_SIZE - memaddr_align_fleft);
			flash_spi_wr(ass_readbuf, ass_readbuf, tem_wrt_len + 5);
			memcpy(tmp_flash_opbuf + addr_offset,ass_readbuf + 5,tem_wrt_len);
			addr_offset +=tem_wrt_len;
		}else{
			if (i == writeloop - 1){
				tem_wrt_len = memaddr_align_bleft;
				flash_spi_wr(ass_readbuf, ass_readbuf, tem_wrt_len + 5);
				memcpy(tmp_flash_opbuf + addr_offset,ass_readbuf + 5,tem_wrt_len);

			}else{
				tem_wrt_len = FINFO_PAGE_SIZE;
				flash_spi_wr(ass_readbuf, ass_readbuf, tem_wrt_len + 5);
				memcpy(tmp_flash_opbuf + addr_offset,ass_readbuf + 5,tem_wrt_len);
				addr_offset +=tem_wrt_len;
			}
		}
	}
	free(ass_readbuf);
	tmp_flash_opbuf_len = len;
	return tmp_flash_opbuf;
#else
	uint32_t tmp_lencnt = len + 5;
	
	if(flash_busy_stat_outime_checker_spt(100)!=0)
		return NULL;
	
	if(tmp_flash_opbuf==0)
		tmp_flash_opbuf = (uint8_t*)malloc(tmp_lencnt);
	else
	{
		free(tmp_flash_opbuf);
		tmp_flash_opbuf = (uint8_t*)malloc(tmp_lencnt);
	}
	
	if (tmp_flash_opbuf == NULL)
		return NULL;
	
	memset(tmp_flash_opbuf, 0, tmp_lencnt);
	tmp_flash_opbuf[0] = 0x0B;
	tmp_flash_opbuf[1] = (uint8_t)(( memaddr & 0x00FF0000 ) >> 8*2);
	tmp_flash_opbuf[2] = (uint8_t)(( memaddr & 0x0000FF00 ) >> 8*1);
	tmp_flash_opbuf[3] = (uint8_t)( memaddr & 0x000000FF ) ;
	tmp_flash_opbuf[4] = 0x00;
	flash_spi_wr(tmp_flash_opbuf, tmp_flash_opbuf, tmp_lencnt);
	tmp_flash_opbuf_len = len;
	return tmp_flash_opbuf + 5;
#endif
}	
uint32_t flash_get_readbuflen(void)
{
	return tmp_flash_opbuf_len;
}

void flash_release_buf(void)
{
	if(tmp_flash_opbuf != NULL){
		free(tmp_flash_opbuf);
		tmp_flash_opbuf = NULL;
		tmp_flash_opbuf_len = 0;
	}
}

void flash_release_buf_w(void)
{
	if(tmp_flash_wbuf != NULL){
		free(tmp_flash_wbuf);
		tmp_flash_wbuf = NULL;
	}
}

uint8_t flash_write(uint32_t memaddr,uint8_t* write_bufaddr,uint16_t len){
	volatile uint8_t memaddr_align_fleft = memaddr % FINFO_PAGE_SIZE;
	volatile uint8_t memaddr_align_bleft = (memaddr_align_fleft + len) % FINFO_PAGE_SIZE;
	volatile uint8_t writeloop = (memaddr_align_fleft + len) / FINFO_PAGE_SIZE  + ((((memaddr_align_fleft + len) % FINFO_PAGE_SIZE) != 0) ? 1:0);
	volatile uint16_t tem_wrt_len = 0;
	volatile uint16_t addr_offset = 0;
	uint32_t tmp_flash_wbuf_len = 0;
	
	if (writeloop == 1){
		tmp_flash_wbuf_len = len + 4;
	}else{
		tmp_flash_wbuf_len = FINFO_PAGE_SIZE + 4;
	}
	
	if(flash_busy_stat_outime_checker()!=0)
		return 0xEE;
	
	for (;;){
		if (flash_busy_stat_outime_checker_spt(1000) == FLASH_INTIMEOUT)
				break;
	}
	
	if (tmp_flash_wbuf == NULL)
		tmp_flash_wbuf = (uint8_t*)malloc(tmp_flash_wbuf_len);
	else
	{
		free(tmp_flash_wbuf);
		tmp_flash_wbuf = (uint8_t*)malloc(tmp_flash_wbuf_len);
	}
	
	if (tmp_flash_wbuf == NULL){
		free(tmp_flash_wbuf);
		return 0xEE;
	}
	memset(tmp_flash_wbuf,0,tmp_flash_wbuf_len);
	
	for (uint8_t i = 0; i < writeloop; i++ )
	{
		tmp_flash_wbuf[0] = 0x02;
		tmp_flash_wbuf[1] = (uint8_t)(( (memaddr + FINFO_PAGE_SIZE * i - ((i != 0)? memaddr_align_fleft:0)) & 0x00FF0000 ) >> 8*2);
		tmp_flash_wbuf[2] = (uint8_t)(( (memaddr + FINFO_PAGE_SIZE * i - ((i != 0)? memaddr_align_fleft:0)) & 0x0000FF00 ) >> 8*1);
		tmp_flash_wbuf[3] = (uint8_t)(( (memaddr + FINFO_PAGE_SIZE * i - ((i != 0)? memaddr_align_fleft:0)) & 0x000000FF ) >> 8*0);
		
		if (i == 0){
			tem_wrt_len = (len < (FINFO_PAGE_SIZE - memaddr_align_fleft)) ? len:(FINFO_PAGE_SIZE - memaddr_align_fleft);
			memcpy(tmp_flash_wbuf+4,write_bufaddr,tem_wrt_len);
			flash_writen(F_WRITE_ENABLE);
			flash_spi_wr(tmp_flash_wbuf, tmp_flash_wbuf, tem_wrt_len + 4);
			flash_writen(F_WRITE_DISABLE);
			addr_offset +=tem_wrt_len;
		}else{
			if(flash_busy_stat_outime_checker()!=0){
				break;
			}
			if (i == writeloop - 1){
				tem_wrt_len = memaddr_align_bleft;
				memcpy(tmp_flash_wbuf+4,write_bufaddr + addr_offset,tem_wrt_len);
				flash_writen(F_WRITE_ENABLE);
				flash_spi_wr(tmp_flash_wbuf, tmp_flash_wbuf, tem_wrt_len + 4);
				flash_writen(F_WRITE_DISABLE);
			}else{
				tem_wrt_len = FINFO_PAGE_SIZE;
				memcpy(tmp_flash_wbuf+4,write_bufaddr + addr_offset,FINFO_PAGE_SIZE);
				flash_writen(F_WRITE_ENABLE);
				flash_spi_wr(tmp_flash_wbuf, tmp_flash_wbuf, FINFO_PAGE_SIZE + 4);
				flash_writen(F_WRITE_DISABLE);
				addr_offset +=tem_wrt_len;
			}
		}
	}
	for (;;){
		if (flash_busy_stat_outime_checker_spt(1000) == FLASH_INTIMEOUT)
				return 0;
	}
}	

void flash_sector_erase(uint32_t memaddr){
	
	uint8_t op_code[4] = {0x00};
	op_code[0] = 0x20;
	op_code[1] = (uint8_t)(( memaddr & 0x00FF0000 ) >> 8*2);
	op_code[2] = (uint8_t)(( memaddr & 0x0000FF00 ) >> 8*1);
	op_code[3] = (uint8_t)( memaddr & 0x000000FF );
//	if(flash_busy_stat_outime_checker()!=0){
//		return ;
//	}
	flash_writen(F_WRITE_ENABLE);
	flash_spi_wr(op_code,op_code,sizeof(op_code));
	flash_writen(F_WRITE_DISABLE);
	for (;;){
			if (flash_busy_stat_outime_checker_spt(1000) == FLASH_INTIMEOUT)
				return ;
	}
}

void flash_block_erase_32kb(uint32_t memaddr){
	
	uint8_t op_code[4] = {0x00};
	op_code[0] = 0x52;
	op_code[1] = (uint8_t)(( memaddr & 0x00FF0000 ) >> 8*2);
	op_code[2] = (uint8_t)(( memaddr & 0x0000FF00 ) >> 8*1);
	op_code[3] = (uint8_t)( memaddr & 0x000000FF );
	if(flash_busy_stat_outime_checker()!=0){
		return ;
	}
	flash_writen(F_WRITE_ENABLE);
	flash_spi_wr(op_code,op_code,sizeof(op_code));
	flash_writen(F_WRITE_DISABLE);
	for (;;){
			if (flash_busy_stat_outime_checker_spt(1000) == FLASH_INTIMEOUT)
				return ;
	}
}

void flash_block_erase_64kb(uint32_t memaddr){
	
	uint8_t op_code[4] = {0x00};
	op_code[0] = 0xD8;
	op_code[1] = (uint8_t)(( memaddr & 0x00FF0000 ) >> 8*2);
	op_code[2] = (uint8_t)(( memaddr & 0x0000FF00 ) >> 8*1);
	op_code[3] = (uint8_t)( memaddr & 0x000000FF );
	if(flash_busy_stat_outime_checker()!=0){
		return ;
	}
	flash_writen(F_WRITE_ENABLE);
	flash_spi_wr(op_code,op_code,sizeof(op_code));
	flash_writen(F_WRITE_DISABLE);
}

void flash_chip_erase(void){
#ifdef FINFO_CHIP_CLEAN_CMD_TYPE1
	uint8_t op_code[] = {0xC7};
#else
	uint8_t op_code[] = {0x60};
#endif
	if(flash_busy_stat_outime_checker()!=0){
		return ;
	}
	flash_writen(F_WRITE_ENABLE);
	flash_spi_wr(op_code,op_code,sizeof(op_code));
	flash_writen(F_WRITE_DISABLE);
	for (;;){
			if (flash_busy_stat_outime_checker_spt(2000) == FLASH_INTIMEOUT)
				return ;
	}
}

uint32_t falsh_init (void) {
	hal_control_spi_init();
	LL_mDelay(5);
	flash_reset();
	LL_mDelay(5);
	su32_flash_jedec = flash_read_jedec();
	return su32_flash_jedec;
}


uint32_t falsh_get_jedec (void) {
	return su32_flash_jedec;
}

void flash_time_counter(void)
{	
	if (tmp_flash_optimeout_stat != 0)
		tmp_flash_optimeout++;
}

uint32_t flash_busy_stat_outime_checker(void){
	tmp_flash_optimeout_stat = 1;
	while(F_IS_STAT_SET(flash_read_status(),F_STAT_MASK_BUSY) != 0 ){
		if (tmp_flash_optimeout >= FCTRL_OUTTIME)
		{	
			tmp_flash_optimeout_stat = 0;
			tmp_flash_optimeout = 0;
			return FLASH_TIMEOUT;
		}
		LL_mDelay(1);
	}
	tmp_flash_optimeout_stat = 0;
	tmp_flash_optimeout = 0;
	return FLASH_INTIMEOUT;
}

uint32_t flash_busy_stat_outime_checker_spt(uint16_t outime){
	tmp_flash_optimeout_stat = 1;
	while(F_IS_STAT_SET(flash_read_status(),F_STAT_MASK_BUSY) != 0 ){
		if (tmp_flash_optimeout >= outime)
		{	
			tmp_flash_optimeout_stat = 0;
			tmp_flash_optimeout = 0;
			return FLASH_TIMEOUT;
		}
		LL_mDelay(50);
	}
	tmp_flash_optimeout_stat = 0;
	tmp_flash_optimeout = 0;
	return FLASH_INTIMEOUT;
}
