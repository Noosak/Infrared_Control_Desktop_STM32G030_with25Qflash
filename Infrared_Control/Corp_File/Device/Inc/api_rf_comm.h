#ifndef  __API_RF_COMM_H__
#define  __API_RF_COMM_H__

#include "main.h"
#include "system_queue.h"
/******************************************************************************
 *                             定义主动上报重发开关
*******************************************************************************/
#define RF_REPORT_REPEAT_ON

/******************************************************************************
 *                             定义本机设备类型
*******************************************************************************/
#define RF_DEV_TYPE         0x1A    //红外人体探测和控制器

/******************************************************************************
 *                             定义本机命令类型
*******************************************************************************/
//传输层
#define RF_CMD_HANDSHAKE    0x0D    //握手命令
#define RF_CMD_ADDRESS      0x0E    //地址命令
#define RF_CMD_BOUND        0x07    //绑定命令
#define RF_CMD_UNBOUND      0x08    //解绑命令
#define RF_CMD_CHANNEL      0x09    //跳频命令
#define RF_CMD_GROUP        0x0A    //广播分组命令
#define RF_CMD_VERSION			0x0C 

//应用层
#define RF_CMD_QUERY        0x06    //查询命令
#define RF_CMD_REPORT       0x05    //上报命令
#define RF_CMD_SET          0x0B    //设置命令

/******************************************************************************
 *                              定义通信状态
*******************************************************************************/
typedef enum
{
    COMM_RECEIVE  = 0,      //等待接收
    COMM_SEND     = 1,      //发送数据
    COMM_WAIT_ACK = 2       //等待回应
} COMM_STATE;

/******************************************************************************
 *                             定义设置信道状态
*******************************************************************************/
typedef enum
{
    CH_WAIT_NEW_NUM = 0,    //等待下发新信道
    CH_WAIT_CHECK   = 1     //等待确认新信道
} CH_WAIT_STATE;

/******************************************************************************
 *                           定义RF通信控制变量类型
*******************************************************************************/
typedef struct
{
    COMM_STATE comm_state;                      //RF通信状态变量
    CH_WAIT_STATE ch_wait_state;                //RF设置信道状态变量
    uint8_t channel_num_temp;
    
    uint8_t data_lenght;                        //RF数据：长度
    uint8_t data_config;                        //RF数据：配置位
    uint32_t data_dev_addr;                     //RF数据：设备地址
    uint16_t data_host_addr;                    //RF数据：主机地址
    uint8_t data_dev_type;                      //RF数据：设备类型
    uint8_t data_cmd;                           //RF数据：命令
    uint8_t data_packet_num;                    //RF数据：包序号
    uint8_t data_group_num;                     //RF数据：广播组号
    
    uint8_t packet_num;                         //本机包序号
    uint8_t packet_num_one;                     //单一设备指令包序号
    uint8_t packet_num_group;                   //广播指令包序号
    
    uint8_t my_packet_flag;                     //本机数据包标志
    
    uint8_t host_to_dev_buf[Q_DATA_LEN_MAX];    //出队数据缓存
    uint8_t dev_to_host_buf[Q_DATA_LEN_MAX];    //出队数据缓存
} RF_COMM_STRUCT;

/******************************************************************************
 *                              外部函数声明
*******************************************************************************/
extern void rf_report_initiate(void);
extern void rf_comm_init(void);
extern RF_COMM_STRUCT* rf_comm_RequestRFInfo(void);
extern void rf_power_on_delay_report_task(void);
extern void rf_comm_task(void);
extern void rf_data_packet_num_reset_task(void);
extern void rf_address_respond_or_initiate(uint8_t cmd_type);
#endif
