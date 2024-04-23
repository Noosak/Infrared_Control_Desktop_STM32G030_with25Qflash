#ifndef  __SYSTEM_RINGBUFFER_H__
#define  __SYSTEM_RINGBUFFER_H__

#include "main.h"

/******************************************************************************
 *                              定义循环缓冲变量类型
*******************************************************************************/
#define RINGBUFFER_DATA_LEN     50
#define RINGBUFFER_LEN          5

typedef struct
{
    uint16_t period;
    uint32_t delay;
    uint8_t times;
    uint8_t data[RINGBUFFER_DATA_LEN];
} ringbuffer_node_s;

typedef struct
{
    uint16_t head;
    uint16_t tail;
    uint16_t lenght;
    ringbuffer_node_s node[RINGBUFFER_LEN];
} ringbuffer_s;

/******************************************************************************
 *                              外部函数声明
*******************************************************************************/
extern void ringbuffer_init(ringbuffer_s *p_ringbuffer);
extern uint8_t ringbuffer_write(ringbuffer_s *p_ringbuffer, uint8_t *pdata, uint8_t num, uint16_t delay, uint16_t period, uint8_t times);
extern uint8_t ringbuffer_read(ringbuffer_s *p_ringbuffer, uint8_t *pdata, uint8_t num);

#endif
