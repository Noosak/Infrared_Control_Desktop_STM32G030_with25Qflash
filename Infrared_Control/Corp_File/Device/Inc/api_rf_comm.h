#ifndef  __API_RF_COMM_H__
#define  __API_RF_COMM_H__

#include "main.h"
#include "system_queue.h"
/******************************************************************************
 *                             ���������ϱ��ط�����
*******************************************************************************/
#define RF_REPORT_REPEAT_ON

/******************************************************************************
 *                             ���屾���豸����
*******************************************************************************/
#define RF_DEV_TYPE         0x1A    //��������̽��Ϳ�����

/******************************************************************************
 *                             ���屾����������
*******************************************************************************/
//�����
#define RF_CMD_HANDSHAKE    0x0D    //��������
#define RF_CMD_ADDRESS      0x0E    //��ַ����
#define RF_CMD_BOUND        0x07    //������
#define RF_CMD_UNBOUND      0x08    //�������
#define RF_CMD_CHANNEL      0x09    //��Ƶ����
#define RF_CMD_GROUP        0x0A    //�㲥��������
#define RF_CMD_VERSION			0x0C 

//Ӧ�ò�
#define RF_CMD_QUERY        0x06    //��ѯ����
#define RF_CMD_REPORT       0x05    //�ϱ�����
#define RF_CMD_SET          0x0B    //��������

/******************************************************************************
 *                              ����ͨ��״̬
*******************************************************************************/
typedef enum
{
    COMM_RECEIVE  = 0,      //�ȴ�����
    COMM_SEND     = 1,      //��������
    COMM_WAIT_ACK = 2       //�ȴ���Ӧ
} COMM_STATE;

/******************************************************************************
 *                             ���������ŵ�״̬
*******************************************************************************/
typedef enum
{
    CH_WAIT_NEW_NUM = 0,    //�ȴ��·����ŵ�
    CH_WAIT_CHECK   = 1     //�ȴ�ȷ�����ŵ�
} CH_WAIT_STATE;

/******************************************************************************
 *                           ����RFͨ�ſ��Ʊ�������
*******************************************************************************/
typedef struct
{
    COMM_STATE comm_state;                      //RFͨ��״̬����
    CH_WAIT_STATE ch_wait_state;                //RF�����ŵ�״̬����
    uint8_t channel_num_temp;
    
    uint8_t data_lenght;                        //RF���ݣ�����
    uint8_t data_config;                        //RF���ݣ�����λ
    uint32_t data_dev_addr;                     //RF���ݣ��豸��ַ
    uint16_t data_host_addr;                    //RF���ݣ�������ַ
    uint8_t data_dev_type;                      //RF���ݣ��豸����
    uint8_t data_cmd;                           //RF���ݣ�����
    uint8_t data_packet_num;                    //RF���ݣ������
    uint8_t data_group_num;                     //RF���ݣ��㲥���
    
    uint8_t packet_num;                         //���������
    uint8_t packet_num_one;                     //��һ�豸ָ������
    uint8_t packet_num_group;                   //�㲥ָ������
    
    uint8_t my_packet_flag;                     //�������ݰ���־
    
    uint8_t host_to_dev_buf[Q_DATA_LEN_MAX];    //�������ݻ���
    uint8_t dev_to_host_buf[Q_DATA_LEN_MAX];    //�������ݻ���
} RF_COMM_STRUCT;

/******************************************************************************
 *                              �ⲿ��������
*******************************************************************************/
extern void rf_report_initiate(void);
extern void rf_comm_init(void);
extern RF_COMM_STRUCT* rf_comm_RequestRFInfo(void);
extern void rf_power_on_delay_report_task(void);
extern void rf_comm_task(void);
extern void rf_data_packet_num_reset_task(void);
extern void rf_address_respond_or_initiate(uint8_t cmd_type);
#endif
