/**
  ******************************************************************************
  * @file    HAL_SM_MainLoop.c
  * @author  Nausak(Du)
  * @brief   This File Include All of StateMechine Func.
  ******************************************************************************
  * @attention
  *     1.This File Should not Modified!  Change This File Must have Authorization.
  *
  ******************************************************************************
  */

#include "HAL_SM_MainLoop.h"
#include "HAL_GlobalConfig.h"
#include "HAL_SM_DefaultTask_API.h"

#include "system_malloc.h"
#define TRUE 0
#define FAULT 0xFF

enum{
    HAL_SM_AWAKE_POWERON,
    HAL_SM_AWAKE_SLEEP,
}HAL_SM_AWAKEN_E;

typedef struct 
{
    HAL_SM_FUNCLINKEDLIST_S TaskQueue;
    uint8_t AwakenInSleep;
    uint8_t u8QueueTail;
}HAL_SM_GLOBALTASKINFO_S;

/**** Struct Definition Begin****/
static HAL_SM_GLOBALTASKINFO_S gtGlobalTaskInfo;
const HAL_SM_FUNCLINKEDLIST_S *gptGlobalTaskQueue = &(gtGlobalTaskInfo.TaskQueue);
volatile uint32_t gu32TimeBase;
/**** Struct Definition End****/


/**
  * @brief  Add Task's FuncNode into globalTask linked list.
  * @param  need add Task FuncNode
  * @retval Add success or not;
  */
uint32_t HAL_SM_AddTask(HAL_SM_FUNCNODE_S *sInputFuncNode)
{
    volatile HAL_SM_FUNCLINKEDLIST_S* ptTempJumpNode = (HAL_SM_FUNCLINKEDLIST_S*)gptGlobalTaskQueue;

    if (gtGlobalTaskInfo.u8QueueTail < HAL_SM_REALMAXQUENUM){
        for (uint8_t i = 1; i < HAL_SM_REALMAXQUENUM; i++)
        {
            if (sInputFuncNode->FuncID == ptTempJumpNode->FuncNode->FuncID)
                return HAL_ERROR_CONFLICT;
            if (ptTempJumpNode->NextNode != NULL)
            {
                ptTempJumpNode = ptTempJumpNode->NextNode;
            }
            else{
                ptTempJumpNode->NextNode = (HAL_SM_FUNCLINKEDLIST_S*) malloc(sizeof(HAL_SM_FUNCLINKEDLIST_S));
                memset(ptTempJumpNode->NextNode, 0, sizeof(HAL_SM_FUNCLINKEDLIST_S));
                
                ptTempJumpNode->FuncNode = (HAL_SM_FUNCNODE_S*) malloc(sizeof(HAL_SM_FUNCNODE_S));
                memset(ptTempJumpNode->FuncNode, 0, sizeof(HAL_SM_FUNCNODE_S));

                memcpy(ptTempJumpNode->FuncNode, sInputFuncNode, sizeof(HAL_SM_FUNCNODE_S));
                gtGlobalTaskInfo.u8QueueTail++;
                return HAL_ERROR_NORMAL;
            }
        }
    }
    return HAL_ERROR_EOF;
}
/**
  * @brief  Delete Task's FuncNode from globalTask linked list.
  * @param  Delete Task ID
  * @retval Delete success or not;
  */
uint32_t HAL_SM_DelTask(uint32_t u32InFuncID)
{
    volatile HAL_SM_FUNCLINKEDLIST_S* ptAheadNode = NULL;
    volatile HAL_SM_FUNCLINKEDLIST_S* ptTempJumpNode = (HAL_SM_FUNCLINKEDLIST_S*)gptGlobalTaskQueue;

    for (uint8_t i = 0; i < gtGlobalTaskInfo.u8QueueTail; i++)
    {
        if (u32InFuncID != ptTempJumpNode->FuncNode->FuncID)
        {
            ptAheadNode = ptTempJumpNode; 
            ptTempJumpNode = ptTempJumpNode->NextNode;
        }
        else{
            if(ptTempJumpNode->FuncNode != NULL && ptTempJumpNode->FuncNode->TaskFuncPointer != NULL)
            {
                free(ptTempJumpNode->FuncNode);
                ptAheadNode->NextNode = ptTempJumpNode->NextNode;
                free((HAL_SM_FUNCLINKEDLIST_S*)ptTempJumpNode);
                gtGlobalTaskInfo.u8QueueTail--;
                return HAL_ERROR_NORMAL;
            }
        }
    }
    return HAL_ERROR_NOTFIND;
}


/**
  * @brief  Count all task TimeCounter in this Func.
  * @param  None;
  * @retval None;
  * @attention :When the function runs once, TimeCounter self adds once;
  */
void HAL_SM_TimeCounter (uint32_t* u32TimeBase)
{
    static uint32_t u32TempTimeBase;   
    static volatile HAL_SM_FUNCLINKEDLIST_S* ptTempJumpNode;
    if (u32TempTimeBase == *u32TimeBase)
        return ;
    u32TempTimeBase = *u32TimeBase;
    ptTempJumpNode = gptGlobalTaskQueue->NextNode;
    for (uint8_t i = 0; i < gtGlobalTaskInfo.u8QueueTail - 1; i++)
    {
        if((ptTempJumpNode->FuncNode->TimeCounter <= ptTempJumpNode->FuncNode->MinCycTime &&    \
            (ptTempJumpNode->FuncNode->RunState == HAL_SM_RUNSTA_IDLE || ptTempJumpNode->FuncNode->RunState == HAL_SM_RUNSTA_RUN) &&    \
            (ptTempJumpNode->FuncNode->MinCycTime != 0)) || ptTempJumpNode->FuncNode->CounterInFunc == HAL_STA_ENABLE )
        {
            ptTempJumpNode->FuncNode->TimeCounter++;
        }
        ptTempJumpNode = ptTempJumpNode->NextNode;
    }
}


/**
  * @brief  Change Task Running State.
  * @param  u32InFuncID: Changed Task ID
   * @param  NewState: Changede Task state
  * @retval success or not;
  */
uint32_t HAL_SM_ChangeNodeState (uint32_t u32InFuncID ,HAL_SM_RUNSTATE_E NewState)
{
    volatile HAL_SM_FUNCLINKEDLIST_S* ptTempJumpNode = (HAL_SM_FUNCLINKEDLIST_S*)gptGlobalTaskQueue;
	
    for (uint8_t i = 0; i < gtGlobalTaskInfo.u8QueueTail; i++)
    {
        if (ptTempJumpNode->FuncNode->FuncID == u32InFuncID)
        {
            ptTempJumpNode->FuncNode->RunState = NewState;
            break;
        }
        ptTempJumpNode = ptTempJumpNode->NextNode;
    }
    /****Change Other Task PrequestFuncState ****/

    if (gptGlobalTaskQueue->NextNode == NULL)
        return HAL_ERROR_NORMAL;
		ptTempJumpNode = gptGlobalTaskQueue->NextNode;
    for (uint8_t i = 0; i < gtGlobalTaskInfo.u8QueueTail - 1; i++)
    {
        if (ptTempJumpNode->FuncNode->PrequestFuncID == u32InFuncID && ptTempJumpNode->FuncNode->PrequestFuncID != NULL)
        {
            ptTempJumpNode->FuncNode->PrequestFuncState = NewState;
        }
        ptTempJumpNode = ptTempJumpNode->NextNode;
    }
    return HAL_ERROR_NOTFIND;
}

/**
  * @brief  Change Task Minimum Cyclic Time.
  * @param  u32InFuncID: Changed Task ID
  * @param  u32InputTime: Minimum Cyclic Time Value
  * @retval success or not;
  */
uint32_t HAL_SM_SetMinCycTime(uint32_t u32InFuncID,uint32_t u32InputTime)
{
    volatile HAL_SM_FUNCLINKEDLIST_S* ptTempJumpNode = gptGlobalTaskQueue->NextNode;
    for (uint8_t i = 0; i < gtGlobalTaskInfo.u8QueueTail - 1; i++){
        if (ptTempJumpNode->FuncNode->FuncID == u32InFuncID){
            if (ptTempJumpNode->FuncNode->TimeCounter != 0)
                ptTempJumpNode->FuncNode->TimeCounter = 0;
            ptTempJumpNode->FuncNode->MinCycTime = u32InputTime; 
            return HAL_ERROR_NORMAL;
        }
    }
    return HAL_ERROR_NOTFIND;
}


/**
  * @brief  Initialize StateMechine.
  * @param  None
  * @retval None
  */
void HAL_SM_Init(void)
{
    volatile HAL_SM_FUNCLINKEDLIST_S* ptTempJumpNode = (HAL_SM_FUNCLINKEDLIST_S*)gptGlobalTaskQueue;
    uint8_t u8ArryLoopNum = (uint8_t)(sizeof(gtDefaultTaskArry) / sizeof(HAL_SM_FUNCNODE_S)); 
    
    if (u8ArryLoopNum > HAL_SM_MAXTASKQUEUE) 
        while(1);
    memset(&gtGlobalTaskInfo, 0, sizeof(HAL_SM_GLOBALTASKINFO_S));

    for (uint8_t i = 0; i < u8ArryLoopNum; i++ ){
        ptTempJumpNode->NextNode = (HAL_SM_FUNCLINKEDLIST_S*)malloc(sizeof(HAL_SM_FUNCLINKEDLIST_S));
        memset(ptTempJumpNode->NextNode, 0, sizeof(HAL_SM_FUNCLINKEDLIST_S));
        
        ptTempJumpNode->FuncNode = (HAL_SM_FUNCNODE_S*)&(gtDefaultTaskArry[i]);
        
        gtGlobalTaskInfo.u8QueueTail++;
        ptTempJumpNode = ptTempJumpNode->NextNode;
    }
}


/**
  * @brief  Find FuncNode address.
* @param  u32InFuncID: Task ID
  * @retval FuncNode address
  */
HAL_SM_FUNCNODE_S* HAL_SM_RequestFuncNode(uint32_t u32InFuncID) 
{
    volatile HAL_SM_FUNCLINKEDLIST_S* ptTempJumpNode = (HAL_SM_FUNCLINKEDLIST_S*)gptGlobalTaskQueue;
    for (uint8_t i = 0; i < gtGlobalTaskInfo.u8QueueTail; i++){
        if (ptTempJumpNode->FuncNode->FuncID == u32InFuncID){
            return (ptTempJumpNode->FuncNode);
        }
                ptTempJumpNode = ptTempJumpNode->NextNode;
    }
    return NULL;
}

/**
  * @brief  Find FuncNode RunState.
* @param  u32InFuncID: Task ID
  * @retval FuncNode address
  */
HAL_SM_RUNSTATE_E HAL_SM_RequestFuncState(uint32_t u32InFuncID) 
{
    volatile HAL_SM_FUNCLINKEDLIST_S* ptTempJumpNode = (HAL_SM_FUNCLINKEDLIST_S*)gptGlobalTaskQueue;
    for (uint8_t i = 0; i < gtGlobalTaskInfo.u8QueueTail; i++){
        if (ptTempJumpNode->FuncNode->FuncID == u32InFuncID){
            return (ptTempJumpNode->FuncNode->RunState);
        }
                ptTempJumpNode = ptTempJumpNode->NextNode;
    }
    return HAL_SM_RUNSTA_BANNED;
}

/**
  * @brief  Task Run place
  * @param  None
  * @retval None
  */
void HAL_SM_TaskRun(void)
{
    static volatile HAL_SM_FUNCLINKEDLIST_S* ptTempJumpNode;
    ptTempJumpNode = (HAL_SM_FUNCLINKEDLIST_S*)gptGlobalTaskQueue;
    for (uint8_t i = 0; i < gtGlobalTaskInfo.u8QueueTail; i++)
    {
        if((ptTempJumpNode->FuncNode->RunState != HAL_SM_RUNSTA_BANNED) & \
            ((ptTempJumpNode->FuncNode->PrequestFuncState != HAL_SM_RUNSTA_UNINIT) | (ptTempJumpNode->FuncNode->PrequestFuncID == NULL) ) & \
            ((ptTempJumpNode->FuncNode->TimeCounter >= ptTempJumpNode->FuncNode->MinCycTime) | (ptTempJumpNode->FuncNode->MinCycTime == 0) ) & \
            (ptTempJumpNode->FuncNode->TaskFuncPointer != NULL))
        {
            HAL_ERROR_FuncPointJumpArguCheck(ptTempJumpNode->FuncNode->TaskFuncPointer);
        }
        ptTempJumpNode = ptTempJumpNode->NextNode;
    }
}

void HAL_SM_MainLoop(void)
{
    HAL_SM_Init();
    for(;;){
        HAL_SM_TaskRun();
        HAL_SM_TimeCounter((uint32_t*)&gu32TimeBase);
    }
}

