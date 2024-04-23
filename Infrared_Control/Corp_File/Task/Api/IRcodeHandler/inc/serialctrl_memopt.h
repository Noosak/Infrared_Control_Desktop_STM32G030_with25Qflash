#pragma once
#ifndef __SERIALCTRL_MEMOPT_H__
#define __SERIALCTRL_MEMOPT_H__ 
#include <stdint.h>
#include "serialctrl_handler.h"
#include "hal_abstract_flash_alloc.h"

union MEMOPT_INDEX_SYMBOL{
	uint64_t index_locate_symbol;
	uint8_t symbol_in_flash[sizeof(uint64_t)];
};

#define MEMOPT_INDEX_ADDR_START (AFINFO_STRT_FLASH_ADDR + AFINFO_SECTOR_SIZE * AFINFO_MAXSIZE)

#define MEMOPT_INDEX_INDEX_START (MEMOPT_INDEX_ADDR_START + sizeof(uint64_t) + 1)

#define MEMOPT_INDEX_BLOCKSIZE sizeof(PKG_INDEX)
#define MEMOTP_INDEX_MAX 10

#define MEMOPT_INDEX_ADDR_END (MEMOPT_INDEX_ADDR_START + AFINFO_SECTOR_SIZE)

extern uint8_t memopt_index_init(void);
extern uint64_t memopt_index_get_index_mask(void);
extern uint8_t memopt_index_write(PKG_INDEX *st_write);
extern uint8_t memopt_index_read(uint8_t dev_num, PKG_INDEX *st_write);
extern uint8_t memopt_index_remove(uint8_t dev_num);
extern uint8_t memopt_index_mask_remove(void);
#endif
