#ifndef __25QFLASH_H__
#define __25QFLASH_H__

#include "stdint.h"

#ifndef FINFO_USE_READ_SFDP
	#define FINFO_PAGE_SIZE 256
	#define FINFO_SECTOR_SIZE 4096
	#define FINFO_BLOCK_SIZE 16
	#define FINFO_BLOCK_TOTAL 32
#endif

#define FINFO_CHIP_CLEAN_CMD_TYPE1 


#define FCTRL_OUTTIME	200

#define FMASK_MANUFACID 0x00FF0000
#define FMASK_MEMTYPE 	0x0000FF00
#define FMASK_MEMSIZE 	0x000000FF

	
#define  F_GET_MANUFACID(x)   	(uint8_t)((uint32_t)(x & FMASK_MANUFACID) >> (8*2))
#define  F_GET_MEMTYPE(x)   	(uint8_t)((uint32_t)(x & FMASK_MEMTYPE) >> (8*1))
#define  F_GET_MEMSIZE(x)   	(uint8_t)((uint32_t)(x & FMASK_MEMSIZE))

#define  F_STAT_MASK_SUS   	(1UL << 15)
#define  F_STAT_MASK_CMP   	(1UL << 14)
#define  F_STAT_MASK_LB3   	(1UL << 13)
#define  F_STAT_MASK_LB2   	(1UL << 12)
#define  F_STAT_MASK_LB1   	(1UL << 11)
#define  F_STAT_MASK_LB0  	(1UL << 10)
#define  F_STAT_MASK_QE  	(1UL << 9)
#define  F_STAT_MASK_SPR1   (1UL << 8)
#define  F_STAT_MASK_SPR0  	(1UL << 7)
#define  F_STAT_MASK_SEC  	(1UL << 6)
#define  F_STAT_MASK_TB  	(1UL << 5)
#define  F_STAT_MASK_BP2   	(1UL << 4)
#define  F_STAT_MASK_BP1   	(1UL << 3)
#define  F_STAT_MASK_BP0   	(1UL << 2)
#define  F_STAT_MASK_WEL   	(1UL << 1)
#define  F_STAT_MASK_BUSY   (1UL << 0)

#define F_IS_STAT_SET(x,y)	((uint32_t)(x & y & 0x0000FFFF) != 0 ? 1 : 0)

enum F_WRITE_STAT{
	F_WRITE_DISABLE,
	F_WRITE_ENABLE,
};

extern uint32_t falsh_init (void);
extern void flash_reset(void);
extern uint32_t falsh_get_jedec (void);

extern void flash_powdown (void);
extern void flash_wakeup (void);

extern uint32_t flash_read_jedec(void);
extern uint16_t flash_read_status(void);
extern uint64_t flash_read_uniqueid(void);
extern void flash_release_buf(void);
extern void flash_release_buf_w(void);
extern uint8_t* flash_nread(uint32_t memaddr,uint16_t len);
extern uint8_t* flash_fread(uint32_t memaddr,uint16_t len);

extern uint8_t flash_write(uint32_t memaddr,uint8_t* write_bufaddr,uint16_t len);

extern void flash_sector_erase(uint32_t memaddr);
extern void flash_block_erase_32kb(uint32_t memaddr);
extern void flash_block_erase_64kb(uint32_t memaddr);
extern void flash_chip_erase(void);
extern void flash_time_counter(void);

extern uint32_t flash_busy_stat_outime_checker_spt(uint16_t outime);
#endif
