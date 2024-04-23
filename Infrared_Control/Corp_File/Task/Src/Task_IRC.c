#include "Task_CommonFile.h"
#include "driver_irc.h"

#include "driver_irc_tv_chengmai_can.h"

#define MAX_DEPEND_TIME_FUNC 10
static HAL_SM_FUNCNODE_S* ptTask_FuncNode = &NilNode;

void Task_IRC_Handler_Uninit (void);
void Task_IRC_Handler_IDLE (void);
void Task_IRC_Handler_Run (void);

void Task_IRC_TimeCounterHandler(uint32_t* u32TimeCounterBase);

static HAL_FUNTIONPOINT_NRNA_P pFuncArry[] = {
    Task_IRC_Handler_Uninit,
    Task_IRC_Handler_IDLE,
    Task_IRC_Handler_Run,
};

void Task_IRC_Handler_Uninit (void)
{
    ptTask_FuncNode = HAL_SM_RequestFuncNode(TASK_ID_IRC);
    if (ptTask_FuncNode == &NilNode || ptTask_FuncNode == NULL)
        return ;
    irc_init();
    
    irc_chengmai_can_tv_init();
    
    
    ptTask_FuncNode->CounterInFunc = HAL_STA_ENABLE;
    HAL_SM_ChangeNodeState(TASK_ID_IRC,HAL_SM_RUNSTA_IDLE);
}



void Task_IRC_Handler_IDLE (void)
{   
    irc_chengmai_can_tv_send_task();
    //ptTask_FuncNode->TimeCounter = 0;
    //HAL_IO_SetState(HAL_IOCLASS_A,HAL_IOPIN_12,HAL_IOSTA_TOGGLE);
}

void Task_IRC_Handler_Run (void)
{
    ptTask_FuncNode->TimeCounter = 0;
}

void Task_IRC_MainLoop (void)
{
    HAL_ERROR_FuncPointJumpArguCheck(pFuncArry[ptTask_FuncNode->RunState]);
//    if (pFuncArry[ptTask_FuncNode->RunState] != NULL)
//        pFuncArry[ptTask_FuncNode->RunState]();
}
