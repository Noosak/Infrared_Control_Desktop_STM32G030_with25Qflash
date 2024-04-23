#ifndef  __DRIVER_CC1101_H__
#define  __DRIVER_CC1101_H__

#include "main.h"
#include "system_queue.h"
/******************************************************************************
 *                              定义CC1101端口操作
*******************************************************************************/
#define CC1101_CS_OUT(x) (x) ? (LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4)) : (LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4))
#define CC1101_SOMI_IN   (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_6))
#define CC1101_GDO0_IN   (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_3))

/******************************************************************************
 *                            定义接收数据包最大长度
*******************************************************************************/
#define RF_DATA_LEN_MAX         48//64

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
extern void TI_CC_Wait(uint32_t cycles);
extern void TI_CC_SPISetup(void);
extern void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value);
extern void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count);
extern uint8_t TI_CC_SPIReadReg(uint8_t addr);
extern void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count);
extern uint8_t TI_CC_SPIReadStatus(uint8_t addr);
extern void TI_CC_SPIStrobe(uint8_t strobe);
extern void TI_CC_PowerupResetCCxxxx(void);

extern void writeRFSettings(void);
extern void RFSendPacket(uint8_t *, uint8_t);
extern uint8_t RFReceivePacket(uint8_t *, uint8_t *);

extern void cc1101_god0_int_cmd(uint8_t state);
extern void cc1101_set_channel(uint8_t channel);
extern void cc1101_enter_pwd(void);
extern void cc1101_rx_cal(void);
extern void cc1101_init(uint8_t init_type);

#endif
