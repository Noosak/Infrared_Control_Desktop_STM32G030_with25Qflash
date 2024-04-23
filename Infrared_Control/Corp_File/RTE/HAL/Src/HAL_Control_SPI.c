#include "HAL_Control_SPI.h" 
#include "spi.h"

void hal_control_spi_init (void)
{
	LL_SPI_SetRxFIFOThreshold(SPI1,LL_SPI_RX_FIFO_TH_QUARTER);
	
	LL_SPI_EnableDMAReq_RX(SPI1);
	LL_SPI_EnableDMAReq_TX(SPI1);
	

	
//	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)&hal_spi_rx_buf);
//    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)LL_SPI_DMA_GetRegAddr(SPI1));
//    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, 0);

////    /**** Init DMA Set TX buffer****/
//    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)&hal_spi_tx_buf);
//    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)LL_SPI_DMA_GetRegAddr(SPI1));
//    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, 0);

//	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
	
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
	LL_SPI_Enable(SPI1);
	while(LL_SPI_IsActiveFlag_BSY(SPI1));
	
}

void hal_control_spi_rw(const uint8_t* tx_buf, uint8_t* rx_buf,uint16_t rw_len)
{
	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)rx_buf);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)LL_SPI_DMA_GetRegAddr(SPI1));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, rw_len);

//    /**** Init DMA Set TX buffer****/
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)tx_buf);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)LL_SPI_DMA_GetRegAddr(SPI1));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, rw_len);
	
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
	while(!LL_DMA_IsActiveFlag_TC4(DMA1));
	while(!LL_DMA_IsActiveFlag_TC3(DMA1));
	LL_DMA_ClearFlag_TC4(DMA1);
	LL_DMA_ClearFlag_TC3(DMA1);
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
	LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);

}
