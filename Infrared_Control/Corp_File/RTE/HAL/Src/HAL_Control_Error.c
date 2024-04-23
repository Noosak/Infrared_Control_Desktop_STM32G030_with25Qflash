#include "HAL_Control_Error.h"
#include "stdio.h"
HAL_ERROR_TRACK HAL_Error_Track_gt;

void HAL_ERROR_TrackInfoWrite()
{
    sprintf(HAL_Error_Track_gt.ERROR_FUNCNAME,"%s",__func__);
    sprintf(HAL_Error_Track_gt.ERROR_FILENAME,"%s",__FILE__);
    HAL_Error_Track_gt.ERROR_LINE = __LINE__;
}

uint32_t HAL_ERROR_FuncPointJumpArguCheck (HAL_FUNTIONPOINT_NRNA_P pvInputFuncPoint)
{
    if ((uint32_t)(*pvInputFuncPoint) < (0x08000000 + 2048 * 16) && (uint32_t)(*pvInputFuncPoint) > (0x08000000 + 0x400 + 0x600)){
				pvInputFuncPoint();
				return HAL_ERROR_NORMAL;
		}
		return HAL_ERROR_NULL;
}
