#ifndef __GPIO_DEFS_H
#define __GPIO_DEFS_H

#include "stm32g0xx_ll_gpio.h"

extern void ResetIOMode_Out (GPIO_TypeDef *GPIOx, uint32_t PinMask);
extern void ResetIOMode_In (GPIO_TypeDef *GPIOx, uint32_t PinMask);
#define CMT_CSB_GPIO                GPIOC
#define CMT_CSB_GPIO_PIN            LL_GPIO_PIN_15


#define CMT_FCSB_GPIO               GPIOA
#define CMT_FCSB_GPIO_PIN           LL_GPIO_PIN_12

#define CMT_SCLK_GPIO               GPIOA
#define CMT_SCLK_GPIO_PIN           LL_GPIO_PIN_0

#define CMT_SDIO_GPIO               GPIOA
#define CMT_SDIO_GPIO_PIN           LL_GPIO_PIN_1

#define CMT_GPIO1_GPIO              GPIOC
#define CMT_GPIO1_GPIO_PIN          LL_GPIO_PIN_14

#define CMT_GPIO2_GPIO              GPIOA
#define CMT_GPIO2_GPIO_PIN          LL_GPIO_PIN_11

#define SET_GPIO_OUT(x)             ResetIOMode_Out(x, x##_PIN)
#define SET_GPIO_IN(x)              ResetIOMode_In(x, x##_PIN)
#define SET_GPIO_H(x)               LL_GPIO_SetOutputPin(x, x##_PIN)
#define SET_GPIO_L(x)               LL_GPIO_ResetOutputPin(x, x##_PIN)
#define READ_GPIO_PIN(x)            (((x->IDR & x##_PIN) != 0) ?1 :0) //GPIO_ReadInputDataBit(x, x##_PIN) 

#endif
