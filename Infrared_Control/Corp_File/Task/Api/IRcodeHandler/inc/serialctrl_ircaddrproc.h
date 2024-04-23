#pragma once
#ifndef __SERIALCTRL_IRCADDRPROC_H__
#define __SERIALCTRL_IRCADDRPROC_H__ 
#include <stdint.h>
#include "serialctrl_handler.h"

extern uint8_t mmc_ircode_addr_flash (void);
extern uint32_t mmc_ircode_addr_get (uint32_t len);
#endif
