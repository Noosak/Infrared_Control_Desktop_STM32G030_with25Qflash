
#include "HAL_Control_Uart.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_usart.h"
#include "Task_Serial.h"
#include "api_serial_comm.h"
#include "driver_memory.h"

#include "serialctrl_handler.h"

static HAL_SM_FUNCNODE_S* ptTask_FuncNode = &NilNode;

void Task_Serial_Handler_Uninit (void);
void Task_Serial_Handler_IDLE (void);
void Task_Serial_Handler_Run (void);

static HAL_FUNTIONPOINT_NRNA_P pFuncArry[] = {
    Task_Serial_Handler_Uninit,
    Task_Serial_Handler_IDLE,
    Task_Serial_Handler_Run,
};

void Task_Serial_Handler_Uninit (void)
{   
    ptTask_FuncNode = HAL_SM_RequestFuncNode(TASK_ID_Serial);
    if (ptTask_FuncNode != &NilNode && ptTask_FuncNode != NULL)
    {
				ptTask_FuncNode->TimeCounter = 0;
				ptTask_FuncNode->MinCycTime = 10;
        HAL_SM_ChangeNodeState(TASK_ID_Serial,HAL_SM_RUNSTA_IDLE);
    }
}

void Task_Serial_Handler_IDLE (void)
{
	serial_msg_handler(u2_receive_buffer,&u2_rx_data_len);
}

void Task_Serial_Handler_Run (void)
{
    
}

void Task_Serial_MainLoop (void)
{
		ptTask_FuncNode->TimeCounter = 0;
    HAL_ERROR_FuncPointJumpArguCheck(pFuncArry[ptTask_FuncNode->RunState]);
}
