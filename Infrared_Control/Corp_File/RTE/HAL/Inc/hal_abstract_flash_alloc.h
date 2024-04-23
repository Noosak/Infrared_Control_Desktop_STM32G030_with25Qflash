#ifndef __HAL_ABSTRACT_FLASH_ALLOC_H__
#define __HAL_ABSTRACT_FLASH_ALLOC_H__
#include "stdint.h"

#define AFINFO_SECTOR_SIZE 4096
#define AFINFO_MAXSIZE 4
#define AFINFO_STRT_FLASH_ADDR (0x00000000)

extern uint8_t falloc_opt_write(uint8_t index_num,uint16_t offset, uint8_t* writebuf, uint16_t len);
extern uint8_t falloc_opt_read(uint8_t index_num,uint16_t offset, uint8_t* readbuf, uint16_t len);
extern uint8_t falloc_opt_new(uint16_t len);
extern uint8_t falloc_opt_free(uint8_t index_num);
extern uint8_t fls_init(void);
#endif
