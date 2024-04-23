#pragma once
#ifndef __SERIALCTRL_HANDLER_H__
#define __SERIALCTRL_HANDLER_H__ 
#include <stdint.h>
#define INDEX_MAX_OFFSER 10
typedef struct{
	uint8_t device_num;
	uint8_t device_type;
	uint8_t device_ircode_stat;
	uint8_t device_offset_total;
	uint16_t device_ircode_len;
	uint32_t ircode_addr;
	uint32_t ircode_total;
	uint8_t offset[INDEX_MAX_OFFSER];
	
}PKG_INDEX;

union pkg_index_if{
	PKG_INDEX g_pkg_index;
	uint8_t inflash[sizeof(PKG_INDEX)];
};
extern void serial_inpkg(uint8_t cmd,uint8_t* data,uint8_t len);
extern void serial_msg_handler(uint8_t *msg,uint16_t* len);
#endif
