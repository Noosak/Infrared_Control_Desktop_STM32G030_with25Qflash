#include "Task_CommonFile.h"


//static uint8_t InTaskTimeHandlerNum  = 0; 
//static HAL_INTASK_TIME_S InTaskTimeHandler_IDLE[MAX_DEPEND_TIME_FUNC];

TASK_SUBFUNC_T* Task_COM_InitSubFuncInfoStruct (void)
{
    TASK_SUBFUNC_T* ptTempSubFunc;
    ptTempSubFunc = (TASK_SUBFUNC_T*)malloc(sizeof(TASK_SUBFUNC_T));
    memset(ptTempSubFunc, 0, sizeof(TASK_SUBFUNC_T));
    return ptTempSubFunc;
}


uint32_t Task_COM_AddCycFunc (TASK_SUBFUNC_T* ptSubFuncInfo,HAL_INTASK_TIME_S *InputCucFuncInfo)
{
    if (ptSubFuncInfo->ProcArryLen < MAX_DEPEND_TIME_FUNC)
    {
        for (uint8_t i = 0; i < ptSubFuncInfo->ProcArryLen; i++)
        {
            if (ptSubFuncInfo->TaskProcArry[i].RunFuncPoint == InputCucFuncInfo->RunFuncPoint)
                return HAL_ERROR_CONFLICT;
        }
        memcpy(&(ptSubFuncInfo->TaskProcArry[ptSubFuncInfo->ProcArryLen]),InputCucFuncInfo,sizeof(HAL_INTASK_TIME_S));
        ptSubFuncInfo->ProcArryLen++;
        return HAL_ERROR_NORMAL;
    }
    return HAL_ERROR_EOF;
}

uint32_t Task_Com_DelCycFunc (TASK_SUBFUNC_T* ptSubFuncInfo,HAL_FUNTIONPOINT_NRNA_P fpDelFuncPoint)
{
    if (fpDelFuncPoint != NULL)
    {
        for (uint8_t i = 0;i < ptSubFuncInfo->ProcArryLen; i++)
        {
            if (ptSubFuncInfo->TaskProcArry[i].RunFuncPoint == fpDelFuncPoint)
            {
                memset(&(ptSubFuncInfo->TaskProcArry[i]),0,sizeof(HAL_INTASK_TIME_S));
                if (i < ptSubFuncInfo->ProcArryLen -1 && i != MAX_DEPEND_TIME_FUNC - 1)
                    for (uint8_t j = i; j < ptSubFuncInfo->ProcArryLen - i; j++)
                        memcpy(&(ptSubFuncInfo->TaskProcArry[j]), &(ptSubFuncInfo->TaskProcArry[j + 1]),sizeof(HAL_INTASK_TIME_S));
                //memset(&(ptSubFuncInfo->TaskProcArry[ptSubFuncInfo->ProcArryLen - 1]), 0, sizeof(HAL_INTASK_TIME_S));
                ptSubFuncInfo->ProcArryLen --;
                return HAL_ERROR_NORMAL;
            }
        }
        return HAL_ERROR_NOTFIND;
    }
    return HAL_ERROR_NULL;
}

void Task_Com_TimeCounterHandler(TASK_SUBFUNC_T* ptSubFuncInfo,uint32_t* u32TimeCounterBase)
{
    volatile HAL_INTASK_TIME_S* TimeHandlerNode = NULL;
    if (ptSubFuncInfo != NULL){
        if ((*u32TimeCounterBase) >= 1)
        {
            for (uint8_t i = 0; i < ptSubFuncInfo->ProcArryLen; i++)
            {
                TimeHandlerNode = &(ptSubFuncInfo->TaskProcArry[i]);
                if(TimeHandlerNode->RunFuncPoint != NULL)
                {
                    if (TimeHandlerNode->u32DelayTime == 0 )
                    {
                        if (TimeHandlerNode->u32TimeCounter >= TimeHandlerNode->u32CycTime )
                        {
                            HAL_ERROR_FuncPointJumpArguCheck(TimeHandlerNode->RunFuncPoint);
                            TimeHandlerNode->u32TimeCounter = 0;
                        }
                        TimeHandlerNode->u32TimeCounter++;
                    }
                    else
                    {
                        if (TimeHandlerNode->u32TimeCounter >= TimeHandlerNode->u32DelayTime)
                        {
                            HAL_ERROR_FuncPointJumpArguCheck(TimeHandlerNode->RunFuncPoint);
                            memset(&(ptSubFuncInfo->TaskProcArry[i]),0,sizeof(HAL_INTASK_TIME_S));
                            if (i < ptSubFuncInfo->ProcArryLen && i != MAX_DEPEND_TIME_FUNC - 1)
                                for (uint8_t j = i; j < ptSubFuncInfo->ProcArryLen - i; j++)
                                    memcpy(&(ptSubFuncInfo->TaskProcArry[j]), &(ptSubFuncInfo->TaskProcArry[j + 1]),sizeof(HAL_INTASK_TIME_S));
                                memset(&(ptSubFuncInfo->TaskProcArry[ptSubFuncInfo->ProcArryLen - 1]), 0, sizeof(HAL_INTASK_TIME_S));
                                ptSubFuncInfo->ProcArryLen --;
                        }
                        else
                            TimeHandlerNode->u32TimeCounter++;
                    }
                }
            }
            (*u32TimeCounterBase) = 0;
        }
    }
}
