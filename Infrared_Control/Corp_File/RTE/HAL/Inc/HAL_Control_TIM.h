#pragma once
#ifndef __HAL_CONTROL_TIM_H__
#define __HAL_CONTROL_TIM_H__ 

#include "HAL_GlobalConfig.h"
#include "HAL_Control_Error.h"

typedef struct 
{
    HAL_FUNTIONPOINT_NRNA_P   HAL_TIM_INIT;
    HAL_FUNTIONPOINT_NRNA_P   HAL_TIM_HANDLER;
    HAL_FUNTIONPOINT_NREN_P   HAL_TIM_STASET;
    HAL_FUNTIONPOINT_NRU16_P  HAL_TIM_CUNTSET;
    HAL_FUNTIONPOINT_NRU16_P  HAL_TIM_DUTYSET;
}HAL_Control_TIM_Operation_t;

#if ( HAL_MAX_TIMER == 0 && HAL_MAX_TIMER == NULL)
    #error "ERROR!! 'HAL_MAX_TIMER' Is Zero Or Not Define in HAL_GlobalConfig.h"
#endif
    

extern HAL_Control_TIM_Operation_t gtTimerOperationArry [HAL_MAX_TIMER];

extern uint8_t HAL_Control_AllTimerInit (void);
#endif
