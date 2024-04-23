#pragma once
#ifndef __HAL_GLOBALCONFIG_H__
#define __HAL_GLOBALCONFIG_H__ 

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    HAL_STA_DISABLE,
    HAL_STA_ENABLE,
    HAL_STA_NULL = 0xFF,
}HAL_STA_E;

typedef void (*HAL_FUNTIONPOINT_NRNA_P) (void);
typedef void (*HAL_FUNTIONPOINT_NREN_P) (HAL_STA_E);
typedef void (*HAL_FUNTIONPOINT_NRU16_P) (uint16_t);
typedef void (*HAL_FUNTIONPOINT_NRU32_P) (uint32_t);

#define HAL_MAX_TIMER  2

#define Device_STM32G030



#endif
