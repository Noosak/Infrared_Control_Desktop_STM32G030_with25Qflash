#pragma once
#ifndef __HAL_CONTROL_UART_H__
#define __HAL_CONTROL_UART_H__ 
#include <stdint.h>
#define SERIAL_BUFFER_SIZE 275

extern uint8_t u2_receive_buffer[SERIAL_BUFFER_SIZE];
extern uint16_t u2_rx_data_len;
extern void hal_control_uart2_interrupt(void);
extern void hal_control_uart2_send(uint8_t *write_buf,uint16_t len);
extern uint32_t HAL_Control_UartInit (void);
#endif
