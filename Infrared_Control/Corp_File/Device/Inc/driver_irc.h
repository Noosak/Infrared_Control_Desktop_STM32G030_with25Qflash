#ifndef  __DRIVER_IRC_H__
#define  __DRIVER_IRC_H__

#include "main.h"

/******************************************************************************
 *                              定义空调控制参数
*******************************************************************************/
typedef enum
{
    AC_POWER_OFF = 0,                       //关机
    AC_POWER_ON  = 1                        //开机
} AC_POWER;

typedef enum
{
    AC_MODE_SUPPLY = 0,                     //通风
    AC_MODE_COOL   = 1,                     //制冷
    AC_MODE_HEAT   = 2,                     //制热
    AC_MODE_DEHUM  = 3,                     //除湿
    AC_MODE_AUTO   = 4                      //自动
} AC_WORK_MODE;

typedef enum
{
    AC_SPEED_LOW  = 0,                      //小风
    AC_SPEED_MID  = 1,                      //中风
    AC_SPEED_HIGH = 2,                      //大风
    AC_SPEED_AUTO = 3                       //自动
} AC_SPEED;

typedef enum
{
    AC_SWING_OFF            = 0,            //扫风关闭
    AC_SWING_UP_AND_DOWN    = 1,            //上下扫风
    AC_SWING_LEFT_AND_RIGHT = 2             //左右扫风
} AC_SWING;

/******************************************************************************
 *                              定义机顶盒控制参数
*******************************************************************************/
typedef enum
{
    TVBOX_KEY_ONOFF          = 0,           //开关机
    TVBOX_KEY_VOLUME_PLUS    = 20,          //音量加
    TVBOX_KEY_VOLUME_REDUCE  = 21,          //音量减
    TVBOX_KEY_VOLUME_MUTTE   = 22,          //静音和取消静音
    TVBOX_KEY_CHANNEL_PLUS   = 10,          //频道加
    TVBOX_KEY_CHANNEL_REDUCE = 11,          //频道减
    TVBOX_KEY_NUM_0          = 40,          //数字0
    TVBOX_KEY_NUM_1          = 41,          //数字1
    TVBOX_KEY_NUM_2          = 42,          //数字2
    TVBOX_KEY_NUM_3          = 43,          //数字3
    TVBOX_KEY_NUM_4          = 44,          //数字4
    TVBOX_KEY_NUM_5          = 45,          //数字5
    TVBOX_KEY_NUM_6          = 46,          //数字6
    TVBOX_KEY_NUM_7          = 47,          //数字7
    TVBOX_KEY_NUM_8          = 48,          //数字8
    TVBOX_KEY_NUM_9          = 49,          //数字9
    TVBOX_KEY_UP             = 30,          //上
    TVBOX_KEY_DOWN           = 31,          //下
    TVBOX_KEY_LEFT           = 32,          //左
    TVBOX_KEY_RIGHT          = 33,          //右
    TVBOX_KEY_CONFIRM        = 34,          //确认
    TVBOX_KEY_HOMEPAGE       = 54,          //主页
    TVBOX_KEY_RETURN         = 51,          //返回
    
} TVBOX_KEY;

/******************************************************************************
 *                              外部变量声明
*******************************************************************************/
extern uint16_t irc_output_timing[];
extern volatile uint16_t irc_output_num;
extern uint16_t irc_output_index;
extern volatile uint8_t irc_output_busy_flag;

/******************************************************************************
 *                              外部函数声明
*******************************************************************************/
extern void irc_output_level_ctrl(uint8_t level);
extern void irc_output_timing_ctrl(uint16_t time);
extern void irc_output_start(void);
extern void irc_init(void);

#endif
