/******************************************************************************
 *                                  头文件
*******************************************************************************/
#include "driver_key.h"
#include "HAL_Control_ADC.h"
#include "HAL_Control_GPIO.h"
/******************************************************************************
 *                                定义全局变量
*******************************************************************************/
static uint8_t key_num = 0, key_num_bak = 0;                    //当前键值及备份键值：0=无键按下，1=key1，2=key2，...，5=key5
static uint8_t key_jz_t = 0;                                    //短击禁止重复响应标志：0=允许响应，1=禁止响应
static uint8_t key_xd_counter = KEY_XD_DL;                      //消抖计数器：消抖时递减计数，判断是否为有效按键
static uint8_t key_lj_counter = KEY_LJ_DL;                      //连击延时计数器：连击时递减计数，确定连击响应时间间隔
static uint8_t key_cj_counter = 0;                              //长击延时计数器：消抖结束后开始递增计数，判断短击与长击
uint8_t key_lj_valid_flag = 0;                                  //连击有效状态标志：0=连击已松手或未触发，1=连击有效

KEY_EVEN key_1, key_2, key_3, key_4, key_5, key_6, key_test;    //按键事件变量

/******************************************************************************
 * FunctionName : key_scan
 * Description  : 按键端口扫描函数
 * Parameters   : none
 * Returns      : none
*******************************************************************************/

static uint8_t key_scan(void)
{
//    uint32_t u32RESValue = 0;
//    uint32_t u32TempADCValue = 0;
//    u32TempADCValue =  HAL_ADC_ReqChannelDate(HAL_ADC_CH1) * 0.80566406;
//    if (u32TempADCValue <= 3000)
//        {
//        u32RESValue = 3300 * u32TempADCValue / ( 3300 - u32TempADCValue);
//        
//        if(u32RESValue < 200 && u32RESValue > 50 )
//            return KEY_VALUE_1;
//        
//        if(u32RESValue < 1500 && u32RESValue > 500 )
//            return KEY_VALUE_2;
//        
//        if(u32RESValue < 4000 && u32RESValue > 2000 )
//            return KEY_VALUE_3;
//        
//        if(u32RESValue < 8000 && u32RESValue > 6000 )
//            return KEY_VALUE_4;
//    }

    if (HAL_IO_ReadPin(HAL_IOCLASS_A, HAL_IOPIN_15) == 0)
        return KEY_VALUE_TEST;
    
    return KEY_VALUE_NONE;
}

/******************************************************************************
 * FunctionName : key_scan_task
 * Description  : 按键扫描和判断任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void key_scan_task(void)
{
    key_num = key_scan();                                           //扫描按键端口，获取当前键值
    if (!key_num)                                                   //若当前无按键按下或者按键已弹起
    {
        if ((key_cj_counter > 0) && (key_cj_counter < KEY_CJ_DL))   //0<key_cj_counter<KEY_CJ_DL，可判断为一次短击释放，需响应
        {                                                           //key_cj_counter==0，可判断为无按键按下或者消抖未结束按键已释放，不需响应
                                                                    //key_cj_counter>=KEY_CJ_DL，可判断为一次长击释放，不需响应
            /*短击释放后响应*/
            switch (key_num_bak)                                    //注意：此时只能用备份键值来判断，因为当前键值已清零！
            {
                case KEY_VALUE_TEST :
                    key_test.even_short_release = 1;
                    break;
                default :
                    ;
            }
        }
        key_num_bak = 0;                                            //更新备份键值为0
        key_jz_t = 0;                                               //允许响应按键
        key_xd_counter = KEY_XD_DL;                                 //消抖计数器初始化
        key_lj_counter = KEY_LJ_DL;                                 //连击延时计数器初始化
        key_cj_counter = 0;                                         //长击延时计数器清零
        key_lj_valid_flag = 0;                                      //连击无效状态
        return ;                                                    //不进行按键处理，等待新按键事件
    }
    
    if (key_num != key_num_bak)                                     //若当前键值不等于备份键值（第一次扫描到该按键）
    {
        key_num_bak = key_num;                                      //更新备份键值
        key_jz_t = 0;                                               //允许响应按键
        key_xd_counter = KEY_XD_DL;                                 //消抖计数器初始化
        key_lj_counter = KEY_LJ_DL;                                 //连击延时计数器初始化
        key_cj_counter = 0;                                         //长击延时计数器清零
        key_lj_valid_flag = 0;                                      //连击无效状态
    }
    
    if (key_xd_counter)                                             //消抖开始（先判断再计数才能保证计数准确，若采用先计数再判断会导致实际周期数比计数值少1！）
    {
        key_xd_counter--;
        return ;                                                    //消抖未完成，不进行按键处理
    }
    
    if (key_jz_t == 0)                                              //消抖结束，若短击未响应过，则立即响应
    {
        /*短击立即响应*/
        switch (key_num)
        {
            case KEY_VALUE_TEST :
                key_test.even_short_confirm = 1;
                break;
            default :
                ;
        }
        key_jz_t = 1;                                               //短击已响应一次，禁止重复响应
    }
    
    if (key_lj_counter)                                             //消抖结束，若连击延时未到，连击延时计数器-1（减10ms）
        key_lj_counter--;
    else                                                            //消抖结束，若连击延时已到，则进行连击响应并重置连击延时计数器
    {
        /*持续按键的连击响应*/
        switch (key_num)
        {
            case KEY_VALUE_TEST :
                key_test.even_continue = 1;
                break;
            default :
                ;
        }
        key_lj_valid_flag = 1;                                      //连击有效状态
        key_lj_counter = KEY_LJ_DL - 1;                             //注意：连续计数时初值为KEY_LJ_DL-1而不是KEY_LJ_DL！！
    }
    
    if (key_cj_counter < KEY_CJ_DL)                                 //消抖结束，若长击延时未到，长击延时计数器+1（加10ms）
        key_cj_counter++;
    else if (key_cj_counter == KEY_CJ_DL)                           //消抖结束，若长击延时刚好等于长击延时常数时，响应长击
    {
        /*长击立即响应*/
        switch (key_num)
        {
            case KEY_VALUE_TEST :
                key_test.even_long_confirm = 1;
                break;
            default :
                ;
        }
        key_cj_counter++;                                           //长击响应后，令key_cj_count大于KEY_CJ_DL，即使按键不释放，长击也不会重复响应
    }
}

/******************************************************************************
 * FunctionName : key_clear_evens
 * Description  : 清空按键事件
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void key_clear_evens(KEY_EVEN *key_p)
{
    key_p->even_short_release = 0;
    key_p->even_short_confirm = 0;
    key_p->even_continue      = 0;
    key_p->even_long_confirm  = 0;
}

/******************************************************************************
 * FunctionName : key_init
 * Description  : 按键功能初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
//void key_init(void)
//{
//    sch_add_task(key_scan_task, 0, 10, HSPD);   //按键扫描和判断任务：10ms周期，高优先级
//}
