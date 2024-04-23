#ifndef __DRIVER_FLASH_H__
#define __DRIVER_FLASH_H__

#include "main.h"

/******************************************************************************
 *                               定义FLASH参数
*******************************************************************************/
#define STM32_FLASH_SIZE    32             //定义FLASH容量大小（单位为K字节）
#define STM32_SECTOR_SIZE   2048            //定义一个扇区的容量（字节）
#define STM32_FLASH_BASE    0x08000000      //定义FLASH的起始地址

/******************************************************************************
 *                                外部函数声明
*******************************************************************************/
extern uint64_t Int_flash_read_double_word(uint32_t read_addr);
extern uint32_t Int_flash_read_word(uint32_t read_addr);
extern uint16_t Int_flash_read_half_word(uint32_t read_addr);
extern uint8_t Int_flash_read_byte(uint32_t read_addr);
extern void Int_flash_read(uint32_t read_addr, uint64_t *pbuffer, uint16_t num_to_read);
extern void Int_flash_write(uint32_t write_addr, uint64_t *pbuffer, uint16_t num_to_write);
//extern void test_write(uint32_t write_addr, uint64_t write_data);
//extern void flash_init(void);

#endif
