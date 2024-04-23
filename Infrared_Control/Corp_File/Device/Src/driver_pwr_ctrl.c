/******************************************************************************
 *                                  头文件
*******************************************************************************/
#include "driver_pwr_ctrl.h"
#include "HAL_Control_GPIO.h"
/******************************************************************************
 * FunctionName : pwr_ctrl_onoff
 * Description  : 电源控制
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void pwr_ctrl_onoff(uint8_t num, uint8_t onoff)
{
    IOCLASS_E POW_CON_X_PORT;
    IOPIN_E POW_CON_X_PIN;
//    GPIO_TypeDef *POW_CON_X_PORT;
//    uint32_t POW_CON_X_PIN;
    
    switch (num)
    {
        case 1 :
            POW_CON_X_PORT = HAL_IOCLASS_A;
            POW_CON_X_PIN = HAL_IOPIN_4;
            break;
        case 2 :
            POW_CON_X_PORT = HAL_IOCLASS_A;
            POW_CON_X_PIN = HAL_IOPIN_5;
            break;
        case 3 :
            POW_CON_X_PORT = HAL_IOCLASS_A;
            POW_CON_X_PIN = HAL_IOPIN_6;
            break;
        default :
            return ;
    }
    switch (onoff){
        case 0:
            HAL_IO_SetState(POW_CON_X_PORT,POW_CON_X_PIN,HAL_IOSTA_RESET);
            break;
        case 1:
            HAL_IO_SetState(POW_CON_X_PORT,POW_CON_X_PIN,HAL_IOSTA_SET);
            break;
        case 2:
            HAL_IO_SetState(POW_CON_X_PORT,POW_CON_X_PIN,HAL_IOSTA_TOGGLE);
            break;
    }
//    if (onoff == 2)
//    {
//        (LL_GPIO_IsInputPinSet(POW_CON_X_PORT, POW_CON_X_PIN)) ? (onoff = 0) : (onoff = 1);
//    }
//    
//    (onoff) ? (LL_GPIO_SetOutputPin(POW_CON_X_PORT, POW_CON_X_PIN)) :
//              (LL_GPIO_ResetOutputPin(POW_CON_X_PORT, POW_CON_X_PIN));
}

/******************************************************************************
 * FunctionName : pwr_ctrl_init
 * Description  : 电源控制功能初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void pwr_ctrl_init(void)
{
    pwr_ctrl_onoff(1, 0);                   //初始化电源控制状态
    pwr_ctrl_onoff(2, 0);
    pwr_ctrl_onoff(3, 0);
}
