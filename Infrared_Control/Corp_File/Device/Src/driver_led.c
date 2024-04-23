/******************************************************************************
 *                                  头文件
*******************************************************************************/
#include "driver_led.h"
#include "HAL_Control_GPIO.h"

#define  LED_CLASS_R  HAL_IOCLASS_A
#define  LED_PIN_R  HAL_IOPIN_0

#define  LED_CLASS_W  HAL_IOCLASS_A
#define  LED_PIN_W  HAL_IOPIN_12
/******************************************************************************
 * FunctionName : led_ind_onoff
 * Description  : 状态灯控制
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void led_ind_onoff(uint8_t num, uint8_t onoff)
{

    
}
static void led_74164_Control(uint8_t u8OutPutData)
{
    for (uint8_t i = 0; i < 8; i++){
        if(u8OutPutData & 0x80)
            HAL_IO_SetState(HAL_IOCLASS_A,HAL_IOPIN_0,HAL_IOSTA_SET);
        else
            HAL_IO_SetState(HAL_IOCLASS_A,HAL_IOPIN_0,HAL_IOSTA_RESET);
        
        HAL_IO_SetState(HAL_IOCLASS_A,HAL_IOPIN_1,HAL_IOSTA_SET);
        HAL_IO_SetState(HAL_IOCLASS_A,HAL_IOPIN_1,HAL_IOSTA_RESET);
        u8OutPutData = u8OutPutData << 1;
    }
    HAL_IO_SetState(HAL_IOCLASS_A,HAL_IOPIN_0,HAL_IOSTA_RESET);
    HAL_IO_SetState(HAL_IOCLASS_A,HAL_IOPIN_1,HAL_IOSTA_RESET);
}
/******************************************************************************
 * FunctionName : led_key_onoff
 * Description  : 按键背光灯控制
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void led_key_onoff(uint8_t num, uint8_t onoff, uint8_t off_mode)
{
    static uint8_t u8StoryLEDState = 0;

    if (onoff == 1 )
        u8StoryLEDState |= (1<<(num-1));
    else    
        u8StoryLEDState &= (~(1<<(num-1)));
    led_74164_Control(u8StoryLEDState);
}
