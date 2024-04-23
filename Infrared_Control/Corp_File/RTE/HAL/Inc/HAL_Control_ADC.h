#pragma once
#ifndef __HAL_CONTROL_ADC_H__
#define __HAL_CONTROL_ADC_H__ 

#include "HAL_GlobalConfig.h"
#include "HAL_Control_Error.h"

#define HAL_ADC_MAXCHANNEL 3

typedef enum {
    HAL_ADC_CH1,
    HAL_ADC_CH2,
    HAL_ADC_CH3,
}HAL_CONTROL_ADC_CHANNEL_E;

extern uint32_t HAL_ADC_Init(void);
extern uint32_t HAL_ADC_ReqChannelDate(HAL_CONTROL_ADC_CHANNEL_E InputADChannel);
#endif
