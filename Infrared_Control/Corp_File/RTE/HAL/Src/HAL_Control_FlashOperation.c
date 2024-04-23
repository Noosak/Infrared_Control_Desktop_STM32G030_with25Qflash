#include "HAL_Control_FlashOperation.h"
#include "HAL_Control_Error.h"

typedef struct 
{
    uint32_t BlockNum;
    uint32_t BlockSize;
    uint32_t StartAddr;

    uint32_t PageSize;
    uint32_t SectorSize;
}HAL_FLASH_ATTRIBUTE_S;

typedef uint32_t (*HAL_FUNTIONPOINT_FLASH_INIT_P)(HAL_FLASH_ATTRIBUTE_S*);

typedef uint32_t (*HAL_FUNTIONPOINT_FLASH_READ_P)(uint32_t BlockNum);
typedef uint32_t (*HAL_FUNTIONPOINT_FLASH_WRITE_P)(HAL_FLASH_ATTRIBUTE_S*);

typedef uint32_t (*HAL_FUNTIONPOINT_FLASH_MODIF_P)(HAL_FLASH_ATTRIBUTE_S*);
typedef uint32_t (*HAL_FUNTIONPOINT_FLASH_APPEND_P)(HAL_FLASH_ATTRIBUTE_S*);

uint32_t HAL_Flash_Init (){

}
