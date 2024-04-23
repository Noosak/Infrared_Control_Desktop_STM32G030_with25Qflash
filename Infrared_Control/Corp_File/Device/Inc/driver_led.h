#ifndef  __DRIVER_LED_H__
#define  __DRIVER_LED_H__

#include "main.h"

/******************************************************************************
 *                              ����״̬������
*******************************************************************************/
typedef enum
{
    LED_IND_W = 1,                 //�׵�
    LED_IND_R = 2,                 //���
    
} LED_IND_NAME;

/******************************************************************************
 *                              �ⲿ��������
*******************************************************************************/
extern void led_ind_onoff(uint8_t num, uint8_t onoff);
extern void led_init(void);
extern void led_key_onoff(uint8_t num, uint8_t onoff, uint8_t off_mode);

#endif
