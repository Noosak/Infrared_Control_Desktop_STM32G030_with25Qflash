#include "Task_IREmit.h"
#include "Task_CommonFile.h"

static HAL_SM_FUNCNODE_S* ptTask_FuncNode = &NilNode;

void Task_IREmit_Handler_Uninit (void);
void Task_IREmit_Handler_IDLE (void);
void Task_IREmit_Handler_Run (void);

TASK_SUBFUNC_T* ptIREmitInTaskFunc;

static HAL_FUNTIONPOINT_NRNA_P pFuncArry[] = {
    Task_IREmit_Handler_Uninit,
    Task_IREmit_Handler_IDLE,
    Task_IREmit_Handler_Run,
};

void Task_IREmit_Handler_Uninit (void)
{
    HAL_INTASK_TIME_S TempTimeFuncNode;
    ptTask_FuncNode = HAL_SM_RequestFuncNode(TASK_ID_IREmit);
										
    if (ptTask_FuncNode == &NilNode || ptTask_FuncNode == NULL)
        return ;
		ptIREmitInTaskFunc = Task_COM_InitSubFuncInfoStruct();
    memset(&TempTimeFuncNode, 0, sizeof(HAL_INTASK_TIME_S));
		
//		TempTimeFuncNode.RunFuncPoint = work_even_task;
//    TempTimeFuncNode.u32CycTime = 10;
//    Task_COM_AddCycFunc(ptIREmitInTaskFunc,&TempTimeFuncNode);

    ptTask_FuncNode->CounterInFunc = HAL_STA_ENABLE;
    HAL_SM_ChangeNodeState(TASK_ID_IREmit,HAL_SM_RUNSTA_IDLE);
}

void Add_SubIREmitTask(HAL_INTASK_TIME_S* ptInTask)
{
	Task_COM_AddCycFunc(ptIREmitInTaskFunc,ptInTask);
}

void Del_SubIREmitTask(HAL_FUNTIONPOINT_NRNA_P hook)
{
	Task_Com_DelCycFunc(ptIREmitInTaskFunc,hook);
}

void Task_IREmit_Handler_IDLE (void)
{   
		Task_Com_TimeCounterHandler(ptIREmitInTaskFunc,&ptTask_FuncNode->TimeCounter);
}

void Task_IREmit_Handler_Run (void)
{
    ptTask_FuncNode->TimeCounter = 0;
}

void Task_IREmit_MainLoop (void)
{
    HAL_ERROR_FuncPointJumpArguCheck(pFuncArry[ptTask_FuncNode->RunState]);
}
