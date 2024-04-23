/******************************************************************************
 *                                  头文件
*******************************************************************************/
#include "system_queue.h"
//#include "stdlib.h"
//#include "string.h"
/******************************************************************************
 * FunctionName : queue_init
 * Description  : 静态链的创建和初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
link_queue queue_init(void)
{
    /* 新建头结点 */
    link_node new_node = (link_node)calloc(1,sizeof(q_node));
    //link_node new_node = (link_node)mymalloc(SRAMIN, sizeof(q_node));
    new_node->next = NULL;
    
    /* 新建队列头尾指针 */
    link_queue q = (link_queue)calloc(1,sizeof(l_queue));
    //link_queue q = (link_queue)mymalloc(SRAMIN, sizeof(*q));
    q->front = q->rear = new_node;
    q->len = 0;
    
    return q;
}

/******************************************************************************
 * FunctionName : queue_empty
 * Description  : 判断队列是否为空，若为空返回true，否则返回false
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
int queue_empty(link_queue q)
{
    return q->front == q->rear;
}

/******************************************************************************
 * FunctionName : queue_en
 * Description  : 插入元素e为队q的队尾新元素，插入成功返回true，队满返回false
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
int queue_en(link_queue q, datatype *e)
{
    /* 新建数据结点 */
    //__disable_irq();            //！！！
//    if (q->len >= )
    link_node new_node = (link_node)calloc(1,sizeof(q_node));
    //link_node new_node = (link_node)mymalloc(SRAMIN, sizeof(q_node));
    //__enable_irq();             //！！！
    
    /* 内存分配失败 */
    if (new_node == NULL)
    {
        return false;
    }
    
    /* 内存分配成功 */
    memcpy(new_node->data, e, Q_DATA_LEN_MAX);
    new_node->next = NULL;
    
    /* 新节点接入链表 */
    q->rear->next = new_node;
    
    /* 队尾指针移动 */
    q->rear = new_node;
    
    /* 更新队列长度 */
    q->len += 1;
    
    return true;  
}

/******************************************************************************
 * FunctionName : queue_de
 * Description  : 队头元素出队，用e返回出队元素,并返回true，若队空返回false
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
int queue_de(link_queue q, datatype *e)
{
    /* 队列为空 */
    if (q->front == q->rear)
    {
        return false;
    }
    
    /* 队列非空 */
    memcpy(e, q->front->next->data, Q_DATA_LEN_MAX);
    
    /* 队头指针移动 */
    link_node temp = q->front;
    q->front = temp->next;
    
    /* 释放出队元素内存 */
    //__disable_irq();            //！！！
    free(temp);
    //myfree(SRAMIN, temp);
    temp = NULL;
    //__enable_irq();             //！！！
    
    /* 更新队列长度 */
    q->len -= 1;
    
    return true;
}

/******************************************************************************
 * FunctionName : queue_de_discard
 * Description  : 队头元素出队，出队元素丢弃,并返回true，若队空返回false
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
int queue_de_discard(link_queue q)
{
    /* 队列为空 */
    if (q->front == q->rear)
    {
        return false;
    }
    
    /* 队列非空 */
    
    /* 队头指针移动 */
    link_node temp = q->front;
    q->front = temp->next;
    
    /* 释放出队元素内存 */
    //__disable_irq();            //！！！
    free(temp);
    //myfree(SRAMIN, temp);
    temp = NULL;
    //__enable_irq();             //！！！
    
    /* 更新队列长度 */
    q->len -= 1;
    
    return true;
}

/******************************************************************************
 * FunctionName : queue_clear
 * Description  : 清空队
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void queue_clear(link_queue q)
{
    //__disable_irq();            //！！！
    
    /* 释放第一个结点及之后所有结点的内存 */
    link_node temp = q->front->next;
    while (temp)
    {
        link_node p = temp;
        temp = p->next;
        free(p);
        //myfree(SRAMIN, p);
        p = NULL;
    }
    
    /* 队列头尾指针回归头结点 */
    link_node head = q->front;
    head->next = NULL;
    q->front = q->rear = head;
    q->len = 0;
    
    //__enable_irq();             //！！！
}

/******************************************************************************
 * FunctionName : queue_destroy
 * Description  : 销毁队
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void queue_destroy(link_queue q)
{
    //__disable_irq();            //！！！
    
    /* 清空队列 */
    queue_clear(q);
    /* 释放头结点 */
    free(q->front);
    //myfree(SRAMIN, q->front);
    /* 释放队列头尾指针 */
    free(q);
    //myfree(SRAMIN, q);
    q = NULL;
    
    //__enable_irq();             //！！！
}

/******************************************************************************
 * FunctionName : queue_len
 * Description  : 获得队长
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
int queue_len(link_queue q)
{
    //__disable_irq();            //！！！
    
    /* 从头结点后的第一个结点开始判断 */
    link_node p = q->front->next;
    
    /* 计数器 */
    int count = 0;
    while(p)
    {
        count += 1;
        p = p -> next;
    }
    
    //__enable_irq();             //！！！
    return count;
}
