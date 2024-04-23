/******************************************************************************
 *                                  头文件
*******************************************************************************/
#include "api_work_state.h"
#include "driver_key.h"
#include "api_rf_comm.h"
/******************************************************************************
 *                             定义本机工作状态变量
*******************************************************************************/
WORK_STATE_STRUCT work_state;

/******************************************************************************
 * FunctionName : work_control_task
 * Description  : 工作控制处理任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void work_control_task(void)
{
    switch (work_state.run_state)
    {
        case RUN_OFF :          //关机状态
            {
                ;
            }
            break;
        case RUN_ON :           //开机状态
            {
                ;
            }
            break;
        default :               //异常状态处理
            work_state.run_state = RUN_ON;
    }
}

/******************************************************************************
 * FunctionName : work_even_task
 * Description  : 工作事件处理任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void work_even_task(void)
{
    switch (work_state.run_state)
    {
        case RUN_OFF :          //关机状态
            {
                ;
            }
            break;
        case RUN_ON :           //开机状态
            {
                if (key_test.even_short_release)        //KEY_TEST短按事件
                {
                    rf_report_initiate();
                    
                    key_clear_evens(&key_test);
                }
                
                if (key_test.even_long_confirm)         //KEY_TEST长按事件
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
 * Description  : 工作状态初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
//void work_state_init(void)
//{
//    work_state.run_state = RUN_ON;                      //所有工作控制模式和状态初始化
//    
//    sch_add_task(work_control_task, 0, 10, LSPD);       //工作控制处理任务：10ms周期，低优先级
//    sch_add_task(work_even_task, 0, 10, LSPD);          //工作事件处理任务：10ms周期，低优先级
//}

/******************************************************************************
 * FunctionName : xxx_check
 * Description  : 参数范围检查函数
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
