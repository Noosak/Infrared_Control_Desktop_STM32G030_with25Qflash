#ifndef  __API_SERIAL_MOTOR_COMM_H__
#define  __API_SERIAL_MOTOR_COMM_H__

#include "main.h"
#include "system_queue.h"
/******************************************************************************
 *                              定义串口命令类型
*******************************************************************************/
#define MOTOR_CMD_KEY           0x30            //按键
#define MOTOR_CMD_STOP          0x00            //停止
#define MOTOR_CMD_ACTION_P      0x01            //正转
#define MOTOR_CMD_ACTION_N      0x02            //反转

/******************************************************************************
 *                              定义串口通信状态
*******************************************************************************/
typedef enum
{
    SMOTORCOMM_RECEIVE  = 0,                    //等待接收
    SMOTORCOMM_SEND     = 1,                    //发送数据
    SMOTORCOMM_WAIT_ACK = 2                     //等待回应
} SMOTORCOMM_STATE;

/******************************************************************************
 *                           定义串口通信控制变量类型
*******************************************************************************/
typedef struct
{
    SMOTORCOMM_STATE comm_state;                //串口通信状态变量
    
    uint8_t data_lenght;                        //串口数据：长度
    uint8_t data_cmd;                           //串口数据：命令
    uint8_t data_state;                         //串口数据：状态
    
    uint8_t rx_buf[Q_DATA_LEN_MAX];             //RX出队数据缓存
    uint8_t tx_buf[Q_DATA_LEN_MAX];             //TX出队数据缓存
} SMOTORCOMM_STRUCT;

/******************************************************************************
 *                              外部变量声明
*******************************************************************************/
extern SMOTORCOMM_STRUCT serial_motor_comm;

/******************************************************************************
 *                              外部函数声明
*******************************************************************************/
extern void serial_motor_cmd_stop(void);
extern void serial_motor_cmd_action_p(void);
extern void serial_motor_cmd_action_n(void);
extern void serial_motor_comm_init(void);
extern SMOTORCOMM_STRUCT* serial_RequestSerInfo(void);
extern void serial_motor_config_task(void);
extern void serial_motor_comm_task(void);
#endif
