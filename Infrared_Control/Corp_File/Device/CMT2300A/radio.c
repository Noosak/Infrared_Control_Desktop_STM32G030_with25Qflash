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
 * @file    radio.c
 * @brief   Generic radio handlers
 *
 * @version 1.2
 * @date    Jul 17 2017
 * @author  CMOSTEK R@D
 */
 
#include "radio.h"
#include "cmt2300a_params.h"
#include "driver_memory.h"
#include <string.h>

//EnumRFStatus g_nNextRFState = RF_STATE_IDLE;
#define RF_FREQ_OFS 'O'


uint8_t rf_txbuf[RF_DATA_LEN_MAX];              //发送缓存
uint8_t rf_rxbuf[RF_DATA_LEN_MAX];              //接收缓存
uint8_t rf_sen_fail_flag = 0;                   //发送失败标志
uint8_t rf_sen_fail_count = 0;                  //发送失败后重发间隔定时器
uint8_t rf_rec_ok_flag = 0;                     //接收数据有效标志
uint8_t rf_data_len = RF_DATA_LEN_MAX;          //数据长度

link_queue rf_rx_queue;                         //定义接收数据队列
link_queue rf_tx_queue;                         //定义发送数据队列


void RF_Init(void)
{
    u8 tmp;
    
    CMT2300A_InitGpio();
		CMT2300A_Init();
    
    /* Config registers */
    CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR       , g_cmt2300aCmtBank       , CMT2300A_CMT_BANK_SIZE       );
    CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR    , g_cmt2300aSystemBank    , CMT2300A_SYSTEM_BANK_SIZE    );
    CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , g_cmt2300aFrequencyBank , CMT2300A_FREQUENCY_BANK_SIZE );
    CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , g_cmt2300aDataRateBank  , CMT2300A_DATA_RATE_BANK_SIZE );
    CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR  , g_cmt2300aBasebandBank  , CMT2300A_BASEBAND_BANK_SIZE  );
    CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR        , g_cmt2300aTxBank        , CMT2300A_TX_BANK_SIZE        );
    
    // xosc_aac_code[2:0] = 2
    tmp = (~0x07) & CMT2300A_ReadReg(CMT2300A_CUS_CMT10);
    CMT2300A_WriteReg(CMT2300A_CUS_CMT10, tmp|0x02);
    
	RF_Config();
    g_nNextRFState = RF_STATE_IDLE;
}

void RF_Config(void)
{
#ifdef ENABLE_ANTENNA_SWITCH
    /* If you enable antenna switch, GPIO1/GPIO2 will output RX_ACTIVE/TX_ACTIVE,
       and it can't output INT1/INT2 via GPIO1/GPIO2 */
    CMT2300A_EnableAntennaSwitch(0);
    
#else
    /* Config GPIOs */
    CMT2300A_ConfigGpio(
        CMT2300A_GPIO1_SEL_INT1 | /* INT1 > GPIO1 */
        CMT2300A_GPIO2_SEL_INT2 | /* INT2 > GPIO2 */
        CMT2300A_GPIO3_SEL_DOUT
        );
    
    /* Config interrupt */
    CMT2300A_ConfigInterrupt(
        CMT2300A_INT_SEL_TX_DONE, /* Config INT1 */
        CMT2300A_INT_SEL_PKT_OK   /* Config INT2 */
        );
#endif

    /* Enable interrupt */
    CMT2300A_EnableInterrupt(
        CMT2300A_MASK_TX_DONE_EN  |
        CMT2300A_MASK_PREAM_OK_EN |
        CMT2300A_MASK_SYNC_OK_EN  |
        CMT2300A_MASK_NODE_OK_EN  |
        CMT2300A_MASK_CRC_OK_EN   |
        CMT2300A_MASK_PKT_DONE_EN
        );
    
    /* Disable low frequency OSC calibration */
    CMT2300A_EnableLfosc(FALSE);
    
    /* Use a single 64-byte FIFO for either Tx or Rx */
    //CMT2300A_EnableFifoMerge(TRUE);
    
    //CMT2300A_SetFifoThreshold(16);
    
    /* This is optional, only needed when using Rx fast frequency hopping */
    /* See AN142 and AN197 for details */
    //CMT2300A_SetAfcOvfTh(0x27);
    
    /* Go to sleep for configuration to take effect */
    CMT2300A_GoSleep();
    /*Go to Standby Mode*/
//    CMT2300A_GoStby();
//    CMT2300A_ClearInterruptFlags();
}

void RF_SetStatus(EnumRFStatus nStatus)
{
    g_nNextRFState = nStatus;
}

EnumRFStatus RF_GetStatus(void)
{
    return g_nNextRFState;
}

u8 RF_GetInterruptFlags(void)
{
    return g_nInterrutFlags;
}


void CMT2300A_Go2TXMode (void)
{
    
}
BOOL CMT2300A_Go2RXMode (void)
{
    CMT2300A_GoStby();
    CMT2300A_ClearInterruptFlags();
    CMT2300A_SetPktType(1);
        /* Must clear FIFO after enable SPI to read or write the FIFO */
    CMT2300A_EnableReadFifo();
    CMT2300A_ClearRxFifo();
    
	return CMT2300A_GoRx();
}

void CMT2300A_Device_Init (void)
{
    RF_Init();
    if(FALSE==CMT2300A_IsExist())
        while(1);
    
    CMT2300A_SetFrequencyStep(RF_FREQ_OFS);
    
    CMT2300A_Go2RXMode();
    rf_rx_queue = queue_init();                                     //创建无线接收数据队列头结点
    rf_tx_queue = queue_init();                                     //创建无线发送数据队列头结点

}

void CMT2300A_Device_ReInit(void)
{
    RF_Init();
    if(FALSE==CMT2300A_IsExist())
        while(1);
    
    CMT2300A_SetFrequencyStep(RF_FREQ_OFS);
    CMT2300A_SetFrequencyChannel(my_channel_num);
}
void RFSendPacket(uint8_t* txBuffer, uint8_t size)
{
		uint32_t u32TempTimeSave;
	
				
        CMT2300A_GoStby();
        CMT2300A_ClearInterruptFlags();
        CMT2300A_SetPktType(0);
        CMT2300A_SetPayloadLength(size);
        /* Must clear FIFO after enable SPI to read or write the FIFO */
        CMT2300A_EnableWriteFifo();
        CMT2300A_ClearTxFifo();
        
        /* The length need be smaller than 32 */
        CMT2300A_WriteFifo(txBuffer, size);
        
        if( 0==(CMT2300A_MASK_TX_FIFO_NMTY_FLG & CMT2300A_ReadReg(CMT2300A_CUS_FIFO_FLAG)) )
            g_nNextRFState = RF_STATE_ERROR;

        if(FALSE==CMT2300A_GoTx())
            g_nNextRFState = RF_STATE_ERROR;
        else
            g_nNextRFState = RF_STATE_TX_WAIT;
        rf_sen_fail_flag = 1;
				u32TempTimeSave = gu32TimeBase;
        while(g_nNextRFState != RF_STATE_TX_DONE){
            if(CMT2300A_ReadGpio1())  /* Read INT1, TX_DONE */
            {
                rf_sen_fail_flag = 0;
                g_nNextRFState = RF_STATE_TX_DONE;
            }
						else
						{
							if ((gu32TimeBase - u32TempTimeSave) > 500)
							{
								CMT2300A_Device_ReInit();
								CMT2300A_ClearRxFifo();
								CMT2300A_ClearTxFifo();
								rf_sen_fail_flag = 1;
								break;
							}
						}
        }
        
        CMT2300A_Go2RXMode();
}

