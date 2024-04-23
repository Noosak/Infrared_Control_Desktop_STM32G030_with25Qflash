#pragma once
#ifndef __HAL_SM_MAINLOOP_H__
#define __HAL_SM_MAINLOOP_H__ 

#include "HAL_GlobalConfig.h"
#include "HAL_Control_Error.h"

#define HAL_SM_MAXTASKQUEUE 10
#define HAL_SM_REALMAXQUENUM (HAL_SM_MAXTASKQUEUE + 1)
/*
*

*/
typedef enum {
    HAL_SM_RUNSTA_UNINIT,
    HAL_SM_RUNSTA_IDLE,
    HAL_SM_RUNSTA_RUN,
    HAL_SM_RUNSTA_BANNED = 0xFF,
}HAL_SM_RUNSTATE_E;

typedef enum {
    HAL_SM_PRIORITY_NORMAL,
    HAL_SM_PRIORITY_VH,
    HAL_SM_PRIORITY_H,
    HAL_SM_PRIORITY_L,
    HAL_SM_PRIORITY_VL,
}HAL_SM_PRIORITY_E;

#pragma pack(4) 
typedef struct HAL_SM_FUNCNODE_S
{
    HAL_FUNTIONPOINT_NRNA_P TaskFuncPointer;
    uint32_t FuncID;
    uint32_t PrequestFuncID;
    uint32_t MinCycTime;
    uint32_t TimeCounter;
    uint32_t CounterInFunc;
    HAL_SM_PRIORITY_E Priority;
    HAL_SM_RUNSTATE_E RunState;
    HAL_SM_RUNSTATE_E PrequestFuncState;
}HAL_SM_FUNCNODE_S;

typedef struct HAL_SM_FUNCLINKEDLIST_S
{
    struct HAL_SM_FUNCLINKEDLIST_S *NextNode;
    HAL_SM_FUNCNODE_S *FuncNode;

}HAL_SM_FUNCLINKEDLIST_S;
#pragma pack() 
extern volatile uint32_t gu32TimeBase;

extern void HAL_SM_MainLoop (void);
extern void HAL_SM_TimeCounter (uint32_t* u32TimeBase);
extern void HAL_SM_Init(void);

extern uint32_t HAL_SM_AddTask(HAL_SM_FUNCNODE_S *sInputFuncNode);
extern uint32_t HAL_SM_DelTask(uint32_t u32InFuncID);
extern uint32_t HAL_SM_ChangeNodeState (uint32_t u32InFuncID ,HAL_SM_RUNSTATE_E NewState);
extern uint32_t HAL_SM_SetMinCycTime(uint32_t u32InFuncID,uint32_t u32InputTime);
extern HAL_SM_FUNCNODE_S* HAL_SM_RequestFuncNode(uint32_t u32InFuncID);
extern HAL_SM_RUNSTATE_E HAL_SM_RequestFuncState(uint32_t u32InFuncID);
#endif
