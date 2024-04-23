#include "HAL_Control_ADC.h"
#include "stm32g0xx_ll_dma.h"
uint16_t gu16ADConvertedDataArry[HAL_ADC_MAXCHANNEL];


uint32_t HAL_ADC_Init(void)
{
//    LL_DMA_ConfigAddresses(DMA1,            \
//                            LL_DMA_CHANNEL_2,       \
//                            LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),       \
//                            (uint32_t)&gu16ADConvertedDataArry,     \
//                            LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
//    LL_DMA_SetDataLength(DMA1,      \
//                        LL_DMA_CHANNEL_2,       \
//                        HAL_ADC_MAXCHANNEL);	
//                       
//    LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_2);
//                       
//    LL_ADC_Enable(ADC1);
//	  
//	/* Start ADC group regular conversion */ 
//    LL_ADC_REG_StartConversion(ADC1);
//	  
//	LL_ADC_REG_SetDMATransfer(ADC1,LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
    return 0;
}

uint32_t HAL_ADC_ReqChannelDate(HAL_CONTROL_ADC_CHANNEL_E InputADChannel)
{
    return (uint32_t)gu16ADConvertedDataArry[(int)InputADChannel]; 
}
