/******************************************************************************
 *                                  头文件
*******************************************************************************/
#include "driver_cc1101.h"
#include "system_queue.h"
#include "TI_CC_CC1100-CC2500.h"
/******************************************************************************
 *                          定义RF发送和接收控制变量
*******************************************************************************/
uint8_t rf_txbuf[RF_DATA_LEN_MAX];              //发送缓存
uint8_t rf_rxbuf[RF_DATA_LEN_MAX];              //接收缓存
uint8_t rf_sen_fail_flag = 0;                   //发送失败标志
uint8_t rf_sen_fail_count = 0;                  //发送失败后重发间隔定时器
uint8_t rf_rec_ok_flag = 0;                     //接收数据有效标志
uint8_t rf_data_len = RF_DATA_LEN_MAX;          //数据长度

link_queue rf_rx_queue;                         //定义接收数据队列
link_queue rf_tx_queue;                         //定义发送数据队列

/******************************************************************************
 *                          定义RF发送功率配置参数
*******************************************************************************/
static uint8_t paTable[] = { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t paTableLen = 8;

/******************************************************************************
 * FunctionName : cc1101_exti_config
 * Description  : CC1101外部中断配置
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void cc1101_exti_config(void)
{
//    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTA, LL_EXTI_CONFIG_LINE3);
//    
//    LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_3);
//    LL_EXTI_DisableEvent_0_31(LL_EXTI_LINE_3);
//    
//    LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_0);
//    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_0);
//    
//    NVIC_SetPriority(EXTI0_1_IRQn, 1);
//    NVIC_EnableIRQ(EXTI0_1_IRQn);
      /**/
  LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTA, LL_EXTI_CONFIG_LINE3);
  LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_3);
  LL_EXTI_DisableEvent_0_31(LL_EXTI_LINE_3);
  
  LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_3);
  LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_3);
  
  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI2_3_IRQn, 1);
  NVIC_EnableIRQ(EXTI2_3_IRQn);
}

/******************************************************************************
 * FunctionName : cc1101_exti_cmd
 * Description  : CC1101外部中断控制
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void cc1101_exti_cmd(uint8_t state)
{
    if (state)  //使能EXTI线
    {  
        LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_3);
        LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_3);
    }
    else        //禁止EXTI线
        LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_3);
}

/******************************************************************************
 * FunctionName : cc1101_spi_read_write_byte
 * Description  : SPI2读写一个字节
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static uint8_t cc1101_spi_read_write_byte(uint8_t txdata)
{
//    uint8_t retry;
//    
//    retry = 0;
//    while (LL_SPI_IsActiveFlag_TXE(SPI1) == RESET)      //检查发送缓存是否空闲
//    {
//        retry++;
//        if (retry > 200)
//            return 0;
//    }
//    LL_SPI_TransmitData8(SPI1, txdata);                 //发送一个数据
//    
//    retry = 0;
//    while (LL_SPI_IsActiveFlag_RXNE(SPI1) == RESET)     //检查接收缓存是否非空
//    {
//        retry++;
//        if (retry > 200)
//            return 0;
//    }
//    return LL_SPI_ReceiveData8(SPI1);                   //返回接收到的一个数据
}

/******************************************************************************
 * FunctionName : cc1101_wait
 * Description  : 延时等待
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void cc1101_wait(uint32_t cycles)
{
    while (cycles > 15)                                 // 15 cycles consumed by overhead
    {
        cycles = cycles - 6;                            // 6 cycles consumed each iteration
    }
}

/******************************************************************************
 * FunctionName : cc1101_spi_setup
 * Description  : 初始化IO口和SPI总线
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void cc1101_spi_setup(void)
{
    cc1101_exti_config();                               //配置GDO0中断，默认禁止
    
    CC1101_CS_OUT(1);                                   // /CS disable
}

/******************************************************************************
 * FunctionName : cc1101_spi_write_reg
 * Description  : 向指定地址的配置寄存器写入一个数据
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void cc1101_spi_write_reg(uint8_t addr, uint8_t value)
{
    CC1101_CS_OUT(0);                                   // /CS enable
    while (CC1101_SOMI_IN != RESET)                     // Wait CCxxxx ready
        ;
    
    cc1101_spi_read_write_byte(addr);                   // Send address
    
    cc1101_spi_read_write_byte(value);                  // Send data
    
    CC1101_CS_OUT(1);                                   // /CS disable
}

/******************************************************************************
 * FunctionName : cc1101_spi_write_burst_reg
 * Description  : 向指定起始地址的配置寄存器连续写入数据
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void cc1101_spi_write_burst_reg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
    uint8_t i;
    
    CC1101_CS_OUT(0);                                   // /CS enable
    while (CC1101_SOMI_IN != RESET)                     // Wait CCxxxx ready
        ;
    
    addr = addr | TI_CCxxx0_WRITE_BURST;
    cc1101_spi_read_write_byte(addr);                   // Send address
    
    for (i = 0; i < count; i++)                         // Send data
    {
        cc1101_spi_read_write_byte(buffer[i]);
    }
    
    CC1101_CS_OUT(1);                                   // /CS disable
}

/******************************************************************************
 * FunctionName : cc1101_spi_read_reg
 * Description  : 从指定地址的配置寄存器读出一个数据
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static uint8_t cc1101_spi_read_reg(uint8_t addr)
{
    uint8_t x;
    
    CC1101_CS_OUT(0);                                   // /CS enable
    while (CC1101_SOMI_IN != RESET)                     // Wait CCxxxx ready
        ;
    
    addr = addr | TI_CCxxx0_READ_SINGLE;
    cc1101_spi_read_write_byte(addr);                   // Send address
    
    x = cc1101_spi_read_write_byte(0xFF);               // Read data
    
    CC1101_CS_OUT(1);                                   // /CS disable
    
    return x;
}

/******************************************************************************
 * FunctionName : cc1101_spi_read_burst_reg
 * Description  : 从指定起始地址的配置寄存器连续读出数据
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void cc1101_spi_read_burst_reg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
    uint8_t i;
    
    CC1101_CS_OUT(0);                                   // /CS enable
    while (CC1101_SOMI_IN != RESET)                     // Wait CCxxxx ready
        ;
    
    addr = addr | TI_CCxxx0_READ_BURST;
    cc1101_spi_read_write_byte(addr);                   // Send address
    
    for (i = 0; i < count; i++)                         // Read data
    {
        buffer[i] = cc1101_spi_read_write_byte(0xFF);
    }
    
    CC1101_CS_OUT(1);                                   // /CS disable
}

/******************************************************************************
 * FunctionName : cc1101_spi_read_status
 * Description  : 从指定地址的状态寄存器中读出一个数据
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static uint8_t cc1101_spi_read_status(uint8_t addr)
{
    uint8_t x;
    
    CC1101_CS_OUT(0);                                   // /CS enable
    while (CC1101_SOMI_IN != RESET)                     // Wait CCxxxx ready
        ;
    
    addr = addr | TI_CCxxx0_READ_BURST;
    cc1101_spi_read_write_byte(addr);                   // Send address
    
    x = cc1101_spi_read_write_byte(0xFF);               // Read data
    
    CC1101_CS_OUT(1);                                   // /CS disable
    
    return x;
}

/******************************************************************************
 * FunctionName : cc1101_spi_strobe
 * Description  : 向命令寄存器写入一个数据
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void cc1101_spi_strobe(uint8_t strobe)
{
    CC1101_CS_OUT(0);                                   // /CS enable
    while (CC1101_SOMI_IN != RESET)                     // Wait CCxxxx ready
        ;
    
    cc1101_spi_read_write_byte(strobe);                 // Send strobe
    
    CC1101_CS_OUT(1);                                   // /CS disable
}

/******************************************************************************
 * FunctionName : cc1101_power_up_reset
 * Description  : 芯片硬复位
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void cc1101_power_up_reset(void)
{
    uint8_t strobe;
    
    CC1101_CS_OUT(1);                                   // /CS disable
    cc1101_wait(30);
    CC1101_CS_OUT(0);                                   // /CS enable
    cc1101_wait(30);
    CC1101_CS_OUT(1);                                   // /CS disable
    cc1101_wait(45);
    CC1101_CS_OUT(0);                                   // /CS enable
    while (CC1101_SOMI_IN != RESET)                     // Wait CCxxxx ready
        ;
    
    strobe = TI_CCxxx0_SRES;
    cc1101_spi_read_write_byte(strobe);                 // Send strobe
    
    while (CC1101_SOMI_IN != RESET)                     // Wait CCxxxx ready
        ;
    
    CC1101_CS_OUT(1);                                   // /CS disable
}

/******************************************************************************
 *                             IT_CC_spi.c的函数封装
*******************************************************************************/
//----------------------------------------------------------------------------
//  void TI_CC_SPISetup(void)
//
//  DESCRIPTION:
//  Configures the assigned interface to function as a SPI port and
//  initializes it.
//----------------------------------------------------------------------------
//  void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
//
//  DESCRIPTION:
//  Writes "value" to a single configuration register at address "addr".
//----------------------------------------------------------------------------
//  void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
//
//  DESCRIPTION:
//  Writes values to multiple configuration registers, the first register being
//  at address "addr".  First data byte is at "buffer", and both addr and
//  buffer are incremented sequentially (within the CCxxxx and MSP430,
//  respectively) until "count" writes have been performed.
//----------------------------------------------------------------------------
//  uint8_t TI_CC_SPIReadReg(uint8_t addr)
//
//  DESCRIPTION:
//  Reads a single configuration register at address "addr" and returns the
//  value read.
//----------------------------------------------------------------------------
//  void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
//
//  DESCRIPTION:
//  Reads multiple configuration registers, the first register being at address
//  "addr".  Values read are deposited sequentially starting at address
//  "buffer", until "count" registers have been read.
//----------------------------------------------------------------------------
//  uint8_t TI_CC_SPIReadStatus(uint8_t addr)
//
//  DESCRIPTION:
//  Special read function for reading status registers.  Reads status register
//  at register "addr" and returns the value read.
//----------------------------------------------------------------------------
//  void TI_CC_SPIStrobe(uint8_t strobe)
//
//  DESCRIPTION:
//  Special write function for writing to command strobe registers.  Writes
//  to the strobe at address "addr".
//----------------------------------------------------------------------------
void TI_CC_Wait(uint32_t cycles)
{
    cc1101_wait(cycles);
}
//----------------------------------------------------------------------------
void TI_CC_SPISetup(void)
{
    cc1101_spi_setup();
}
//----------------------------------------------------------------------------
void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
    cc1101_spi_write_reg(addr, value);
}
//----------------------------------------------------------------------------
void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
    cc1101_spi_write_burst_reg(addr, buffer, count);
}
//----------------------------------------------------------------------------
uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
    return cc1101_spi_read_reg(addr);
}
//----------------------------------------------------------------------------
void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
    cc1101_spi_read_burst_reg(addr, buffer, count);
}
//----------------------------------------------------------------------------
uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
    return cc1101_spi_read_status(addr);
}
//----------------------------------------------------------------------------
void TI_CC_SPIStrobe(uint8_t strobe)
{
    cc1101_spi_strobe(strobe);
}
//----------------------------------------------------------------------------
void TI_CC_PowerupResetCCxxxx(void)
{
    cc1101_power_up_reset();
}
//----------------------------------------------------------------------------

/******************************************************************************
 *                             CC1100-CC2500.c的函数封装
*******************************************************************************/
//------------------------------------------------------------------------------
//  void writeRFSettings(void)
//
//  DESCRIPTION:
//  Used to configure the CCxxxx registers.  There are five instances of this
//  function, one for each available carrier frequency.  The instance compiled
//  is chosen according to the system variable TI_CC_RF_FREQ, assigned within
//  the header file "TI_CC_hardware_board.h".
//
//  ARGUMENTS:
//      none
//------------------------------------------------------------------------------
//  void RFSendPacket(uint8_t *txBuffer, uint8_t size)
//
//  DESCRIPTION:
//  This function transmits a packet with length up to 63 bytes.  To use this
//  function, GD00 must be configured to be asserted when sync word is sent and
//  de-asserted at the end of the packet, which is accomplished by setting the
//  IOCFG0 register to 0x06, per the CCxxxx datasheet.  GDO0 goes high at
//  packet start and returns low when complete.  The function polls GDO0 to
//  ensure packet completion before returning.
//
//  ARGUMENTS:
//      uint8_t *txBuffer
//          Pointer to a buffer containing the data to be transmitted
//
//      uint8_t size
//          The size of the txBuffer
//-----------------------------------------------------------------------------
//  uint8_t RFReceivePacket(uint8_t *rxBuffer, uint8_t *length)
//
//  DESCRIPTION:
//  Receives a packet of variable length (first byte in the packet must be the
//  length byte).  The packet length should not exceed the RXFIFO size.  To use
//  this function, APPEND_STATUS in the PKTCTRL1 register must be enabled.  It
//  is assumed that the function is called after it is known that a packet has
//  been received; for example, in response to GDO0 going low when it is
//  configured to output packet reception status.
//
//  The RXBYTES register is first read to ensure there are bytes in the FIFO.
//  This is done because the GDO signal will go high even if the FIFO is flushed
//  due to address filtering, CRC filtering, or packet length filtering.
//
//  ARGUMENTS:
//      uint8_t *rxBuffer
//          Pointer to the buffer where the incoming data should be stored
//      uint8_t *length
//          Pointer to a variable containing the size of the buffer where the
//          incoming data should be stored. After this function returns, that
//          variable holds the packet length.
//
//  RETURN VALUE:
//      uint8_t
//          0x80:  CRC OK
//          0x00:  CRC NOT OK (or no pkt was put in the RXFIFO due to filtering)
//-----------------------------------------------------------------------------
void writeRFSettings(void) 
{
#ifndef CC1101_NEW_CFG
    TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG2,   0x29);    // GDO2 output pin config.
    TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG1,   0x2E);
    TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG0,   0x06);    // GDO0 output pin config.
    TI_CC_SPIWriteReg(TI_CCxxx0_FIFOTHR,  0x47);    // RXFIFO and TXFIFO thresholds.
    TI_CC_SPIWriteReg(TI_CCxxx0_SYNC1,    'L');     // L
    TI_CC_SPIWriteReg(TI_CCxxx0_SYNC0,    'S');     // S
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTLEN,   0x3D);    // Packet length.
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL1, 0x0C);    // Packet automation control.
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x05);    // Packet automation control.    
    TI_CC_SPIWriteReg(TI_CCxxx0_ADDR,     0x00);    // Device address.    
    TI_CC_SPIWriteReg(TI_CCxxx0_CHANNR,   0x00);    // Channel number.  
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL1,  0x06);    // Freq synthesizer control.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL0,  0x00);    // Freq synthesizer control.
    TI_CC_SPIWriteReg(TI_CCxxx0_FREQ2,    0x10);    // Freq control word, high byte
    TI_CC_SPIWriteReg(TI_CCxxx0_FREQ1,    0xA7);    // Freq control word, mid byte.
    TI_CC_SPIWriteReg(TI_CCxxx0_FREQ0,    0x62);    // Freq control word, low byte. 
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG4,  0xDA);    // Modem configuration.    
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG3,  0x83);    // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG2,  0x13);    // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG1,  0x22);    // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG0,  0xF8);    // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_DEVIATN,  0x34);    // Modem dev (when FSK mod en)
    TI_CC_SPIWriteReg(TI_CCxxx0_MCSM2 ,   0x07);    // Until end of packet 
    TI_CC_SPIWriteReg(TI_CCxxx0_MCSM1 ,   0x30);    // reception sent over IDLE
    TI_CC_SPIWriteReg(TI_CCxxx0_MCSM0 ,   0x18);    // MainRadio Cntrl State Machine
    TI_CC_SPIWriteReg(TI_CCxxx0_FOCCFG,   0x16);    // Freq Offset Compens. Config
    TI_CC_SPIWriteReg(TI_CCxxx0_BSCFG,    0x6C);    // Bit synchronization config.
    TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL2, 0x03);    // AGC control.
    TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL1, 0x40);    // AGC control.
    TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL0, 0x91);    // AGC control.
    TI_CC_SPIWriteReg(TI_CCxxx0_WOREVT1,  0x28);
    TI_CC_SPIWriteReg(TI_CCxxx0_WOREVT0,  0xA0);    // Event0 = 300ms, RX = 585us
    TI_CC_SPIWriteReg(TI_CCxxx0_WORCTRL,  0xC8);    // Event1 = 0.444ms,RC_PD= 1,RC_CAL=1;
    TI_CC_SPIWriteReg(TI_CCxxx0_FREND1,   0x56);    // Front end RX configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_FREND0,   0x10);    // Front end RX configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL3,   0xE9);    // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL2,   0x2A);    // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL1,   0x00);    // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL0,   0x1F);    // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_RCCTRL1,  0x41);    // RC Oscillator Configuration
    TI_CC_SPIWriteReg(TI_CCxxx0_RCCTRL0,  0x00);    // RC Oscillator Configuration
    TI_CC_SPIWriteReg(TI_CCxxx0_FSTEST,   0x59);    // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_PTEST,    0x7F);    // Production Test
    TI_CC_SPIWriteReg(TI_CCxxx0_AGCTEST,  0x3F);    // AGC Test
    TI_CC_SPIWriteReg(TI_CCxxx0_TEST2,    0x81);    // Various test settings.
    TI_CC_SPIWriteReg(TI_CCxxx0_TEST1,    0x35);    // Various test settings.
    TI_CC_SPIWriteReg(TI_CCxxx0_TEST0,    0x09);    // Various test settings. 
#else
    //数据白化、发码固定包长、前向纠错
    TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG2,   0x29);    // GDO2 output pin config.
    TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG1,   0x2E);
    TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG0,   0x06);    // GDO0 output pin config.
    TI_CC_SPIWriteReg(TI_CCxxx0_FIFOTHR,  0x47);    // RXFIFO and TXFIFO thresholds.
    TI_CC_SPIWriteReg(TI_CCxxx0_SYNC1,    'L');     // L
    TI_CC_SPIWriteReg(TI_CCxxx0_SYNC0,    'S');     // S
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTLEN,   0x3D);    // Packet length.
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL1, 0x0C);    // Packet automation control.
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x45);    // Packet automation control.    
    TI_CC_SPIWriteReg(TI_CCxxx0_ADDR,     0x00);    // Device address.    
    TI_CC_SPIWriteReg(TI_CCxxx0_CHANNR,   0x00);    // Channel number.  
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL1,  0x06);    // Freq synthesizer control.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL0,  0x00);    // Freq synthesizer control.
    TI_CC_SPIWriteReg(TI_CCxxx0_FREQ2,    0x10);    // Freq control word, high byte
    TI_CC_SPIWriteReg(TI_CCxxx0_FREQ1,    0xA7);    // Freq control word, mid byte.
    TI_CC_SPIWriteReg(TI_CCxxx0_FREQ0,    0x62);    // Freq control word, low byte. 
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG4,  0xDA);    // Modem configuration.    
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG3,  0x83);    // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG2,  0x13);    // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG1,  0xA2);    // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG0,  0xF8);    // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_DEVIATN,  0x34);    // Modem dev (when FSK mod en)
    TI_CC_SPIWriteReg(TI_CCxxx0_MCSM2 ,   0x07);    // Until end of packet 
    TI_CC_SPIWriteReg(TI_CCxxx0_MCSM1 ,   0x30);    // reception sent over IDLE
    TI_CC_SPIWriteReg(TI_CCxxx0_MCSM0 ,   0x18);    // MainRadio Cntrl State Machine
    TI_CC_SPIWriteReg(TI_CCxxx0_FOCCFG,   0x16);    // Freq Offset Compens. Config
    TI_CC_SPIWriteReg(TI_CCxxx0_BSCFG,    0x6C);    // Bit synchronization config.
    TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL2, 0x03);    // AGC control.
    TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL1, 0x40);    // AGC control.
    TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL0, 0x91);    // AGC control.
    TI_CC_SPIWriteReg(TI_CCxxx0_WOREVT1,  0x28);
    TI_CC_SPIWriteReg(TI_CCxxx0_WOREVT0,  0xA0);    // Event0 = 300ms, RX = 585us
    TI_CC_SPIWriteReg(TI_CCxxx0_WORCTRL,  0xC8);    // Event1 = 0.444ms,RC_PD= 1,RC_CAL=1;
    TI_CC_SPIWriteReg(TI_CCxxx0_FREND1,   0x56);    // Front end RX configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_FREND0,   0x10);    // Front end RX configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL3,   0xE9);    // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL2,   0x2A);    // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL1,   0x00);    // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL0,   0x1F);    // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_RCCTRL1,  0x41);    // RC Oscillator Configuration
    TI_CC_SPIWriteReg(TI_CCxxx0_RCCTRL0,  0x00);    // RC Oscillator Configuration
    TI_CC_SPIWriteReg(TI_CCxxx0_FSTEST,   0x59);    // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_PTEST,    0x7F);    // Production Test
    TI_CC_SPIWriteReg(TI_CCxxx0_AGCTEST,  0x3F);    // AGC Test
    TI_CC_SPIWriteReg(TI_CCxxx0_TEST2,    0x81);    // Various test settings.
    TI_CC_SPIWriteReg(TI_CCxxx0_TEST1,    0x35);    // Various test settings.
    TI_CC_SPIWriteReg(TI_CCxxx0_TEST0,    0x09);    // Various test settings. 
#endif
}
//-----------------------------------------------------------------------------
void RFSendPacket(uint8_t *txBuffer, uint8_t size)
{
    uint32_t Time_num;                                              //注意：超时时间一定要长于硬件正常等待时间！
    uint32_t Time_num_max;                                          //超时时间
    
    if (txBuffer[0] < 10)                                           //根据不同包长，计算适当的超时时间
        Time_num_max = 20000;
    else
        Time_num_max = txBuffer[0] / 10 * 20000;
    
    cc1101_god0_int_cmd(0);                                         //关闭GDO0中断
    TI_CC_SPIStrobe(TI_CCxxx0_SIDLE);                               //切换到空闲模式
    TI_CC_SPIStrobe(TI_CCxxx0_SFTX);                                //清空发送缓存
#ifndef CC1101_NEW_CFG
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x04);                    //切换到固定包长模式
#else
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x44);                    //切换到固定包长模式
#endif
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTLEN, size);                      //设置固定包长模式下的数据包长度
    
    TI_CC_SPIWriteBurstReg(TI_CCxxx0_TXFIFO, txBuffer, size);       //将数据写入发送缓存
    TI_CC_SPIStrobe(TI_CCxxx0_STX);                                 //切换到发送模式发送数据
    
    Time_num = 0;
    while (CC1101_GDO0_IN != SET)                                   //等待GDO0线变高：同步字发送完成
    {
        Time_num++;
        if (Time_num > Time_num_max)
            break;
    }
    if (Time_num > Time_num_max)                                    //发送超时置位发送失败标志！
        rf_sen_fail_flag = 1;
    else
    {
        Time_num = 0;
        while (CC1101_GDO0_IN != RESET)                             //等待GDO0线变低：数据包发送完成
        {
            Time_num++;
            if (Time_num > Time_num_max)
                break;
        }
        
        rf_sen_fail_flag = 0;                                       //发送成功清零发送失败标志！
    }
    
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTLEN, 0x3D);                      //恢复可变包长模式下的最大包长设置
#ifndef CC1101_NEW_CFG
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x05);                    //切换到可变包长模式
#else
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x45);                    //切换到可变包长模式
#endif
    TI_CC_SPIStrobe(TI_CCxxx0_SIDLE);                               //切换到空闲模式
    TI_CC_SPIStrobe(TI_CCxxx0_SFRX);                                //清空接收缓存
    TI_CC_SPIStrobe(TI_CCxxx0_SRX);                                 //切换到接收模式
    cc1101_god0_int_cmd(1);                                         //开启GDO0中断
}
//-----------------------------------------------------------------------------
uint8_t RFReceivePacket(uint8_t *rxBuffer, uint8_t *length)
{
    uint8_t status[2];
    uint8_t pktLen;
    
    if ((TI_CC_SPIReadStatus(TI_CCxxx0_RXBYTES) & TI_CCxxx0_NUM_RXBYTES))
    {
        pktLen = TI_CC_SPIReadReg(TI_CCxxx0_RXFIFO);                            //读数据包长度字节
        rxBuffer[0] = pktLen;                                                   //注意：长度保存为第一个数据字节！
        
        if ((pktLen + 3) <= *length)                                            //若数据包长未超过缓存容量
        {
            TI_CC_SPIReadBurstReg(TI_CCxxx0_RXFIFO, rxBuffer + 1, pktLen);      //读取数据包剩余数据字节（注意：从第二个数组元素位置开始保存！）
            *length = pktLen;                                                   //返回数据包长度
            TI_CC_SPIReadBurstReg(TI_CCxxx0_RXFIFO, status, 2);                 //读取附加的两个状态字节
            rxBuffer[pktLen + 1] = status[0];                                   //保存信号强度
            rxBuffer[pktLen + 2] = status[1];                                   //保存CRC校验结果和链路质量值
            
            TI_CC_SPIStrobe(TI_CCxxx0_SIDLE);
//            TI_CC_SPIStrobe(TI_CCxxx0_SFRX);                                    //清空接收缓存
            TI_CC_SPIStrobe(TI_CCxxx0_SRX);
            
            return (status[TI_CCxxx0_LQI_RX] & TI_CCxxx0_CRC_OK);               //返回数据包的CRC校验状态
        }
        else                                                                    //若数据包长超过缓存容量
        {
            *length = pktLen;                                                   //返回数据包长度
            
            TI_CC_SPIStrobe(TI_CCxxx0_SIDLE);
            TI_CC_SPIStrobe(TI_CCxxx0_SFRX);                                    //清空接收缓存
            TI_CC_SPIStrobe(TI_CCxxx0_SRX);
            
            return 0;                                                           //返回数据错误状态
        }
    }
    else
    {
        TI_CC_SPIStrobe(TI_CCxxx0_SIDLE);
        TI_CC_SPIStrobe(TI_CCxxx0_SFRX);                                        //清空接收缓存（注意：此处的清空接收缓存不可少！）
        TI_CC_SPIStrobe(TI_CCxxx0_SRX);
        
        return 0;                                                               //返回数据错误状态
    }
}
//-----------------------------------------------------------------------------

/******************************************************************************
 * FunctionName : cc1101_god0_int_cmd
 * Description  : GDO0线中断控制
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void cc1101_god0_int_cmd(uint8_t state)
{
    cc1101_exti_cmd(state);
}

/******************************************************************************
 * FunctionName : cc1101_set_channel
 * Description  : 修改RF信道
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void cc1101_set_channel(uint8_t channel)
{
    cc1101_god0_int_cmd(0);
    TI_CC_SPIStrobe(TI_CCxxx0_SIDLE);
    TI_CC_SPIWriteReg(TI_CCxxx0_CHANNR, channel);
    TI_CC_SPIStrobe(TI_CCxxx0_SFTX);
    TI_CC_SPIStrobe(TI_CCxxx0_SFRX);
    TI_CC_SPIStrobe(TI_CCxxx0_SRX);
    cc1101_god0_int_cmd(1);
}

/******************************************************************************
 * FunctionName : cc1101_enter_pwd
 * Description  : RF模块进入掉电模式
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void cc1101_enter_pwd(void)
{
    cc1101_god0_int_cmd(0);
    TI_CC_SPIStrobe(TI_CCxxx0_SIDLE);
    TI_CC_SPIStrobe(TI_CCxxx0_SFTX);
    TI_CC_SPIStrobe(TI_CCxxx0_SFRX);
    TI_CC_SPIStrobe(TI_CCxxx0_SPWD);
    cc1101_god0_int_cmd(1);
}

/******************************************************************************
 * FunctionName : cc1101_rx_cal
 * Description  : RF接收模式校准
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void cc1101_rx_cal(void)
{
    cc1101_god0_int_cmd(0);             //切记：校准前一定要关中断，否则会导致在中断里读数据包状态时，CS拉低但SOMI始终不会变低的问题！！
    TI_CC_SPIStrobe(TI_CCxxx0_SIDLE);
    TI_CC_SPIStrobe(TI_CCxxx0_SFTX);
    TI_CC_SPIStrobe(TI_CCxxx0_SFRX);
    TI_CC_SPIStrobe(TI_CCxxx0_SRX);
    cc1101_god0_int_cmd(1);
}

/******************************************************************************
 * FunctionName : cc1101_init
 * Description  : CC1101功能初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void cc1101_init(uint8_t init_type)
{
    TI_CC_SPISetup();                                               //IO口和SPI总线设置
    TI_CC_PowerupResetCCxxxx();                                     //复位RF芯片
    writeRFSettings();                                              //设置RF芯片参数
    TI_CC_SPIWriteBurstReg(TI_CCxxx0_PATABLE, paTable, paTableLen); //设置RF芯片发射功率
    //------------------------------------------------
//    static uint8_t test_temp;
//    test_temp = TI_CC_SPIReadReg(TI_CCxxx0_IOCFG0);
//    
//    TI_CC_SPIReadBurstReg(TI_CCxxx0_PATABLE, paTable, paTableLen);
    //---------------------------------------------------------------
    
    TI_CC_SPIStrobe(TI_CCxxx0_SIDLE);                               //切换到空闲模式
    TI_CC_SPIStrobe(TI_CCxxx0_SFTX);                                //清空发送缓存
    TI_CC_SPIStrobe(TI_CCxxx0_SFRX);                                //清空接收缓存
    TI_CC_SPIStrobe(TI_CCxxx0_SRX);                                 //切换到接收模式
    cc1101_god0_int_cmd(1);                                         //开启GDO0中断
    
    //------------------------------------------------
//    rf_txbuf[0] = 0x09;
//    rf_txbuf[1] = 0x00;
//    rf_txbuf[2] = 0xC0;
//    rf_txbuf[3] = 0xD0;
//    rf_txbuf[4] = 0xFF;
//    rf_txbuf[5] = 0xFF;
//    rf_txbuf[6] = 0x20;
//    rf_txbuf[7] = 0x00;
//    rf_txbuf[8] = 0x99;
//    rf_txbuf[9] = 0x99;
//    RFSendPacket(rf_txbuf, rf_txbuf[0] + 1);
    //------------------------------------------------
    
    if (init_type)                                                  //0=初次上电初始化
        return ;                                                    //1=唤醒初始化
    
    rf_rx_queue = queue_init();                                     //创建无线接收数据队列头结点
    rf_tx_queue = queue_init();                                     //创建无线发送数据队列头结点
}
