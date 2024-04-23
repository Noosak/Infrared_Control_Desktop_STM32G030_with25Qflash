#pragma once
#ifndef __IRCODE_EMIT_H__
#define __IRCODE_EMIT_H__ 

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern void ircode_emit_it_handler(void);
extern void ircode_emit_start(void);
extern uint8_t ircode_emit_addcode(uint8_t devnum,uint8_t* offset,uint8_t offset_total);
extern uint8_t ircode_emit_inarray(uint8_t status,uint8_t* inarrayCode,uint16_t ircodeTotalLen,uint8_t len);
extern uint16_t ircode_ircode_emit_getInArrayNum(void);
extern uint8_t ircode_emit_directSend(void);
#endif
