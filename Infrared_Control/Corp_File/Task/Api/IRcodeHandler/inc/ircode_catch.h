#pragma once
#ifndef __IRCODE_CATCH_H__
#define __IRCODE_CATCH_H__ 

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32g0xx_ll_tim.h"


#define MAX_IRCODE_LEN (512 + 256)
extern uint8_t ircode_catch_stat;
extern uint8_t ircode_catch_outime_cnt;
extern uint16_t ircode_catch_data_len;
extern uint8_t ircode_send_pkgnum;
extern uint8_t ircode_send_pkgtotal;

extern void ircode_outime_it_handler (void);
extern void ircode_catch_it_handler (uint16_t ircode_catch_data);
extern uint8_t ircode_catch_init_data(void);
extern void ircode_catch_free_data(void);
extern uint16_t* ircode_catch_get_p (void);
extern uint8_t ircode_catch_init_data(void);
extern void ircode_catch_send(void);
extern uint8_t ircode_catch_enable(uint8_t enableState);
extern void ircode_htonc(uint16_t* data,uint16_t len);
extern void ircode_nctoh(uint16_t* data,uint16_t len);
#endif
