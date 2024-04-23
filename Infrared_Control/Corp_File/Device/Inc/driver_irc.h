#ifndef  __DRIVER_IRC_H__
#define  __DRIVER_IRC_H__

#include "main.h"

/******************************************************************************
 *                              ����յ����Ʋ���
*******************************************************************************/
typedef enum
{
    AC_POWER_OFF = 0,                       //�ػ�
    AC_POWER_ON  = 1                        //����
} AC_POWER;

typedef enum
{
    AC_MODE_SUPPLY = 0,                     //ͨ��
    AC_MODE_COOL   = 1,                     //����
    AC_MODE_HEAT   = 2,                     //����
    AC_MODE_DEHUM  = 3,                     //��ʪ
    AC_MODE_AUTO   = 4                      //�Զ�
} AC_WORK_MODE;

typedef enum
{
    AC_SPEED_LOW  = 0,                      //С��
    AC_SPEED_MID  = 1,                      //�з�
    AC_SPEED_HIGH = 2,                      //���
    AC_SPEED_AUTO = 3                       //�Զ�
} AC_SPEED;

typedef enum
{
    AC_SWING_OFF            = 0,            //ɨ��ر�
    AC_SWING_UP_AND_DOWN    = 1,            //����ɨ��
    AC_SWING_LEFT_AND_RIGHT = 2             //����ɨ��
} AC_SWING;

/******************************************************************************
 *                              ��������п��Ʋ���
*******************************************************************************/
typedef enum
{
    TVBOX_KEY_ONOFF          = 0,           //���ػ�
    TVBOX_KEY_VOLUME_PLUS    = 20,          //������
    TVBOX_KEY_VOLUME_REDUCE  = 21,          //������
    TVBOX_KEY_VOLUME_MUTTE   = 22,          //������ȡ������
    TVBOX_KEY_CHANNEL_PLUS   = 10,          //Ƶ����
    TVBOX_KEY_CHANNEL_REDUCE = 11,          //Ƶ����
    TVBOX_KEY_NUM_0          = 40,          //����0
    TVBOX_KEY_NUM_1          = 41,          //����1
    TVBOX_KEY_NUM_2          = 42,          //����2
    TVBOX_KEY_NUM_3          = 43,          //����3
    TVBOX_KEY_NUM_4          = 44,          //����4
    TVBOX_KEY_NUM_5          = 45,          //����5
    TVBOX_KEY_NUM_6          = 46,          //����6
    TVBOX_KEY_NUM_7          = 47,          //����7
    TVBOX_KEY_NUM_8          = 48,          //����8
    TVBOX_KEY_NUM_9          = 49,          //����9
    TVBOX_KEY_UP             = 30,          //��
    TVBOX_KEY_DOWN           = 31,          //��
    TVBOX_KEY_LEFT           = 32,          //��
    TVBOX_KEY_RIGHT          = 33,          //��
    TVBOX_KEY_CONFIRM        = 34,          //ȷ��
    TVBOX_KEY_HOMEPAGE       = 54,          //��ҳ
    TVBOX_KEY_RETURN         = 51,          //����
    
} TVBOX_KEY;

/******************************************************************************
 *                              �ⲿ��������
*******************************************************************************/
extern uint16_t irc_output_timing[];
extern volatile uint16_t irc_output_num;
extern uint16_t irc_output_index;
extern volatile uint8_t irc_output_busy_flag;

/******************************************************************************
 *                              �ⲿ��������
*******************************************************************************/
extern void irc_output_level_ctrl(uint8_t level);
extern void irc_output_timing_ctrl(uint16_t time);
extern void irc_output_start(void);
extern void irc_init(void);

#endif
