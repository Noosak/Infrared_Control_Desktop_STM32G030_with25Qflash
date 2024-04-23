#pragma once
#ifndef __TASK_COMMONFILE_H_
#define __TASK_COMMONFILE_H_ 

#include "HAL_SM_MainLoop.h"
#include "HAL_SM_TaskID.h"

#define MAX_DEPEND_TIME_FUNC 10

static HAL_SM_FUNCNODE_S NilNode = {
    .Priority = HAL_SM_PRIORITY_NORMAL,
    .RunState = HAL_SM_RUNSTA_UNINIT,
    .FuncID = 0,
    .PrequestFuncID = NULL,
    .TaskFuncPointer = NULL,
    .MinCycTime = 0,
    .TimeCounter = 0,
    .PrequestFuncState = HAL_SM_RUNSTA_UNINIT, 
    .CounterInFunc = HAL_STA_DISABLE,
};

#pragma pack(4) 
typedef struct 
{
    uint32_t u32DelayTime;
    uint32_t u32CycTime;
    uint32_t u32TimeCounter;
    HAL_FUNTIONPOINT_NRNA_P RunFuncPoint;
}HAL_INTASK_TIME_S;

typedef struct {
    HAL_INTASK_TIME_S TaskProcArry[MAX_DEPEND_TIME_FUNC];
    uint8_t ProcArryLen;
}TASK_SUBFUNC_T;

#pragma pack()

extern TASK_SUBFUNC_T* Task_COM_InitSubFuncInfoStruct (void);
extern uint32_t Task_COM_AddCycFunc (TASK_SUBFUNC_T* ptSubFuncInfo,HAL_INTASK_TIME_S *InputCucFuncInfo);
extern uint32_t Task_Com_DelCycFunc (TASK_SUBFUNC_T* ptSubFuncInfo,HAL_FUNTIONPOINT_NRNA_P fpDelFuncPoint);
extern void Task_Com_TimeCounterHandler(TASK_SUBFUNC_T* ptSubFuncInfo,uint32_t* u32TimeCounterBase);

#endif
