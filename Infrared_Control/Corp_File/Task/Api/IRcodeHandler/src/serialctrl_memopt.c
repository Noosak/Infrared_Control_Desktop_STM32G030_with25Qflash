#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "25QFlash.h"

#include "serialctrl_memopt.h"

union MEMOPT_INDEX_SYMBOL gu_memopt_index_symbol;

uint8_t memopt_index_remove(uint8_t dev_num){
	static uint8_t* free_tmp_buf = NULL;
	uint8_t tmp_data_buf[MEMOPT_INDEX_BLOCKSIZE * MEMOTP_INDEX_MAX + sizeof(uint64_t) + 1];
//	uint8_t farthest_index = 0xFF;
	union pkg_index_if read_buf;
	uint64_t tmp_index_mask = gu_memopt_index_symbol.index_locate_symbol;
	
	memset(&read_buf,0xFF,sizeof(union pkg_index_if));
	memset(&tmp_data_buf,0xFF,MEMOPT_INDEX_BLOCKSIZE * MEMOTP_INDEX_MAX + sizeof(uint64_t) + 1);
	if (((tmp_index_mask >> dev_num) & 1UL) == 1)
		return 0xe1;
//	for (uint8_t i = 0; i < MEMOTP_INDEX_MAX; i++){
//		if (((tmp_index_mask >> i) & 1UL) == 0)
//			farthest_index = i;
//	}
	flash_release_buf();
	free_tmp_buf = flash_fread(MEMOPT_INDEX_ADDR_START, MEMOPT_INDEX_BLOCKSIZE * MEMOTP_INDEX_MAX + sizeof(uint64_t) + 1);
	if (free_tmp_buf == NULL)
		return 0xe2;
	
	memcpy(tmp_data_buf,free_tmp_buf,MEMOPT_INDEX_BLOCKSIZE * MEMOTP_INDEX_MAX + sizeof(uint64_t) + 1);
	flash_release_buf();
	
	if (memopt_index_read(dev_num,&(read_buf.g_pkg_index)) == 0xee)
		return 0xe3;

	flash_sector_erase(MEMOPT_INDEX_ADDR_START);
	memset(tmp_data_buf + 1 + sizeof(uint64_t) + (MEMOPT_INDEX_BLOCKSIZE * dev_num ), 0xFF ,MEMOPT_INDEX_BLOCKSIZE);
	
	gu_memopt_index_symbol.index_locate_symbol |= (uint64_t)(1UL << dev_num );
	memset(tmp_data_buf,gu_memopt_index_symbol.index_locate_symbol,sizeof(uint64_t));
	flash_release_buf();
	uint8_t ret = flash_write(MEMOPT_INDEX_ADDR_START,tmp_data_buf,MEMOPT_INDEX_BLOCKSIZE * MEMOTP_INDEX_MAX + sizeof(uint64_t) + 1);
//	memopt_index_read(dev_num,&(read_buf.g_pkg_index));
//	memopt_index_read(1,&(read_buf.g_pkg_index));
	flash_release_buf();
	
	for(uint8_t i = 0; i < read_buf.g_pkg_index.ircode_total/4096; i++ ){
		flash_sector_erase(read_buf.g_pkg_index.ircode_addr + i*4096);
	}
	return 0xCC;
}

uint8_t memopt_index_read(uint8_t dev_num, PKG_INDEX *st_write){
	union pkg_index_if read_buf;
	static uint8_t* readinflash_pt = NULL;
	if (st_write == NULL || dev_num >= MEMOTP_INDEX_MAX)
		return 0xee;

	readinflash_pt = flash_fread(MEMOPT_INDEX_INDEX_START + MEMOPT_INDEX_BLOCKSIZE * dev_num, MEMOPT_INDEX_BLOCKSIZE);
	if  (readinflash_pt == NULL)
		return 0xee;
	memcpy(read_buf.inflash,readinflash_pt,MEMOPT_INDEX_BLOCKSIZE);
	memcpy(st_write,&(read_buf.g_pkg_index),MEMOPT_INDEX_BLOCKSIZE);
	flash_release_buf();
	return 0x00;
}

uint8_t memopt_index_write(PKG_INDEX *st_write){
	union pkg_index_if read_buf;
	PKG_INDEX if_read_buf;
	uint8_t* free_tmp_buf = NULL;
	uint64_t tmp_index_mask = gu_memopt_index_symbol.index_locate_symbol;
	uint8_t farthest_index = 0xFF;
	for (uint8_t i = 0; i < MEMOTP_INDEX_MAX; i++){
		if (((tmp_index_mask >> i) & 1UL) == 0)
			farthest_index = i;
	}
	
	if (st_write == NULL)
		return 0xee;
	memcpy(&(read_buf.g_pkg_index),st_write,MEMOPT_INDEX_BLOCKSIZE);
	if (((tmp_index_mask >> st_write->device_num) & 1UL) == 1)
	{
		if( flash_write(MEMOPT_INDEX_INDEX_START + MEMOPT_INDEX_BLOCKSIZE * (st_write->device_num), read_buf.inflash, MEMOPT_INDEX_BLOCKSIZE) != 0xEE)
		{
			gu_memopt_index_symbol.index_locate_symbol = gu_memopt_index_symbol.index_locate_symbol & (~(uint64_t)(1UL << st_write->device_num));
			return flash_write(MEMOPT_INDEX_ADDR_START,gu_memopt_index_symbol.symbol_in_flash,sizeof(uint64_t));
		}
		return 0xee;
	}else{
		if (memopt_index_read(st_write->device_num,&if_read_buf)== 0xee)
			return 0xee;
		read_buf.g_pkg_index.device_ircode_stat =0xFF;
		if(memcmp(&(read_buf.g_pkg_index),&if_read_buf,MEMOPT_INDEX_BLOCKSIZE)== 0 )
		{
			read_buf.g_pkg_index.device_ircode_stat = st_write->device_ircode_stat;
			return flash_write(MEMOPT_INDEX_INDEX_START + MEMOPT_INDEX_BLOCKSIZE * st_write->device_num, read_buf.inflash, MEMOPT_INDEX_BLOCKSIZE);
		}else{
//			free_tmp_buf = flash_fread(MEMOPT_INDEX_ADDR_START, MEMOPT_INDEX_BLOCKSIZE * (farthest_index + 1) + sizeof(uint64_t));
//			if (free_tmp_buf == NULL)
//					return 0xee;
//			flash_sector_erase(MEMOPT_INDEX_ADDR_START);
//			memcpy(free_tmp_buf + 1 + sizeof(uint64_t) + MEMOPT_INDEX_BLOCKSIZE*st_write->device_num, read_buf.inflash,MEMOPT_INDEX_BLOCKSIZE);
//			uint8_t ret = flash_write(MEMOPT_INDEX_ADDR_START,free_tmp_buf,MEMOPT_INDEX_BLOCKSIZE * (farthest_index + 1) + sizeof(uint64_t) + 1);
			return 0xEE;
		}
	}
}

uint8_t memopt_index_mask_remove(void)
{
	memset(&(gu_memopt_index_symbol.index_locate_symbol),0xFF,sizeof(uint64_t));
	flash_sector_erase(MEMOPT_INDEX_ADDR_START);
	return flash_busy_stat_outime_checker_spt(200);;
}

uint8_t memopt_index_init(void){
	uint8_t* tmpread_pt;
	uint8_t memen = 0xff;
	memset(&gu_memopt_index_symbol,0,sizeof(union MEMOPT_INDEX_SYMBOL));
//	tmpread_pt = flash_fread(MEMOPT_INDEX_ADDR_START,sizeof(union MEMOPT_INDEX_SYMBOL));
	flash_release_buf();
	memen = *(flash_fread(MEMOPT_INDEX_ADDR_START+sizeof(union MEMOPT_INDEX_SYMBOL),1));
	if (tmpread_pt == NULL)
		return 0xee;
	if (memen != 0xcc){
		
		flash_chip_erase();
		flash_busy_stat_outime_checker_spt(1000);
		memen = 0xcc;
		flash_write(MEMOPT_INDEX_ADDR_START+sizeof(union MEMOPT_INDEX_SYMBOL),&memen,1);
		
	}
	tmpread_pt = flash_fread(MEMOPT_INDEX_ADDR_START,sizeof(union MEMOPT_INDEX_SYMBOL));
	if (tmpread_pt == NULL)
		return 0xee;
	memcpy(gu_memopt_index_symbol.symbol_in_flash,tmpread_pt,sizeof(union MEMOPT_INDEX_SYMBOL));
	flash_release_buf();
	return 0x00;
}


uint64_t memopt_index_get_index_mask(void){
	return gu_memopt_index_symbol.index_locate_symbol;
}
