#ifndef  __DRIVER_IRC_TV_CHENGMAI_CAN_H__
#define  __DRIVER_IRC_TV_CHENGMAI_CAN_H__

#include "main.h"

/******************************************************************************
 *                             定义红外遥控控制时序
*******************************************************************************/


/******************************************************************************
 *                             定义红外遥控控制参数
*******************************************************************************/
/*typedef enum {
	TV_CMD_ONOFF,
	TV_CMD_VOLUME_PLUS,
	TV_CMD_VOLUME_REDUCE,
	TV_CMD_MUTTE,
	TV_CMD_UP,
	TV_CMD_DOWN,
}CONTROL_CODE_E;
*/
/******************************************************************************
 *                            定义红外遥控命令码长度
*******************************************************************************/
#define IRC_CHENGMAI_CAN_TV_CODE_LEN              1
#define IRC_CHENGMAI_CAN_TV_TIMING_LEN            78

/******************************************************************************
 *                           定义红外遥控控制变量类型
*******************************************************************************/
typedef struct
{
    uint8_t ir_code[IRC_CHENGMAI_CAN_TV_CODE_LEN];
    uint16_t ir_timing[IRC_CHENGMAI_CAN_TV_TIMING_LEN];
    uint16_t ir_num;
    
} IRC_CHENGMAI_CAN_TV_STRUCT;

typedef enum {
	TV_CMD_ONOFF,
	TV_CMD_VOLUME_PLUS,
	TV_CMD_VOLUME_REDUCE,
	TV_CMD_MUTTE,
	TV_CMD_UP,
	TV_CMD_DOWN,
}CONTROL_CODE_E;

/******************************************************************************
 *                              外部函数声明
*******************************************************************************/
extern void irc_chengmai_can_tv_make_and_send_code(uint8_t key);
extern void irc_chengmai_can_tv_send_task(void);
extern void irc_chengmai_can_tv_init(void);

#endif
