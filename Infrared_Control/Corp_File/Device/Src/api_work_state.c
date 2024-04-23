/******************************************************************************
 *                                  ͷ�ļ�
*******************************************************************************/
#include "api_work_state.h"
#include "driver_key.h"
#include "api_rf_comm.h"
/******************************************************************************
 *                             ���屾������״̬����
*******************************************************************************/
WORK_STATE_STRUCT work_state;

/******************************************************************************
 * FunctionName : work_control_task
 * Description  : �������ƴ�������
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void work_control_task(void)
{
    switch (work_state.run_state)
    {
        case RUN_OFF :          //�ػ�״̬
            {
                ;
            }
            break;
        case RUN_ON :           //����״̬
            {
                ;
            }
            break;
        default :               //�쳣״̬����
            work_state.run_state = RUN_ON;
    }
}

/******************************************************************************
 * FunctionName : work_even_task
 * Description  : �����¼���������
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void work_even_task(void)
{
    switch (work_state.run_state)
    {
        case RUN_OFF :          //�ػ�״̬
            {
                ;
            }
            break;
        case RUN_ON :           //����״̬
            {
                if (key_test.even_short_release)        //KEY_TEST�̰��¼�
                {
                    rf_report_initiate();
                    
                    key_clear_evens(&key_test);
                }
                
                if (key_test.even_long_confirm)         //KEY_TEST�����¼�
                {
                    
                    key_clear_evens(&key_test);
                }
            }
            break;
        default :
            ;
    }
}
WORK_STATE_STRUCT* work_state_RequestInfo(void)
{
		return &work_state;
}
/******************************************************************************
 * FunctionName : work_state_init
 * Description  : ����״̬��ʼ��
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
//void work_state_init(void)
//{
//    work_state.run_state = RUN_ON;                      //���й�������ģʽ��״̬��ʼ��
//    
//    sch_add_task(work_control_task, 0, 10, LSPD);       //�������ƴ�������10ms���ڣ������ȼ�
//    sch_add_task(work_even_task, 0, 10, LSPD);          //�����¼���������10ms���ڣ������ȼ�
//}

/******************************************************************************
 * FunctionName : xxx_check
 * Description  : ������Χ��麯��
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
