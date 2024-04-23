/******************************************************************************
 *                                  ͷ�ļ�
*******************************************************************************/
#include "system_queue.h"
//#include "stdlib.h"
//#include "string.h"
/******************************************************************************
 * FunctionName : queue_init
 * Description  : ��̬���Ĵ����ͳ�ʼ��
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
link_queue queue_init(void)
{
    /* �½�ͷ��� */
    link_node new_node = (link_node)calloc(1,sizeof(q_node));
    //link_node new_node = (link_node)mymalloc(SRAMIN, sizeof(q_node));
    new_node->next = NULL;
    
    /* �½�����ͷβָ�� */
    link_queue q = (link_queue)calloc(1,sizeof(l_queue));
    //link_queue q = (link_queue)mymalloc(SRAMIN, sizeof(*q));
    q->front = q->rear = new_node;
    q->len = 0;
    
    return q;
}

/******************************************************************************
 * FunctionName : queue_empty
 * Description  : �ж϶����Ƿ�Ϊ�գ���Ϊ�շ���true�����򷵻�false
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
int queue_empty(link_queue q)
{
    return q->front == q->rear;
}

/******************************************************************************
 * FunctionName : queue_en
 * Description  : ����Ԫ��eΪ��q�Ķ�β��Ԫ�أ�����ɹ�����true����������false
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
int queue_en(link_queue q, datatype *e)
{
    /* �½����ݽ�� */
    //__disable_irq();            //������
//    if (q->len >= )
    link_node new_node = (link_node)calloc(1,sizeof(q_node));
    //link_node new_node = (link_node)mymalloc(SRAMIN, sizeof(q_node));
    //__enable_irq();             //������
    
    /* �ڴ����ʧ�� */
    if (new_node == NULL)
    {
        return false;
    }
    
    /* �ڴ����ɹ� */
    memcpy(new_node->data, e, Q_DATA_LEN_MAX);
    new_node->next = NULL;
    
    /* �½ڵ�������� */
    q->rear->next = new_node;
    
    /* ��βָ���ƶ� */
    q->rear = new_node;
    
    /* ���¶��г��� */
    q->len += 1;
    
    return true;  
}

/******************************************************************************
 * FunctionName : queue_de
 * Description  : ��ͷԪ�س��ӣ���e���س���Ԫ��,������true�����ӿշ���false
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
int queue_de(link_queue q, datatype *e)
{
    /* ����Ϊ�� */
    if (q->front == q->rear)
    {
        return false;
    }
    
    /* ���зǿ� */
    memcpy(e, q->front->next->data, Q_DATA_LEN_MAX);
    
    /* ��ͷָ���ƶ� */
    link_node temp = q->front;
    q->front = temp->next;
    
    /* �ͷų���Ԫ���ڴ� */
    //__disable_irq();            //������
    free(temp);
    //myfree(SRAMIN, temp);
    temp = NULL;
    //__enable_irq();             //������
    
    /* ���¶��г��� */
    q->len -= 1;
    
    return true;
}

/******************************************************************************
 * FunctionName : queue_de_discard
 * Description  : ��ͷԪ�س��ӣ�����Ԫ�ض���,������true�����ӿշ���false
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
int queue_de_discard(link_queue q)
{
    /* ����Ϊ�� */
    if (q->front == q->rear)
    {
        return false;
    }
    
    /* ���зǿ� */
    
    /* ��ͷָ���ƶ� */
    link_node temp = q->front;
    q->front = temp->next;
    
    /* �ͷų���Ԫ���ڴ� */
    //__disable_irq();            //������
    free(temp);
    //myfree(SRAMIN, temp);
    temp = NULL;
    //__enable_irq();             //������
    
    /* ���¶��г��� */
    q->len -= 1;
    
    return true;
}

/******************************************************************************
 * FunctionName : queue_clear
 * Description  : ��ն�
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void queue_clear(link_queue q)
{
    //__disable_irq();            //������
    
    /* �ͷŵ�һ����㼰֮�����н����ڴ� */
    link_node temp = q->front->next;
    while (temp)
    {
        link_node p = temp;
        temp = p->next;
        free(p);
        //myfree(SRAMIN, p);
        p = NULL;
    }
    
    /* ����ͷβָ��ع�ͷ��� */
    link_node head = q->front;
    head->next = NULL;
    q->front = q->rear = head;
    q->len = 0;
    
    //__enable_irq();             //������
}

/******************************************************************************
 * FunctionName : queue_destroy
 * Description  : ���ٶ�
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void queue_destroy(link_queue q)
{
    //__disable_irq();            //������
    
    /* ��ն��� */
    queue_clear(q);
    /* �ͷ�ͷ��� */
    free(q->front);
    //myfree(SRAMIN, q->front);
    /* �ͷŶ���ͷβָ�� */
    free(q);
    //myfree(SRAMIN, q);
    q = NULL;
    
    //__enable_irq();             //������
}

/******************************************************************************
 * FunctionName : queue_len
 * Description  : ��öӳ�
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
int queue_len(link_queue q)
{
    //__disable_irq();            //������
    
    /* ��ͷ����ĵ�һ����㿪ʼ�ж� */
    link_node p = q->front->next;
    
    /* ������ */
    int count = 0;
    while(p)
    {
        count += 1;
        p = p -> next;
    }
    
    //__enable_irq();             //������
    return count;
}
