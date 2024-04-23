#include "HAL_Control_Uart.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_usart.h"
#include "string.h"
#include "serialctrl_handler.h"
uint8_t u2_receive_buffer[SERIAL_BUFFER_SIZE];
uint16_t u2_rx_data_len;
uint32_t HAL_Control_UartInit (void)
{
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)u2_receive_buffer);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)(&USART2->RDR));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, SERIAL_BUFFER_SIZE);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
//    /**** Init DMA Set TX buffer****/
//    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)u1_send_buffer);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)(&USART2->TDR));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, 0);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
	
    LL_USART_ClearFlag_TC(USART2);
//    LL_USART_EnableIT_TC(USART2);

    LL_USART_ClearFlag_IDLE(USART2);
    LL_USART_EnableIT_IDLE(USART2);
    LL_USART_EnableDMAReq_RX(USART2);
    LL_USART_EnableDMAReq_TX(USART2);
    return 0;
}

void hal_control_uart2_send(uint8_t *write_buf,uint16_t len)
{
	while(!LL_USART_IsActiveFlag_TXE(USART2));
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)write_buf);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)(&USART2->TDR));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, len);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
	while(!LL_USART_IsActiveFlag_TC(USART2));
	LL_USART_ClearFlag_TC(USART2);
}

void hal_control_uart2_interrupt(void){
    if (LL_USART_IsActiveFlag_IDLE(USART2))
    {
        LL_USART_ClearFlag_IDLE(USART2);
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
		u2_rx_data_len = SERIAL_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_1);       //获取接收数据长度
			
        if (u2_rx_data_len < 10)
        {
			memset(u2_receive_buffer,0,SERIAL_BUFFER_SIZE);
			u2_rx_data_len = 0;
		}
        LL_USART_RequestRxDataFlush(USART2);
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, SERIAL_BUFFER_SIZE);
		LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_1);
    }
    else if (LL_USART_IsActiveFlag_TC(USART2))
    {
        LL_USART_ClearFlag_TC(USART2);
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
        LL_USART_RequestTxDataFlush(USART2);
    }
    else if (LL_USART_IsActiveFlag_ORE(USART2)){
        LL_USART_ClearFlag_ORE(USART2);
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
        LL_USART_RequestRxDataFlush(USART2);
    }
}
