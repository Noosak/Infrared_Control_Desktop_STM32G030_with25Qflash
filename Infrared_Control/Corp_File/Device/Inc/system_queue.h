#ifndef  __SYSTEM_QUEUE_H__
#define  __SYSTEM_QUEUE_H__

//#include "stm32g0xx.h"
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//#include "system_malloc.h"

/******************************************************************************
 *                              ��������߼�
*******************************************************************************/
#define true    1
#define false   0

/******************************************************************************
 *                              ������е���������
*******************************************************************************/
typedef uint8_t datatype;

/******************************************************************************
 *                              ����������ݻ����С
*******************************************************************************/
#define Q_DATA_LEN_MAX 32
#define L_QUEUE_DEPTH 16
/******************************************************************************
 *                              ���徲̬�������ݽṹ
*******************************************************************************/
typedef struct q_node
{
    datatype data[Q_DATA_LEN_MAX];  //���ݻ���
    struct q_node *next;            //ָ����һ����ָ��
} q_node, *link_node;

typedef struct l_queue
{
    q_node *front;                  //��ͷָ��
    q_node *rear;                   //��βָ��
    int len;                        //���г���
}l_queue, *link_queue;

/******************************************************************************
 *                              �ⲿ��������
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
