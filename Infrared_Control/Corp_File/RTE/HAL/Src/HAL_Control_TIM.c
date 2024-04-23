#include "HAL_Control_TIM.h"

static void OBJ_TIM1_INIT(void);
static void OBJ_TIM1_Handler(void);
HAL_Control_TIM_Operation_t gtTimerOperationArry [HAL_MAX_TIMER] = {
    {OBJ_TIM1_INIT, OBJ_TIM1_Handler, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL},
};

uint8_t HAL_Control_AllTimerInit (void)
{
    uint8_t u8ErrCode;
    for (uint8_t i = 0; i < HAL_MAX_TIMER ; i++)
    {
        u8ErrCode = HAL_ERROR_FuncPointJumpArguCheck(gtTimerOperationArry[i].HAL_TIM_INIT);
        if (u8ErrCode != 0)
            return u8ErrCode;
    }
    return HAL_ERROR_NORMAL; 
}

uint8_t HAL_Control_Tim_Handler (uint8_t u8TimNum)
{
    if (NULL != gtTimerOperationArry[u8TimNum].HAL_TIM_HANDLER)
    {
       // gtTimerOperationArry[i].HAL_TIM_HANDLER();
    }
    else
    {
        return HAL_ERROR_NULL; 
    }
    return HAL_ERROR_NORMAL; 
}

void OBJ_TIM1_INIT()
{
//    LL_TIM_EnableIT_UPDATE(TIM14);
//    LL_TIM_EnableCounter(TIM14);
}

void OBJ_TIM1_Handler()
{
    
}

void OBJ_TIM1_State(HAL_STA_E InputState){
    switch (InputState){
        case HAL_STA_ENABLE:
            
            break;
        case HAL_STA_DISABLE:
            
            break;
        case HAL_STA_NULL:
        default :
            break;
            
    }
}


