/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, CMOSTEK SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (C) CMOSTEK SZ.
 */

/*!
 * @file    radio.h
 * @brief   Generic radio handlers
 *
 * @version 1.2
 * @date    Jul 17 2017
 * @author  CMOSTEK R@D
 */
 
#ifndef __RADIO_H
#define __RADIO_H

#include "system_queue.h"
#include "typedefs.h"
#include "cmt2300a.h"



#ifdef __cplusplus 
extern "C" { 
#endif

/* RF state machine */
typedef enum {
    RF_STATE_IDLE = 0,
    RF_STATE_RX_START,
    RF_STATE_RX_WAIT,
    RF_STATE_RX_DONE,
    RF_STATE_RX_TIMEOUT,
    RF_STATE_TX_START,
    RF_STATE_TX_WAIT,
    RF_STATE_TX_DONE,
    RF_STATE_TX_TIMEOUT,
    RF_STATE_ERROR,
} EnumRFStatus;

/* RF process function results */
typedef enum {
    RF_IDLE = 0,
    RF_BUSY,
    RF_RX_DONE,
    RF_RX_TIMEOUT,
    RF_TX_DONE,
    RF_TX_TIMEOUT,
    RF_ERROR,
} EnumRFResult;

//#define ENABLE_ANTENNA_SWITCH

void RF_Init(void);
void RF_Config(void);

void RF_SetStatus(EnumRFStatus nStatus);
EnumRFStatus RF_GetStatus(void);
u8 RF_GetInterruptFlags(void);

void RF_StartRx(u8 buf[], u16 len, u32 timeout);
void RF_StartTx(u8 buf[], u16 len, u32 timeout);

static u8 g_rxBuffer[32];   /* RF Rx buffer */

EnumRFResult RF_Process(void);

static volatile EnumRFStatus g_nNextRFState = RF_STATE_IDLE;
static u8* g_pRxBuffer = 0;
static u8* g_pTxBuffer = 0;
static u16 g_nRxLength = 0;
static u16 g_nTxLength = 0;
static u8 g_nInterrutFlags = 0;

/******************************************************************************
 *                            定义接收数据包最大长度
*******************************************************************************/
#define RF_DATA_LEN_MAX         32//64

/******************************************************************************
 *                              外部变量声明
*******************************************************************************/
extern uint8_t rf_txbuf[];
extern uint8_t rf_rxbuf[];
extern uint8_t rf_sen_fail_flag;
extern uint8_t rf_sen_fail_count;
extern uint8_t rf_rec_ok_flag;
extern uint8_t rf_data_len;

extern link_queue rf_rx_queue;
extern link_queue rf_tx_queue;

/******************************************************************************
 *                              外部函数声明
*******************************************************************************/
extern void CMT2300A_Device_Init(void);
extern void CMT2300A_TX_Done(uint8_t u8State);
extern void RFSendPacket(uint8_t txBuffer[], uint8_t size);
#ifdef __cplusplus 
} 
#endif

#endif
