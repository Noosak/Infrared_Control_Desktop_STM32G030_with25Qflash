#pragma once
#ifndef __TASK_IREMIT_H_
#define __TASK_IREMIT_H_ 
#include "Task_CommonFile.h"

extern void Task_IREmit_MainLoop (void);
extern void Add_SubIREmitTask(HAL_INTASK_TIME_S* ptInTask);
extern void Del_SubIREmitTask(HAL_FUNTIONPOINT_NRNA_P hook);
#endif
