#include "hal_abstract_flash_alloc.h"
#include "stdint.h"
#include "25QFlash.h"
#include "string.h"
#include "stdlib.h"

enum {
	FALLOC_CHECK_NOVSCT,
	FALLOC_CHECK_OVSCT,
};

enum {
	FALLOC_CHECK_NOVRNG,
	FALLOC_CHECK_OVRNG,
};

typedef struct FALLOC_INFO{
	uint32_t real_addr;
	uint32_t size;
	uint8_t index_num;
	struct FALLOC_INFO *next;
	struct FALLOC_INFO *forward;
}FALLOC_INFO;

struct FALLOC_MANAGER{
	FALLOC_INFO* falloc_index;
	uint16_t falloced_blockcnt;
	uint32_t falloc_offsetaddr;
	uint8_t falloc_index_cnt;
};

static struct FALLOC_MANAGER gfalloc_manager;
	
static uint8_t flc_check_out_of_range(uint32_t memaddr,uint8_t len){
	if ((memaddr + len - AFINFO_STRT_FLASH_ADDR) > AFINFO_SECTOR_SIZE * AFINFO_MAXSIZE)
		return FALLOC_CHECK_OVRNG;
	else
		return FALLOC_CHECK_NOVRNG;
}

uint8_t flc_write(uint32_t memaddr,uint8_t* write_bufaddr,uint16_t len)
{
	uint16_t tmp_out_of_range_len = 0;
	if (flc_check_out_of_range(memaddr,len) == FALLOC_CHECK_OVRNG)
	{
		tmp_out_of_range_len = (uint16_t)((uint32_t)(AFINFO_SECTOR_SIZE*AFINFO_MAXSIZE + AFINFO_STRT_FLASH_ADDR) - memaddr);
		if (flash_write(memaddr,write_bufaddr,tmp_out_of_range_len) != 0)
			return 0xee;
		if(flash_write(AFINFO_STRT_FLASH_ADDR,write_bufaddr + tmp_out_of_range_len,len - tmp_out_of_range_len) != 0)
			return 0xee;
	}
	else
	{
		if (flash_write(memaddr,write_bufaddr,len) != 0)
			return 0xee;
	}
	return 0; 
}

uint8_t flc_read(uint32_t memaddr,uint8_t* read_bufaddr,uint16_t len)
{
	uint8_t* tmp_bufpoint = NULL;
	uint16_t tmp_out_of_range_len = 0;
	if (flc_check_out_of_range(memaddr,len) == FALLOC_CHECK_OVRNG)
	{
		tmp_out_of_range_len = (uint16_t)((uint32_t)(AFINFO_SECTOR_SIZE*AFINFO_MAXSIZE + AFINFO_STRT_FLASH_ADDR) - memaddr);;
		tmp_bufpoint = flash_fread(memaddr, tmp_out_of_range_len);
		if(tmp_bufpoint == 0)
			return 0xEE;
		memcpy(read_bufaddr,tmp_bufpoint,tmp_out_of_range_len);
		tmp_bufpoint = NULL;
		
		tmp_bufpoint = flash_fread(AFINFO_STRT_FLASH_ADDR,len - tmp_out_of_range_len);
		if(tmp_bufpoint == 0)
			return 0xEE;
		memcpy(read_bufaddr + tmp_out_of_range_len,tmp_bufpoint,len - tmp_out_of_range_len);
	}
	else
	{
		tmp_bufpoint = flash_fread(memaddr, len);
		if(tmp_bufpoint == 0)
			return 0xEE;
		memcpy(read_bufaddr,tmp_bufpoint,len);
	}
	
	flash_release_buf();
	return 0;
}

static FALLOC_INFO* falloc_linklist_op_new(void){
	FALLOC_INFO* tmp_newpoint;
	FALLOC_INFO* tmp_jumppoint;
	tmp_newpoint=(FALLOC_INFO*)malloc(sizeof(FALLOC_INFO));
	if(tmp_newpoint == NULL)
		return NULL;
	memset(tmp_newpoint,0,sizeof(FALLOC_INFO));
	tmp_jumppoint = gfalloc_manager.falloc_index;
	for(uint16_t i = 0; i < gfalloc_manager.falloced_blockcnt; i++ ){
		if (tmp_jumppoint->next != NULL){
			tmp_jumppoint = tmp_jumppoint->next;
		}else
		break;
	}
	if (gfalloc_manager.falloced_blockcnt == 0)
		gfalloc_manager.falloc_index = tmp_newpoint;
	tmp_jumppoint->next = tmp_newpoint;
	tmp_newpoint->forward = tmp_jumppoint;
	gfalloc_manager.falloced_blockcnt++;
	return tmp_newpoint;
}

static uint8_t falloc_linklist_op_free(uint16_t index_num){
	FALLOC_INFO* tmp_jumppoint;
	
	tmp_jumppoint = gfalloc_manager.falloc_index;
	if(tmp_jumppoint == NULL)
		return 0xEE;
	for(uint16_t i = 0; i < gfalloc_manager.falloced_blockcnt;i++ ){
		if (tmp_jumppoint->index_num == index_num)
		{
			if (tmp_jumppoint->next != NULL)
				tmp_jumppoint->next->forward = tmp_jumppoint->forward;
				
			if (tmp_jumppoint->forward != NULL)
				tmp_jumppoint->forward->next = tmp_jumppoint->next;
	
			if(i == 0){
				gfalloc_manager.falloc_index = tmp_jumppoint->next;
			}
			free(tmp_jumppoint);
			gfalloc_manager.falloced_blockcnt--;
			return 0;
		}else{
			if (tmp_jumppoint->next != NULL){
				tmp_jumppoint = tmp_jumppoint->next;
			}
			else
				return 0xEE;
		}
	}
	return 0xEF;
}

uint8_t falloc_opt_write(uint8_t index_num,uint16_t offset, uint8_t* writebuf, uint16_t len){
	FALLOC_INFO* tmp_jumppoint;
	tmp_jumppoint = gfalloc_manager.falloc_index;
	for (uint8_t i = 0; i < gfalloc_manager.falloced_blockcnt; i++ ){
		if (tmp_jumppoint != NULL)
		{
			if (tmp_jumppoint->index_num == index_num)
			{
				return flc_write(tmp_jumppoint->real_addr + offset,writebuf,len);
			}else{
				if (tmp_jumppoint->next != NULL)
					tmp_jumppoint = tmp_jumppoint->next;
				else
					return 0xEE;
			}
		}
		else
			return 0xEE;
	}
	return 0xEE;
}

uint8_t falloc_opt_read(uint8_t index_num,uint16_t offset, uint8_t* readbuf, uint16_t len){
	FALLOC_INFO* tmp_jumppoint;
	tmp_jumppoint = gfalloc_manager.falloc_index;
	for (uint8_t i = 0; i < gfalloc_manager.falloced_blockcnt; i++ ){
		if (tmp_jumppoint != NULL)
		{
			if (tmp_jumppoint->index_num == index_num)
			{
				return flc_read(tmp_jumppoint->real_addr + offset,readbuf,len);
			}else{
				if (tmp_jumppoint->next != NULL)
					tmp_jumppoint = tmp_jumppoint->next;
				else
					return 0xEE;
			}
		}
		else
			return 0xEE;
	}
	return 0xEE;
}

uint8_t falloc_opt_new(uint16_t len){
	FALLOC_INFO* tmp_tl_pt = NULL;
	if (gfalloc_manager.falloc_offsetaddr + len > AFINFO_SECTOR_SIZE * AFINFO_MAXSIZE)
	{
		if (gfalloc_manager.falloced_blockcnt == 0){
			for(uint8_t i = 0; i < AFINFO_MAXSIZE; i++){
				flash_sector_erase(AFINFO_STRT_FLASH_ADDR + AFINFO_SECTOR_SIZE * i);
				if(flash_busy_stat_outime_checker_spt(100)!=0)
					return 0xee;
			}
			tmp_tl_pt = falloc_linklist_op_new();
			if (tmp_tl_pt == NULL)
				return 0x00;
			gfalloc_manager.falloc_offsetaddr = AFINFO_STRT_FLASH_ADDR;
			gfalloc_manager.falloc_index_cnt = 1;
			tmp_tl_pt->real_addr = AFINFO_STRT_FLASH_ADDR;
			tmp_tl_pt->index_num = gfalloc_manager.falloc_index_cnt;
			tmp_tl_pt->size = len;
			return tmp_tl_pt->index_num;
		}
		else
			/*****There Need Add Func******/
			return 0x00;	
			/***********/
	}
	else
	{
		tmp_tl_pt = falloc_linklist_op_new();
		if (tmp_tl_pt == NULL)
			return 0x00;
		tmp_tl_pt->real_addr = gfalloc_manager.falloc_offsetaddr;
		gfalloc_manager.falloc_offsetaddr += len;
		tmp_tl_pt->index_num = ++gfalloc_manager.falloc_index_cnt;
		tmp_tl_pt->size = len;
		return tmp_tl_pt->index_num;
	}
	
}

uint8_t falloc_opt_free(uint8_t index_num){
	return falloc_linklist_op_free(index_num);
}

uint8_t fls_init(void)
{
	if((AFINFO_MAXSIZE % AFINFO_SECTOR_SIZE)==0)
		return 0xee;
	memset(&gfalloc_manager,0,sizeof(gfalloc_manager));
	for(uint8_t i = 0; i < AFINFO_MAXSIZE; i++){
		flash_sector_erase(AFINFO_STRT_FLASH_ADDR + AFINFO_SECTOR_SIZE * i);
		if(flash_busy_stat_outime_checker_spt(100)!=0)
			return 0xee;
	}
	return 0x00;
	
}
