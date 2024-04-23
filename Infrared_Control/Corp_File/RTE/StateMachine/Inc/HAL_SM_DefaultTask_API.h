#pragma once
#ifndef __HAL_SM_DEFAULTTASK_API_H_
#define __HAL_SM_DEFAULTTASK_API_H_ 

#include "HAL_SM_MainLoop.h"
#include "HAL_SM_InitTask_API.h"
#include "Task_LED.h"
#include "Task_Key.h"
#include "Task_433Communicate.h"
#include "Task_Serial.h"
#include "Task_IREmit.h"
/**** Default Task Begin****/
volatile static HAL_SM_FUNCNODE_S gtDefaultTaskArry[] = 
{
    /****1ST Locate Is ONLY SM&Device Init Guide Func ****/
    {
        .Priority = HAL_SM_PRIORITY_VH,
        .RunState = HAL_SM_RUNSTA_UNINIT,
        .FuncID = TASK_ID_SM_DEVINIT,
        .PrequestFuncID = NULL,
        .TaskFuncPointer = HAL_SM_DeviceInitTask,
    },
    /**** **** **** **** **** **** **** **** ***** ****/
    {
        .Priority = HAL_SM_PRIORITY_NORMAL,
        .RunState = HAL_SM_RUNSTA_UNINIT,
        .FuncID = TASK_ID_LED,
        .PrequestFuncID = TASK_ID_RF,
        .TaskFuncPointer = Task_LED_MainLoop, 
    },

    {
        .Priority = HAL_SM_PRIORITY_NORMAL,
        .RunState = HAL_SM_RUNSTA_UNINIT,
        .FuncID = TASK_ID_Key,
        .PrequestFuncID = TASK_ID_RF,
        .TaskFuncPointer = Task_Key_MainLoop, 
    },
    
    {
        .Priority = HAL_SM_PRIORITY_NORMAL,
        .RunState = HAL_SM_RUNSTA_UNINIT,
        .FuncID = TASK_ID_RF,
        .PrequestFuncID = TASK_ID_SM_DEVINIT,
        .TaskFuncPointer = Task_RF_MainLoop, 
    },
		{
        .Priority = HAL_SM_PRIORITY_NORMAL,
        .RunState = HAL_SM_RUNSTA_UNINIT,
        .FuncID = TASK_ID_Serial,
        .PrequestFuncID = TASK_ID_RF,
        .TaskFuncPointer = Task_Serial_MainLoop, 
    },
		{
        .Priority = HAL_SM_PRIORITY_NORMAL,
        .RunState = HAL_SM_RUNSTA_UNINIT,
        .FuncID = TASK_ID_IREmit,
        .PrequestFuncID = TASK_ID_RF,
        .TaskFuncPointer = Task_IREmit_MainLoop, 
    },
};
/**** Default Task End****/

#endif
