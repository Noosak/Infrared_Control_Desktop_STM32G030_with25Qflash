#include "api_work_state.h"
#include "Task_Key.h"
#include "Task_CommonFile.h"
#include "driver_key.h"

static HAL_SM_FUNCNODE_S* ptTask_FuncNode = &NilNode;

void Task_Key_Handler_Uninit (void);
void Task_Key_Handler_IDLE (void);
void Task_Key_Handler_Run (void);

TASK_SUBFUNC_T* ptKeyInTaskFunc;

static HAL_FUNTIONPOINT_NRNA_P pFuncArry[] = {
    Task_Key_Handler_Uninit,
    Task_Key_Handler_IDLE,
    Task_Key_Handler_Run,
};

void Task_Key_Handler_Uninit (void)
{
    HAL_INTASK_TIME_S TempTimeFuncNode;
    WORK_STATE_STRUCT* TempInfoPoint;
    ptTask_FuncNode = HAL_SM_RequestFuncNode(TASK_ID_Key);
										
    if (ptTask_FuncNode == &NilNode || ptTask_FuncNode == NULL)
        return ;
		ptKeyInTaskFunc = Task_COM_InitSubFuncInfoStruct();
    memset(&TempTimeFuncNode, 0, sizeof(HAL_INTASK_TIME_S));
    TempInfoPoint = work_state_RequestInfo();

    TempInfoPoint->run_state = RUN_ON;           
    
    TempTimeFuncNode.RunFuncPoint = key_scan_task;
    TempTimeFuncNode.u32CycTime = 10;
    Task_COM_AddCycFunc(ptKeyInTaskFunc,&TempTimeFuncNode);
    
    TempTimeFuncNode.RunFuncPoint = work_control_task;
    TempTimeFuncNode.u32CycTime = 10;
    Task_COM_AddCycFunc(ptKeyInTaskFunc,&TempTimeFuncNode);
		
		TempTimeFuncNode.RunFuncPoint = work_even_task;
    TempTimeFuncNode.u32CycTime = 10;
    Task_COM_AddCycFunc(ptKeyInTaskFunc,&TempTimeFuncNode);

    ptTask_FuncNode->CounterInFunc = HAL_STA_ENABLE;
    HAL_SM_ChangeNodeState(TASK_ID_Key,HAL_SM_RUNSTA_IDLE);
}



void Task_Key_Handler_IDLE (void)
{   
		Task_Com_TimeCounterHandler(ptKeyInTaskFunc,&ptTask_FuncNode->TimeCounter);
}

void Task_Key_Handler_Run (void)
{
    ptTask_FuncNode->TimeCounter = 0;
}

void Task_Key_MainLoop (void)
{
    HAL_ERROR_FuncPointJumpArguCheck(pFuncArry[ptTask_FuncNode->RunState]);
}
