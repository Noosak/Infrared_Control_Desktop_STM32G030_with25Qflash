#ifndef  __SYSTEM_QUEUE_H__
#define  __SYSTEM_QUEUE_H__

//#include "stm32g0xx.h"
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//#include "system_malloc.h"

/******************************************************************************
 *                              定义真假逻辑
*******************************************************************************/
#define true    1
#define false   0

/******************************************************************************
 *                              定义队列的数据类型
*******************************************************************************/
typedef uint8_t datatype;

/******************************************************************************
 *                              定义队列数据缓存大小
*******************************************************************************/
#define Q_DATA_LEN_MAX 32
#define L_QUEUE_DEPTH 16
/******************************************************************************
 *                              定义静态链的数据结构
*******************************************************************************/
typedef struct q_node
{
    datatype data[Q_DATA_LEN_MAX];  //数据缓存
    struct q_node *next;            //指向下一结点的指针
} q_node, *link_node;

typedef struct l_queue
{
    q_node *front;                  //队头指针
    q_node *rear;                   //队尾指针
    int len;                        //队列长度
}l_queue, *link_queue;

/******************************************************************************
 *                              外部函数声明
*******************************************************************************/
extern link_queue queue_init(void);
extern int queue_empty(link_queue q);
extern int queue_en(link_queue q, datatype *e);
extern int queue_de(link_queue q, datatype *e);
extern int queue_de_discard(link_queue q);
extern void queue_clear(link_queue q);
extern void queue_destroy(link_queue q);
extern int queue_len(link_queue q);

#endif
