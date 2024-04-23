
#include "HAL_Control_Export_API.h"

#include "HAL_Control_Error.h"

#include "HAL_Control_GPIO.h"
#include "HAL_Control_TIM.h"
#include "HAL_Control_Uart.h"
#include "HAL_Control_ADC.h"

#include "driver_memory.h"
#include "system_malloc.h"
#include "HAL_SM_MainLoop.h"
uint8_t HAL_Control_DeviceInit(void)
{
    gu32TimeBase = 0;
    memory_init();
//    my_mem_init(0);
    HAL_IO_Init();
//    HAL_Control_AllTimerInit();
    HAL_Control_UartInit();
//    HAL_ADC_Init();
    return 0;
}

