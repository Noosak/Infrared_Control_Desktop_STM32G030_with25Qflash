#ifndef __HAL_CONTROL_SPI_H__
#define __HAL_CONTROL_SPI_H__
#include "stdint.h"

extern void hal_control_spi_init (void);
extern void hal_control_spi_rw(const uint8_t* tx_buf, uint8_t* rx_buf,uint16_t rw_len);

#endif
