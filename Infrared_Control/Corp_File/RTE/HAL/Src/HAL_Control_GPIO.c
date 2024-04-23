#include <stdint.h>
#include "HAL_Control_GPIO.h"
#include "HAL_GlobalConfig.h"

#include "stm32g0xx_ll_gpio.h"

static void HAL_IO_SetPin (IOCLASS_E IOClass, IOPIN_E IOPin);
static void HAL_IO_ResetPin (IOCLASS_E IOClass, IOPIN_E IOPin);
static void HAL_IO_TogglePin (IOCLASS_E IOClass, IOPIN_E IOPin);

void HAL_IO_Init()
{

}

void HAL_IO_SetPin (IOCLASS_E IOClass, IOPIN_E IOPin)
{
    switch (IOClass){
        case HAL_IOCLASS_A :
            LL_GPIO_SetOutputPin(GPIOA , 0x01UL << IOPin);
        break;
        
        case HAL_IOCLASS_B :
            LL_GPIO_SetOutputPin(GPIOB , 0x01UL << IOPin);
        break;

        case HAL_IOCLASS_C :
            LL_GPIO_SetOutputPin(GPIOC , 0x01UL << IOPin);
        break;

        case HAL_IOCLASS_D :
            LL_GPIO_SetOutputPin(GPIOD , 0x01UL << IOPin);
        break;

        case HAL_IOCLASS_F :
            LL_GPIO_SetOutputPin(GPIOF , 0x01UL << IOPin);
        break;

        default:

        break;
    }
}

void HAL_IO_ResetPin (IOCLASS_E IOClass, IOPIN_E IOPin)
{
    switch (IOClass){
        case HAL_IOCLASS_A :
            LL_GPIO_ResetOutputPin(GPIOA , 0x01UL << IOPin);
        break;

        case HAL_IOCLASS_B :
            LL_GPIO_ResetOutputPin(GPIOB , 0x01UL << IOPin);
        break;

        case HAL_IOCLASS_C :
            LL_GPIO_ResetOutputPin(GPIOC , 0x01UL << IOPin);
        break;

        case HAL_IOCLASS_D :
            LL_GPIO_ResetOutputPin(GPIOD , 0x01UL << IOPin);
        break;

        case HAL_IOCLASS_F :
            LL_GPIO_ResetOutputPin(GPIOF , 0x01UL << IOPin);
        break;

        default:
            
        break;
    }
}

void HAL_IO_TogglePin (IOCLASS_E IOClass, IOPIN_E IOPin)
{
    switch (IOClass){
        case HAL_IOCLASS_A :
            LL_GPIO_TogglePin (GPIOA, 0x01UL << IOPin);
        break;

        case HAL_IOCLASS_B :
            LL_GPIO_TogglePin (GPIOB, 0x01UL << IOPin);
        break;

        case HAL_IOCLASS_C :
            LL_GPIO_TogglePin (GPIOC, 0x01UL << IOPin);
        break;

        case HAL_IOCLASS_D :
            LL_GPIO_TogglePin (GPIOD, 0x01UL << IOPin);
        break;

        case HAL_IOCLASS_F :
            LL_GPIO_TogglePin (GPIOF, 0x01UL << IOPin);
        break;

        default:
            
        break;
    }
}

void HAL_IO_SetState (IOCLASS_E IOClass, IOPIN_E IOPin, IOSTATE_E IOState)
{
    switch (IOState)
    {
    case HAL_IOSTA_RESET :
        HAL_IO_ResetPin(IOClass, IOPin); 
    break;

    case HAL_IOSTA_SET :
        HAL_IO_SetPin(IOClass, IOPin); 
    break;

    case HAL_IOSTA_TOGGLE :
        HAL_IO_TogglePin(IOClass, IOPin); 
    break;
    default:

        break;
    }
}

uint32_t HAL_IO_ReadPin (IOCLASS_E IOClass, IOPIN_E IOPin)
{
    switch (IOClass){
        case HAL_IOCLASS_A :
           return LL_GPIO_IsInputPinSet (GPIOA, 0x01UL << IOPin);

        case HAL_IOCLASS_B :
           return LL_GPIO_IsInputPinSet (GPIOB, 0x01UL << IOPin);

        case HAL_IOCLASS_C :
           return LL_GPIO_IsInputPinSet (GPIOC, 0x01UL << IOPin);

        case HAL_IOCLASS_D :
           return LL_GPIO_IsInputPinSet (GPIOD, 0x01UL << IOPin);

        case HAL_IOCLASS_F :
           return LL_GPIO_IsInputPinSet (GPIOF, 0x01UL << IOPin);

        default:
           return 0;
    }
}
