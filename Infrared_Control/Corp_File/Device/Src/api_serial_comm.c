/******************************************************************************
 *                                  ͷ�ļ�
*******************************************************************************/
#include "api_serial_comm.h"
#include "Task_Serial.h"
#include  "api_rf_comm.h"
/******************************************************************************
 *                              ���崮��ͨ�ſ��Ʊ���
*******************************************************************************/
SMOTORCOMM_STRUCT serial_motor_comm;

/******************************************************************************
 * FunctionName : serial_motor_cmd_stop
 * Description  : ������ֹͣ
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
    
    /*���ݰ����*/
    if (u1_tx_queue->len >= 16)
        queue_de_discard(u1_tx_queue);
    queue_en(u1_tx_queue, u1_txbuf);
}

/******************************************************************************
 * FunctionName : serial_motor_cmd_action_p
 * Description  : ��������ת
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
    
    /*���ݰ����*/
    if (u1_tx_queue->len >= 16)
        queue_de_discard(u1_tx_queue);
    queue_en(u1_tx_queue, u1_txbuf);
}

/******************************************************************************
 * FunctionName : serial_motor_cmd_action_n
 * Description  : ��������ת
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
    
    /*���ݰ����*/
    if (u1_tx_queue->len >= 16)
        queue_de_discard(u1_tx_queue);
    queue_en(u1_tx_queue, u1_txbuf);
}

/******************************************************************************
 * FunctionName : serial_motor_config_task
 * Description  : ����ϵ���������
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void serial_motor_config_task(void)
{
    serial_motor_cmd_stop();
}

/******************************************************************************
 * FunctionName : serial_motor_data_decode
 * Description  : ���ڽ������ݽ���
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void serial_motor_data_decode(void)
{
    serial_motor_comm.data_lenght = serial_motor_comm.rx_buf[2];
    serial_motor_comm.data_cmd = serial_motor_comm.rx_buf[3];
    serial_motor_comm.data_state = serial_motor_comm.rx_buf[5];
    
    //�����
    if (serial_motor_comm.data_cmd == MOTOR_CMD_KEY)                //�����ϱ�
    {
        rf_report_initiate();
    }
}

/******************************************************************************
 * FunctionName : serial_motor_comm_task
 * Description  : �������ͨ������
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void serial_motor_comm_task(void)
{
    static uint8_t send_repeat_time_cnt = 0;    //�ط����
    static uint8_t send_repeat_times = 0;       //�ط�����
    static uint8_t wait_ack_flag = 0;           //�ȴ���Ӧ��־
    
    switch (serial_motor_comm.comm_state)       //����ͨ��״̬��
    {
        case SMOTORCOMM_RECEIVE :               //�ȴ�����״̬
        {
            if (queue_de(u1_rx_queue, serial_motor_comm.rx_buf))
            {
                serial_motor_data_decode();                         //���ݰ�����
            }
            if (queue_de(u1_tx_queue, serial_motor_comm.tx_buf))
            {
                serial_motor_comm.comm_state = SMOTORCOMM_SEND;
            }
            else break;
        }
        case SMOTORCOMM_SEND :                  //��������״̬
            {
                SERIAL_SEND_REPEAT:
                u1_send_packet(serial_motor_comm.tx_buf, serial_motor_comm.tx_buf[2] + 4);
                
                switch (serial_motor_comm.tx_buf[3])                //�ж��Ƿ���Ҫ�ȴ���Ӧ
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
        case SMOTORCOMM_WAIT_ACK :              //�ȴ���Ӧ״̬
            {
                if (queue_de(u1_rx_queue, serial_motor_comm.rx_buf))
                {
                    serial_motor_data_decode();                     //���ݰ�����
                    
                    uint8_t match_flag = 0;
                    
                    //����ȴ���Ӧ״̬�����������ϣ�
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
                
                //�ȴ���Ӧ����
                send_repeat_time_cnt++;
                if (send_repeat_time_cnt >= 100)                    //�ط����500ms
                {
                    send_repeat_time_cnt = 0;
                    if (send_repeat_times < 3)                      //�ط�����3��
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
        default :                               //�쳣״̬����
            serial_motor_comm.comm_state = SMOTORCOMM_RECEIVE;
    }
}

SMOTORCOMM_STRUCT* serial_RequestSerInfo(void)
{
    return &serial_motor_comm;
}

///******************************************************************************
// * FunctionName : serial_motor_comm_init
// * Description  : �������ͨ�Ź��ܳ�ʼ��
// * Parameters   : none
// * Returns      : none
//*******************************************************************************/
//void serial_motor_comm_init(void)
//{
//    serial_motor_comm.comm_state = SMOTORCOMM_RECEIVE;              //����ͨ��״̬��ʼ��
//    
//    sch_add_task(serial_motor_config_task, 1000, 0, LSPD);          //����ϵ���������1s��ʱ�������ȼ�
//    sch_add_task(serial_motor_comm_task, 0, 5, LSPD);               //�������ͨ������5ms���ڣ������ȼ�
//}
