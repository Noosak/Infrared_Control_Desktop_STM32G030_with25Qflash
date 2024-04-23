#ifndef  __API_SERIAL_MOTOR_COMM_H__
#define  __API_SERIAL_MOTOR_COMM_H__

#include "main.h"
#include "system_queue.h"
/******************************************************************************
 *                              ���崮����������
*******************************************************************************/
#define MOTOR_CMD_KEY           0x30            //����
#define MOTOR_CMD_STOP          0x00            //ֹͣ
#define MOTOR_CMD_ACTION_P      0x01            //��ת
#define MOTOR_CMD_ACTION_N      0x02            //��ת

/******************************************************************************
 *                              ���崮��ͨ��״̬
*******************************************************************************/
typedef enum
{
    SMOTORCOMM_RECEIVE  = 0,                    //�ȴ�����
    SMOTORCOMM_SEND     = 1,                    //��������
    SMOTORCOMM_WAIT_ACK = 2                     //�ȴ���Ӧ
} SMOTORCOMM_STATE;

/******************************************************************************
 *                           ���崮��ͨ�ſ��Ʊ�������
*******************************************************************************/
typedef struct
{
    SMOTORCOMM_STATE comm_state;                //����ͨ��״̬����
    
    uint8_t data_lenght;                        //�������ݣ�����
    uint8_t data_cmd;                           //�������ݣ�����
    uint8_t data_state;                         //�������ݣ�״̬
    
    uint8_t rx_buf[Q_DATA_LEN_MAX];             //RX�������ݻ���
    uint8_t tx_buf[Q_DATA_LEN_MAX];             //TX�������ݻ���
} SMOTORCOMM_STRUCT;

/******************************************************************************
 *                              �ⲿ��������
*******************************************************************************/
extern SMOTORCOMM_STRUCT serial_motor_comm;

/******************************************************************************
 *                              �ⲿ��������
*******************************************************************************/
extern void serial_motor_cmd_stop(void);
extern void serial_motor_cmd_action_p(void);
extern void serial_motor_cmd_action_n(void);
extern void serial_motor_comm_init(void);
extern SMOTORCOMM_STRUCT* serial_RequestSerInfo(void);
extern void serial_motor_config_task(void);
extern void serial_motor_comm_task(void);
#endif
