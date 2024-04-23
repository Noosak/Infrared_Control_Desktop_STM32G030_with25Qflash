/******************************************************************************
 *                                  头文件
*******************************************************************************/
#include "driver_memory.h"
#include "sha3.h"
#include "driver_flash.h"

#include <stdlib.h>
#include <string.h>
/******************************************************************************
 *                             定义设备信息存储变量
*******************************************************************************/
uint64_t my_dev_addr = 0xFFFFFF;            //设备地址
uint64_t my_host_addr = 0xFFFF;             //主机地址
uint64_t my_version_num = 0x000000;         //固件版本号
uint64_t my_channel_num = 0x00;             //当前信道号
uint64_t my_sp_mode = 0x00;                 //特殊模式编号
uint64_t my_group_flag = 0;                 //广播组标志
uint64_t my_date_and_time = 0;              //固件编译时间


static uint32_t chip_uniqueid[3];           //MCU的UID号
const char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

/******************************************************************************
 * FunctionName : memory_dev_addr_write
 * Description  : 写入本机设备地址
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void memory_dev_addr_write(uint64_t dev_addr)
{
    if (memory_dev_addr_read() != dev_addr)
        Int_flash_write(MEMORY_DEV_ADDR, &dev_addr, 1);
}

/******************************************************************************
 * FunctionName : memory_host_addr_write
 * Description  : 写入本机主机地址
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void memory_host_addr_write(uint64_t host_addr)
{
    if (memory_host_addr_read() != host_addr)
        Int_flash_write(MEMORY_HOST_ADDR, &host_addr, 1);
}

/******************************************************************************
 * FunctionName : memory_version_num_write
 * Description  : 写入本机固件版本号
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void memory_version_num_write(uint64_t version_num)
{
    if (memory_version_num_read() != version_num)
        Int_flash_write(MEMORY_VERSION_NUM, &version_num, 1);
}

/******************************************************************************
 * FunctionName : memory_channel_num_write
 * Description  : 写入本机当前信道号
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void memory_channel_num_write(uint64_t channel_num)
{
    if (memory_channel_num_read() != channel_num)
        Int_flash_write(MEMORY_CHANNEL_NUM, &channel_num, 1);
}

/******************************************************************************
 * FunctionName : memory_sp_mode_write
 * Description  : 写入本机特殊模式编号
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void memory_sp_mode_write(uint64_t sp_mode)
{
    if (memory_sp_mode_read() != sp_mode)
        Int_flash_write(MEMORY_SP_MODE, &sp_mode, 1);
}

/******************************************************************************
 * FunctionName : memory_group_flag_write
 * Description  : 写入本机广播组标志
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void memory_group_flag_write(uint64_t group_flag)
{
    if (memory_group_flag_read() != group_flag)
        Int_flash_write(MEMORY_GROUP_FLAG, &group_flag, 1);
}

/******************************************************************************
 * FunctionName : memory_date_and_time_write
 * Description  : 写入本机固件编译时间
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void memory_date_and_time_write(uint64_t date_and_time)
{
    if (memory_date_and_time_read() != date_and_time)
        Int_flash_write(MEMORY_DATE_AND_TIME, &date_and_time, 1);
}

/******************************************************************************
 * FunctionName : memory_dev_addr_read
 * Description  : 读取本机设备地址
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
uint64_t memory_dev_addr_read(void)
{
    uint64_t dev_addr_temp;
    
    Int_flash_read(MEMORY_DEV_ADDR, &dev_addr_temp, 1);
    
    return dev_addr_temp;
}

/******************************************************************************
 * FunctionName : memory_host_addr_read
 * Description  : 读取本机主机地址
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
uint64_t memory_host_addr_read(void)
{
    uint64_t host_addr_temp;
    
    Int_flash_read(MEMORY_HOST_ADDR, &host_addr_temp, 1);
    
    return host_addr_temp;
}

/******************************************************************************
 * FunctionName : memory_version_num_read
 * Description  : 读取本机固件版本号
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
uint64_t memory_version_num_read(void)
{
    uint64_t version_num_temp;
    
    Int_flash_read(MEMORY_VERSION_NUM, &version_num_temp, 1);
    
    return version_num_temp;
}

/******************************************************************************
 * FunctionName : memory_channel_num_read
 * Description  : 读取本机当前信道号
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
uint64_t memory_channel_num_read(void)
{
    uint64_t channel_num_temp;
    
    Int_flash_read(MEMORY_CHANNEL_NUM, &channel_num_temp, 1);
    
    return channel_num_temp;
}

/******************************************************************************
 * FunctionName : memory_sp_mode_read
 * Description  : 读取本机特殊模式编号
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
uint64_t memory_sp_mode_read(void)
{
    uint64_t sp_mode_temp;
    
    Int_flash_read(MEMORY_SP_MODE, &sp_mode_temp, 1);
    
    return sp_mode_temp;
}

/******************************************************************************
 * FunctionName : memory_group_flag_read
 * Description  : 读取本机广播分组标志
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
uint64_t memory_group_flag_read(void)
{
    uint64_t group_flag_temp;
    
    Int_flash_read(MEMORY_GROUP_FLAG, &group_flag_temp, 1);
    
    return group_flag_temp;
}

/******************************************************************************
 * FunctionName : memory_date_and_time_read
 * Description  : 读取本机固件编译时间
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
uint64_t memory_date_and_time_read(void)
{
    uint64_t date_and_time_temp;
    
    Int_flash_read(MEMORY_DATE_AND_TIME, &date_and_time_temp, 1);
    
    return date_and_time_temp;
}

/******************************************************************************
 * FunctionName : memory_read_chip_uniqueid
 * Description  : 读取MCU的UID号
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void memory_read_chip_uniqueid(void)
{
    chip_uniqueid[0] = LL_GetUID_Word0();
    chip_uniqueid[1] = LL_GetUID_Word1();
    chip_uniqueid[2] = LL_GetUID_Word2();
}

/******************************************************************************
 * FunctionName : memory_crc8
 * Description  : 获取CRC8校验值
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
//static uint8_t memory_crc8(uint8_t *ptr, uint8_t len)
//{
//    uint8_t crc = 0;
//    uint8_t i;
//    
//    while (len--)
//    {
//        crc ^= *ptr++;
//        
//        for (i = 0; i < 8; i++)
//        {
//            if (crc & 0x01)
//                crc = (crc >> 1) ^ 0x8C;
//            else
//                crc >>= 1;
//        }
//    }
//    
//    return crc;
//}

/******************************************************************************
 * FunctionName : memory_get_dev_addr
 * Description  : 获取设备地址
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void memory_get_dev_addr(uint8_t *p_dev_addr)
{
//    uint8_t uid_temp[4] = { 0 };
//    uint8_t len = 0;
//    
//    len = sizeof(chip_uniqueid) / sizeof(uint32_t);
//    
//    while (len > 0)
//    {
//        uid_temp[0] = ((chip_uniqueid[len - 1] >> 24) & 0xff);
//        uid_temp[1] = ((chip_uniqueid[len - 1] >> 16) & 0xff);
//        uid_temp[2] = (chip_uniqueid[len - 1] >> 8) & 0xff;
//        uid_temp[3] = (chip_uniqueid[len - 1] & 0xff);
//        
//        p_dev_addr[len - 1] = memory_crc8(uid_temp, sizeof(uid_temp));
//        
//        len--;
//    }
    
    sha3(chip_uniqueid, 12, p_dev_addr, 3);
}

/******************************************************************************
 * FunctionName : memory_get_date_and_time
 * Description  : 获取编译日期和时间
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void memory_get_date_and_time(uint8_t *p_date_and_time)
{
    char temp_date[] = __DATE__;
    char temp_time[] = __TIME__;
    uint8_t i;
    
    p_date_and_time[2] = atoi(temp_date + 9);       //年
    *(temp_date + 6) = 0;
    p_date_and_time[0] = atoi(temp_date + 4);       //日
    *(temp_date + 3) = 0;
    for (i = 0; i < 12; i++)                        //月
    {
        if (!strcmp(temp_date, months[i]))
        {
            p_date_and_time[1] = i + 1;
            break;
        }
    }
    
    p_date_and_time[4 + 0] = atoi(temp_time + 6);   //秒
    *(temp_time + 5) = 0;
    p_date_and_time[4 + 1] = atoi(temp_time + 3);   //分
    *(temp_time + 2) = 0;
    p_date_and_time[4 + 2] = atoi(temp_time);       //时
    
    p_date_and_time[3] = 0;
    p_date_and_time[4 + 3] = 0;
}

/******************************************************************************
 * FunctionName : memory_init
 * Description  : 本机设备信息初始化
 *                注意：在FLASH写入过程中掉电将导致当前操作的扇区数据全部清空，因此
 *                      千万不要在此处添加任何写FLASH的语句，否则在频繁拔插电源时将
 *                      导致数据丢失！！
 *                注意: STM32L系列FLASH擦除后的初始值为0x00！
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void memory_init(void)
{
    my_dev_addr = memory_dev_addr_read() & 0xFFFFFF;
    
    my_host_addr = memory_host_addr_read() & 0xFFFF;
    
    my_version_num = memory_version_num_read() & 0xFFFFFF; 
    
    my_channel_num = memory_channel_num_read() & 0xFF;
    
    my_sp_mode = memory_sp_mode_read() & 0xFF;
    
    my_group_flag = memory_group_flag_read();
    
    my_date_and_time = memory_date_and_time_read();
    
    /*若无有效设备地址，自动生成设备地址*/
    if (my_dev_addr == 0xFFFFFF)
    {
        memory_read_chip_uniqueid();
        memory_get_dev_addr((uint8_t *)(&my_dev_addr));
        memory_dev_addr_write(my_dev_addr);
    }
    
    /*若无有效信道，默认设置为0信道*/
    if (my_channel_num == 0xFF)
    {
        my_channel_num = 0x00;
        memory_channel_num_write(my_channel_num);
    }
    
    /*若无有效广播分组标志，默认设置为不属于任何组*/
    if (my_group_flag == 0xFFFFFFFFFFFFFFFF)
    {
        my_group_flag = 0;
        memory_group_flag_write(my_group_flag);
    }
    
    /*若无有效固件编译时间，获取并保存*/
    if (my_date_and_time == 0xFFFFFFFFFFFFFFFF)
    {
        memory_get_date_and_time((uint8_t *)(&my_date_and_time));
        memory_date_and_time_write(my_date_and_time);
    }
}
