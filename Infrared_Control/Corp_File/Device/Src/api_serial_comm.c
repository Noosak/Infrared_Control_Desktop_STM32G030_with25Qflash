/******************************************************************************
 *                                  头文件
*******************************************************************************/
#include "api_serial_comm.h"
#include "Task_Serial.h"
#include  "api_rf_comm.h"
/******************************************************************************
 *                              定义串口通信控制变量
*******************************************************************************/
SMOTORCOMM_STRUCT serial_motor_comm;

/******************************************************************************
 * FunctionName : serial_motor_cmd_stop
 * Description  : 电机命令：停止
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void serial_motor_cmd_stop(void)
{
    u1_txbuf[0] = 0xAA;
    u1_txbuf[1] = 0xAA;
    
    u1_txbuf[2] = 0x03;
    u1_txbuf[3] = 0x12;
    u1_txbuf[4] = 0x00;
    u1_txbuf[5] = 0x00;
    u1_txbuf[6] = 0x15;
    
    /*数据包入队*/
    if (u1_tx_queue->len >= 16)
        queue_de_discard(u1_tx_queue);
    queue_en(u1_tx_queue, u1_txbuf);
}

/******************************************************************************
 * FunctionName : serial_motor_cmd_action_p
 * Description  : 电机命令：正转
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void serial_motor_cmd_action_p(void)
{
    u1_txbuf[0] = 0xAA;
    u1_txbuf[1] = 0xAA;
    
    u1_txbuf[2] = 0x03;
    u1_txbuf[3] = 0x11;
    u1_txbuf[4] = 0x00;
    u1_txbuf[5] = 0x00;
    u1_txbuf[6] = 0x14;
    
    /*数据包入队*/
    if (u1_tx_queue->len >= 16)
        queue_de_discard(u1_tx_queue);
    queue_en(u1_tx_queue, u1_txbuf);
}

/******************************************************************************
 * FunctionName : serial_motor_cmd_action_n
 * Description  : 电机命令：反转
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void serial_motor_cmd_action_n(void)
{
    u1_txbuf[0] = 0xAA;
    u1_txbuf[1] = 0xAA;
    
    u1_txbuf[2] = 0x03;
    u1_txbuf[3] = 0x13;
    u1_txbuf[4] = 0x00;
    u1_txbuf[5] = 0x00;
    u1_txbuf[6] = 0x16;
    
    /*数据包入队*/
    if (u1_tx_queue->len >= 16)
        queue_de_discard(u1_tx_queue);
    queue_en(u1_tx_queue, u1_txbuf);
}

/******************************************************************************
 * FunctionName : serial_motor_config_task
 * Description  : 电机上电配置任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void serial_motor_config_task(void)
{
    serial_motor_cmd_stop();
}

/******************************************************************************
 * FunctionName : serial_motor_data_decode
 * Description  : 串口接收数据解析
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void serial_motor_data_decode(void)
{
    serial_motor_comm.data_lenght = serial_motor_comm.rx_buf[2];
    serial_motor_comm.data_cmd = serial_motor_comm.rx_buf[3];
    serial_motor_comm.data_state = serial_motor_comm.rx_buf[5];
    
    //命令处理
    if (serial_motor_comm.data_cmd == MOTOR_CMD_KEY)                //按键上报
    {
        rf_report_initiate();
    }
}

/******************************************************************************
 * FunctionName : serial_motor_comm_task
 * Description  : 电机串口通信任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void serial_motor_comm_task(void)
{
    static uint8_t send_repeat_time_cnt = 0;    //重发间隔
    static uint8_t send_repeat_times = 0;       //重发次数
    static uint8_t wait_ack_flag = 0;           //等待回应标志
    
    switch (serial_motor_comm.comm_state)       //串口通信状态机
    {
        case SMOTORCOMM_RECEIVE :               //等待接收状态
        {
            if (queue_de(u1_rx_queue, serial_motor_comm.rx_buf))
            {
                serial_motor_data_decode();                         //数据包解析
            }
            if (queue_de(u1_tx_queue, serial_motor_comm.tx_buf))
            {
                serial_motor_comm.comm_state = SMOTORCOMM_SEND;
            }
            else break;
        }
        case SMOTORCOMM_SEND :                  //发送数据状态
            {
                SERIAL_SEND_REPEAT:
                u1_send_packet(serial_motor_comm.tx_buf, serial_motor_comm.tx_buf[2] + 4);
                
                switch (serial_motor_comm.tx_buf[3])                //判断是否需要等待回应
                {
                    case MOTOR_CMD_STOP :
                        wait_ack_flag = 0;
                        break;
                    case MOTOR_CMD_ACTION_P :
                    case MOTOR_CMD_ACTION_N :
                        wait_ack_flag = 1;
                        break;
                    default :
                        ;
                }
                if (wait_ack_flag)
                    serial_motor_comm.comm_state = SMOTORCOMM_WAIT_ACK;
                else
                    serial_motor_comm.comm_state = SMOTORCOMM_RECEIVE;
            }
            break;
        case SMOTORCOMM_WAIT_ACK :              //等待回应状态
            {
                if (queue_de(u1_rx_queue, serial_motor_comm.rx_buf))
                {
                    serial_motor_data_decode();                     //数据包解析
                    
                    uint8_t match_flag = 0;
                    
                    //避免等待回应状态被其他命令打断！
                    switch (serial_motor_comm.tx_buf[3])
                    {
                        case MOTOR_CMD_STOP :
                        case MOTOR_CMD_ACTION_P :
                        case MOTOR_CMD_ACTION_N :
                            {
                                if (serial_motor_comm.data_cmd == serial_motor_comm.tx_buf[3])
                                    match_flag = 1;
                            }
                            break;
                        default :
                            ;
                    }
                    
                    if (match_flag == 1)
                    {
                        send_repeat_times = 0;
                        send_repeat_time_cnt = 0;
                        serial_motor_comm.comm_state = SMOTORCOMM_RECEIVE;
                    }
                }
                
                //等待回应处理
                send_repeat_time_cnt++;
                if (send_repeat_time_cnt >= 100)                    //重发间隔500ms
                {
                    send_repeat_time_cnt = 0;
                    if (send_repeat_times < 3)                      //重发次数3次
                    {
                        serial_motor_comm.comm_state = SMOTORCOMM_SEND;
                        send_repeat_times++;
                        goto SERIAL_SEND_REPEAT;
                    }
                    else
                    {
                        serial_motor_comm.comm_state = SMOTORCOMM_RECEIVE;
                        send_repeat_times = 0;
                    }
                }
            }
            break;
        default :                               //异常状态处理
            serial_motor_comm.comm_state = SMOTORCOMM_RECEIVE;
    }
}

SMOTORCOMM_STRUCT* serial_RequestSerInfo(void)
{
    return &serial_motor_comm;
}

///******************************************************************************
// * FunctionName : serial_motor_comm_init
// * Description  : 电机串口通信功能初始化
// * Parameters   : none
// * Returns      : none
//*******************************************************************************/
//void serial_motor_comm_init(void)
//{
//    serial_motor_comm.comm_state = SMOTORCOMM_RECEIVE;              //串口通信状态初始化
//    
//    sch_add_task(serial_motor_config_task, 1000, 0, LSPD);          //电机上电配置任务：1s延时，低优先级
//    sch_add_task(serial_motor_comm_task, 0, 5, LSPD);               //电机串口通信任务：5ms周期，低优先级
//}
