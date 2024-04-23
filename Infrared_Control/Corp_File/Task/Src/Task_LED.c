#include "Task_LED.h"
#include "HAL_Control_GPIO.h"

static HAL_SM_FUNCNODE_S* ptTASK_LED_FUNCNODE = &NilNode;

void Task_LED_Handler_Uninit (void);
void Task_LED_Handler_IDLE (void);
void Task_LED_Handler_Run (void);

static HAL_FUNTIONPOINT_NRNA_P pFuncArry[] = {
    Task_LED_Handler_Uninit,
    Task_LED_Handler_IDLE,
    Task_LED_Handler_Run,
};


void Task_LED_Handler_Uninit (void)
{
    ptTASK_LED_FUNCNODE = HAL_SM_RequestFuncNode(TASK_ID_LED);
    if (ptTASK_LED_FUNCNODE != &NilNode && ptTASK_LED_FUNCNODE != NULL)
    {
        ptTASK_LED_FUNCNODE->TimeCounter = 0;
        ptTASK_LED_FUNCNODE->MinCycTime = 500;
        HAL_SM_ChangeNodeState(TASK_ID_LED,HAL_SM_RUNSTA_IDLE);
    }
}

void Task_LED_Handler_IDLE (void)
{   
    HAL_IO_SetState(HAL_IOCLASS_B,HAL_IOPIN_7,HAL_IOSTA_TOGGLE);
}

void Task_LED_Handler_Run (void)
{
    ptTASK_LED_FUNCNODE->TimeCounter = 0;
    HAL_IO_SetState(HAL_IOCLASS_B,HAL_IOPIN_7,HAL_IOSTA_TOGGLE);
}

void Task_LED_MainLoop (void)
{
	ptTASK_LED_FUNCNODE->TimeCounter = 0;
    HAL_ERROR_FuncPointJumpArguCheck(pFuncArry[ptTASK_LED_FUNCNODE->RunState]);
//    if (pFuncArry[ptTASK_LED_FUNCNODE->RunState] != NULL)
//        pFuncArry[ptTASK_LED_FUNCNODE->RunState]();
}
