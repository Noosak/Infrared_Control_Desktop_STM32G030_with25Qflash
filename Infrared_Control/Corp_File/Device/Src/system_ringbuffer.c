/******************************************************************************
 *                                  ͷ�ļ�
*******************************************************************************/
#include "system_ringbuffer.h"
#include "stdlib.h"
#include "string.h"
#include "HAL_SM_MainLoop.h"
#include "HAL_SM_TaskID.h"
/******************************************************************************
 * FunctionName : ringbuffer_init
 * Description  : ѭ����������ʼ��
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ringbuffer_init(ringbuffer_s *p_ringbuffer)
{
    p_ringbuffer->head = 0;
    p_ringbuffer->tail = 0;
    p_ringbuffer->lenght = 0;
    memset(&p_ringbuffer->node[0], 0, sizeof(ringbuffer_node_s) * RINGBUFFER_LEN);
}

/******************************************************************************
 * FunctionName : ringbuffer_write
 * Description  : ѭ��������д������
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
uint8_t ringbuffer_write(ringbuffer_s *p_ringbuffer, uint8_t *pdata, uint8_t num, uint16_t delay, uint16_t period, uint8_t times)
{
    HAL_SM_FUNCNODE_S* ptTask_FuncNode = HAL_SM_RequestFuncNode(TASK_ID_IRC);
    if (p_ringbuffer->lenght >= RINGBUFFER_LEN)
        return 0;
    
    p_ringbuffer->node[p_ringbuffer->tail].period = period;
    p_ringbuffer->node[p_ringbuffer->tail].delay = ptTask_FuncNode->TimeCounter + delay;
    p_ringbuffer->node[p_ringbuffer->tail].times = times;
    if (num > RINGBUFFER_DATA_LEN)
        num = RINGBUFFER_DATA_LEN;
    memcpy(p_ringbuffer->node[p_ringbuffer->tail].data, pdata, num);
    p_ringbuffer->tail = (p_ringbuffer->tail + 1) % RINGBUFFER_LEN;
    p_ringbuffer->lenght++;
    
    return 1;
}

/******************************************************************************
 * FunctionName : ringbuffer_read
 * Description  : ѭ����������������
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
uint8_t ringbuffer_read(ringbuffer_s *p_ringbuffer, uint8_t *pdata, uint8_t num)
{
    HAL_SM_FUNCNODE_S* ptTask_FuncNode = HAL_SM_RequestFuncNode(TASK_ID_IRC);
    if (p_ringbuffer->lenght == 0)
        return 0;
    
    if (p_ringbuffer->node[p_ringbuffer->head].times == 0)
        return 0;
    else
    {
        if (ptTask_FuncNode->TimeCounter >= p_ringbuffer->node[p_ringbuffer->head].delay)
        {
            if (num > RINGBUFFER_DATA_LEN)
                num = RINGBUFFER_DATA_LEN;
            memcpy(pdata, p_ringbuffer->node[p_ringbuffer->head].data, num);
            
            p_ringbuffer->node[p_ringbuffer->head].times--;
            if (p_ringbuffer->node[p_ringbuffer->head].times == 0)
            {
                p_ringbuffer->node[p_ringbuffer->head].period = 0;
                p_ringbuffer->node[p_ringbuffer->head].delay = 0;
                
                p_ringbuffer->head = (p_ringbuffer->head + 1) % RINGBUFFER_LEN;
                p_ringbuffer->lenght--;
            }
            else
            {
                p_ringbuffer->node[p_ringbuffer->head].delay = ptTask_FuncNode->TimeCounter + p_ringbuffer->node[p_ringbuffer->head].period;
            }
            
            return 1;
        }
        else
            return 0;
    }
}
