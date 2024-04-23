#pragma once
#ifndef __HAL_CONTROL_GPIO_H__
#define __HAL_CONTROL_GPIO_H__ 

#include "stdint.h"
    typedef enum{
        HAL_IOCLASS_A,
        HAL_IOCLASS_B,
        HAL_IOCLASS_C,
        HAL_IOCLASS_D,
        HAL_IOCLASS_E,
        HAL_IOCLASS_F,
        HAL_IOCLASS_G,
    } IOCLASS_E;

    typedef enum{
        HAL_IOPIN_0,
        HAL_IOPIN_1,
        HAL_IOPIN_2,
        HAL_IOPIN_3,
        HAL_IOPIN_4,
        HAL_IOPIN_5,
        HAL_IOPIN_6,
        HAL_IOPIN_7,
        HAL_IOPIN_8,
        HAL_IOPIN_9,
        HAL_IOPIN_10,
        HAL_IOPIN_11,
        HAL_IOPIN_12,
        HAL_IOPIN_13,
        HAL_IOPIN_14,
        HAL_IOPIN_15,
    } IOPIN_E;

    typedef enum{
        HAL_IOSTA_RESET,
        HAL_IOSTA_SET,
        HAL_IOSTA_TOGGLE,
    } IOSTATE_E;

extern void HAL_IO_SetState (IOCLASS_E IOClass, IOPIN_E IOPin, IOSTATE_E IOState);
extern uint32_t HAL_IO_ReadPin (IOCLASS_E IOClass, IOPIN_E IOPin);    
extern void HAL_IO_Init(void);
#endif
