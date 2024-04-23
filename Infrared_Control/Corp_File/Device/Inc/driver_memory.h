#ifndef __DRIVER_MEMORY_H__
#define __DRIVER_MEMORY_H__

#include "main.h"
#include "driver_flash.h"
/******************************************************************************
 *                            定义设备信息存储地址
*******************************************************************************/
#define MEMORY_DEV_ADDR         (STM32_FLASH_BASE + STM32_SECTOR_SIZE * 16 - 8 - 1 * 8)
#define MEMORY_HOST_ADDR        (STM32_FLASH_BASE + STM32_SECTOR_SIZE * 16 - 8 - 2 * 8)
#define MEMORY_VERSION_NUM      (STM32_FLASH_BASE + STM32_SECTOR_SIZE * 16 - 8 - 3 * 8)
#define MEMORY_CHANNEL_NUM      (STM32_FLASH_BASE + STM32_SECTOR_SIZE * 16 - 8 - 4 * 8)
#define MEMORY_SP_MODE          (STM32_FLASH_BASE + STM32_SECTOR_SIZE * 16 - 8 - 5 * 8)
#define MEMORY_GROUP_FLAG       (STM32_FLASH_BASE + STM32_SECTOR_SIZE * 16 - 8 - 6 * 8)
#define MEMORY_DATE_AND_TIME    (STM32_FLASH_BASE + STM32_SECTOR_SIZE * 16 - 8 - 7 * 8)

/******************************************************************************
 *                              外部变量声明
*******************************************************************************/
extern uint64_t my_dev_addr;
extern uint64_t my_host_addr;
extern uint64_t my_version_num;
extern uint64_t my_channel_num;
extern uint64_t my_sp_mode;
extern uint64_t my_group_flag;
extern uint64_t my_date_and_time;

/******************************************************************************
 *                              外部函数声明
*******************************************************************************/
extern void memory_dev_addr_write(uint64_t dev_addr);
extern void memory_host_addr_write(uint64_t host_addr);
extern void memory_version_num_write(uint64_t version_num);
extern void memory_channel_num_write(uint64_t channel_num);
extern void memory_sp_mode_write(uint64_t sp_mode);
extern void memory_group_flag_write(uint64_t group_flag);
extern void memory_date_and_time_write(uint64_t date_and_time);
extern uint64_t memory_dev_addr_read(void);
extern uint64_t memory_host_addr_read(void);
extern uint64_t memory_version_num_read(void);
extern uint64_t memory_channel_num_read(void);
extern uint64_t memory_sp_mode_read(void);
extern uint64_t memory_group_flag_read(void);
extern uint64_t memory_date_and_time_read(void);
extern void memory_init(void);

#endif
