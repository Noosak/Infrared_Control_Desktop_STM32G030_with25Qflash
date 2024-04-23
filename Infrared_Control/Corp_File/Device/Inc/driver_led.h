#ifndef  __DRIVER_LED_H__
#define  __DRIVER_LED_H__

#include "main.h"

/******************************************************************************
 *                              定义状态灯名称
*******************************************************************************/
typedef enum
{
    LED_IND_W = 1,                 //白灯
    LED_IND_R = 2,                 //红灯
    
} LED_IND_NAME;

/******************************************************************************
 *                              外部函数声明
*******************************************************************************/
extern void led_ind_onoff(uint8_t num, uint8_t onoff);
extern void led_init(void);
extern void led_key_onoff(uint8_t num, uint8_t onoff, uint8_t off_mode);

#endif
