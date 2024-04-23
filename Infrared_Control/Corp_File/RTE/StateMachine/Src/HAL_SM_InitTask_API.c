/**
  ******************************************************************************
  * @file    HAL_SM_InitTask_API.c
  * @author  Nausak(Du)
  * @brief   This File Include Some operation for Initialize Device.
  ******************************************************************************
  * @attention
  *     1.Func Name,Argument,Return Should Not Modified!!  Change That Must have Authorization.
  *
  ******************************************************************************
  */

#include "HAL_SM_InitTask_API.h"
#include "HAL_SM_MainLoop.h"
#include "HAL_SM_TaskID.h"
#include "HAL_Control_Export_API.h"



/**
  * @brief  Initialize Device Operation When Device Wakeup from Chip Sleep Mode  .
  * @param  None
  * @retval None;
  * @attention : this Func depend on HAL_SM_DeviceInitTask,this is a sub func
  */
void HAL_SM_AwakenInit (void)
{

}

/**
  * @brief  Initialize Device Operation .
  * @param  None;
  * @retval None;

  */
void HAL_SM_DeviceInitTask (void)
{
    HAL_Control_DeviceInit();
    HAL_SM_ChangeNodeState(TASK_ID_SM_DEVINIT,HAL_SM_RUNSTA_BANNED);
}
