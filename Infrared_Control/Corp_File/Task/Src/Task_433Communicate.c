#include "Task_433Communicate.h"
#include "HAL_Control_GPIO.h"
#include "api_rf_comm.h"
#include "driver_memory.h"
#include "radio.h"
#define MAX_DEPEND_TIME_FUNC 10
static HAL_SM_FUNCNODE_S* ptTask_FuncNode = &NilNode;
static RF_COMM_STRUCT *RF_ControlInfo; 

TASK_SUBFUNC_T* pt433CommunicateInTaskFunc;

void Task_RF_Handler_Uninit (void);
void Task_RF_Handler_IDLE (void);
void Task_RF_Handler_Run (void);

static HAL_FUNTIONPOINT_NRNA_P pFuncArry[] = {
    Task_RF_Handler_Uninit,
    Task_RF_Handler_IDLE,
    Task_RF_Handler_Run,
};

void Task_RF_Handler_Uninit (void)
{
    HAL_INTASK_TIME_S TempTimeFuncNode;
    memset (&TempTimeFuncNode, 0, sizeof(HAL_INTASK_TIME_S));
    ptTask_FuncNode = HAL_SM_RequestFuncNode(TASK_ID_RF);
    pt433CommunicateInTaskFunc = Task_COM_InitSubFuncInfoStruct();
    //cc1101_init(0);
    CMT2300A_Device_Init();
    if (ptTask_FuncNode != &NilNode && ptTask_FuncNode != NULL)
    {
        RF_ControlInfo = rf_comm_RequestRFInfo();

        CMT2300A_SetFrequencyChannel(my_channel_num);
        RF_ControlInfo->comm_state = COMM_RECEIVE;                  //RF通信状态初始化
        
        RF_ControlInfo->packet_num = 0xFF;
        RF_ControlInfo->packet_num_one = 0xFF;                      //包序号初始化，以接收第一个数据包
        RF_ControlInfo->packet_num_group = 0xFF;
        
				if (my_dev_addr != 0xFFFFFF)                        
				{
							TempTimeFuncNode.RunFuncPoint = rf_power_on_delay_report_task;       //RF通信任务：5ms周期，低优先级
							TempTimeFuncNode.u32DelayTime = 1000;
							Task_COM_AddCycFunc(pt433CommunicateInTaskFunc,&TempTimeFuncNode);
				}
				else                                                
					rf_address_respond_or_initiate(1);
			
        TempTimeFuncNode.RunFuncPoint = rf_comm_task;       //RF通信任务：5ms周期，低优先级
        TempTimeFuncNode.u32CycTime = 1;
				TempTimeFuncNode.u32DelayTime = 0;
        Task_COM_AddCycFunc(pt433CommunicateInTaskFunc,&TempTimeFuncNode);
        
        TempTimeFuncNode.RunFuncPoint = rf_data_packet_num_reset_task;  //RF接收包序号复位任务：30s周期，低优先级
        TempTimeFuncNode.u32CycTime = 30000;
        Task_COM_AddCycFunc(pt433CommunicateInTaskFunc,&TempTimeFuncNode);
        // (&(InTaskTimeHandler_IDLE[2]))->RunFuncPoint = rf_report_initiate;
        // (&(InTaskTimeHandler_IDLE[2]))->u32CycTime = 5000;
//        TempTimeFuncNode.RunFuncPoint = rf_report_initiate;
//        TempTimeFuncNode.u32CycTime = 5000;
//        Task_CC1101_IDLE_AddCycFunc(&TempTimeFuncNode);

        // ptTask_FuncNode->TimeCounter = 0;
        // ptTask_FuncNode->MinCycTime = 0;
        ptTask_FuncNode->CounterInFunc = HAL_STA_ENABLE;
        HAL_SM_ChangeNodeState(TASK_ID_RF,HAL_SM_RUNSTA_IDLE);
    }
}

void Task_RF_Handler_IDLE (void)
{   
    Task_Com_TimeCounterHandler(pt433CommunicateInTaskFunc,&ptTask_FuncNode->TimeCounter);
}

void Task_RF_Handler_Run (void)
{
    ptTask_FuncNode->TimeCounter = 0;
    HAL_IO_SetState(HAL_IOCLASS_A,HAL_IOPIN_0,HAL_IOSTA_TOGGLE);
}

void Task_RF_MainLoop (void)
{
    HAL_ERROR_FuncPointJumpArguCheck(pFuncArry[ptTask_FuncNode->RunState]);
}
