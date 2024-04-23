/******************************************************************************
 *                                  头文件
*******************************************************************************/
#include "api_rf_comm.h"
#include "radio.h"
#include "driver_memory.h"
#include "ircode_emit.h"
#include "Task_433Communicate.h"
/******************************************************************************
 *                              定义RF通信控制变量
*******************************************************************************/
RF_COMM_STRUCT rf_comm;

#define VERSION_INFO "SW1.0.0HW1.0.0@DIR"
/******************************************************************************
 * FunctionName : rf_handshake_respond_or_initiate
 * Description  : RF握手回复或主动握手
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void rf_handshake_respond_or_initiate(uint8_t cmd_type)
{
    rf_txbuf[0] = 0x08;
    rf_txbuf[1] = (my_host_addr == 0xFFFF) ? 0x0A : 0x02;
    rf_txbuf[2] = (uint8_t)(my_dev_addr >> 16);
    rf_txbuf[3] = (uint8_t)(my_dev_addr >> 8);
    rf_txbuf[4] = (uint8_t)my_dev_addr;
    rf_txbuf[5] = (uint8_t)(my_host_addr >> 8);
    rf_txbuf[6] = (uint8_t)my_host_addr;
    rf_txbuf[7] = RF_DEV_TYPE;
    rf_txbuf[8] = RF_CMD_HANDSHAKE;
    
    /*添加包序号*/
    if (cmd_type)           //发送
    {
        rf_txbuf[1] |= 0x20;
        rf_txbuf[0] += 1;
        rf_txbuf[rf_txbuf[0]] = rf_comm.packet_num++;
        if (rf_comm.packet_num == 0xFF) rf_comm.packet_num++;
    }
    else                    //回复
    {
        /*添加通信发起者标志*/
        rf_txbuf[1] |= 0x40;
        
        if (rf_comm.data_config & 0x20)
        {
            rf_txbuf[1] |= 0x20;
            rf_txbuf[0] += 1;
            rf_txbuf[rf_txbuf[0]] = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
        }
    }
    
    /*数据包入队*/
    if (rf_tx_queue->len >= 16)
        queue_de_discard(rf_tx_queue);
    queue_en(rf_tx_queue, (uint8_t *)rf_txbuf);
}

/******************************************************************************
 * FunctionName : rf_address_respond_or_initiate
 * Description  : RF地址回复或主动请求
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void rf_address_respond_or_initiate(uint8_t cmd_type)
{
    rf_txbuf[0] = 0x08;
    rf_txbuf[1] = (my_host_addr == 0xFFFF) ? 0x0A : 0x02;
    if (my_dev_addr == 0xFFFFFF)
        rf_txbuf[1] |= 0x04;
    rf_txbuf[2] = (uint8_t)(my_dev_addr >> 16);
    rf_txbuf[3] = (uint8_t)(my_dev_addr >> 8);
    rf_txbuf[4] = (uint8_t)my_dev_addr;
    rf_txbuf[5] = (uint8_t)(my_host_addr >> 8);
    rf_txbuf[6] = (uint8_t)my_host_addr;
    rf_txbuf[7] = RF_DEV_TYPE;
    rf_txbuf[8] = RF_CMD_ADDRESS;
    
    /*添加包序号*/
    if (cmd_type)           //发送
    {
        rf_txbuf[1] |= 0x20;
        rf_txbuf[0] += 1;
        rf_txbuf[rf_txbuf[0]] = rf_comm.packet_num++;
        if (rf_comm.packet_num == 0xFF) rf_comm.packet_num++;
    }
    else                    //回复
    {
        /*添加通信发起者标志*/
        rf_txbuf[1] |= 0x40;
        
        if (rf_comm.data_config & 0x20)
        {
            rf_txbuf[1] |= 0x20;
            rf_txbuf[0] += 1;
            rf_txbuf[rf_txbuf[0]] = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
        }
    }
    
    /*数据包入队*/
    if (rf_tx_queue->len >= 16)
        queue_de_discard(rf_tx_queue);
    queue_en(rf_tx_queue, (uint8_t *)rf_txbuf);
}

/******************************************************************************
 * FunctionName : rf_bound_respond
 * Description  : RF绑定回复
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void rf_bound_respond(void)
{
    rf_txbuf[0] = 0x08;
    rf_txbuf[1] = (my_host_addr == 0xFFFF) ? 0x0A : 0x02;
    rf_txbuf[2] = (uint8_t)(my_dev_addr >> 16);
    rf_txbuf[3] = (uint8_t)(my_dev_addr >> 8);
    rf_txbuf[4] = (uint8_t)my_dev_addr;
    rf_txbuf[5] = (uint8_t)(my_host_addr >> 8);
    rf_txbuf[6] = (uint8_t)my_host_addr;
    rf_txbuf[7] = RF_DEV_TYPE;
    rf_txbuf[8] = RF_CMD_BOUND;
    
    /*添加通信发起者标志*/
    rf_txbuf[1] |= 0x40;
    
    /*添加包序号*/
    if (rf_comm.data_config & 0x20)
    {
        rf_txbuf[1] |= 0x20;
        rf_txbuf[0] += 1;
        rf_txbuf[rf_txbuf[0]] = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
    }
    
    /*数据包入队*/
    if (rf_tx_queue->len >= 16)
        queue_de_discard(rf_tx_queue);
    queue_en(rf_tx_queue, (uint8_t *)rf_txbuf);
}

/******************************************************************************
 * FunctionName : rf_unbound_respond
 * Description  : RF解绑命令回复
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void rf_unbound_respond(void)
{
    rf_txbuf[0] = 0x08;
    rf_txbuf[1] = (my_host_addr == 0xFFFF) ? 0x0A : 0x02;
    rf_txbuf[2] = (uint8_t)(my_dev_addr >> 16);
    rf_txbuf[3] = (uint8_t)(my_dev_addr >> 8);
    rf_txbuf[4] = (uint8_t)my_dev_addr;
    rf_txbuf[5] = (uint8_t)(my_host_addr >> 8);
    rf_txbuf[6] = (uint8_t)my_host_addr;
    rf_txbuf[7] = RF_DEV_TYPE;
    rf_txbuf[8] = RF_CMD_UNBOUND;
    
    /*添加通信发起者标志*/
    rf_txbuf[1] |= 0x40;
    
    /*添加包序号*/
    if (rf_comm.data_config & 0x20)
    {
        rf_txbuf[1] |= 0x20;
        rf_txbuf[0] += 1;
        rf_txbuf[rf_txbuf[0]] = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
    }
    
    /*数据包入队*/
    if (rf_tx_queue->len >= 16)
        queue_de_discard(rf_tx_queue);
    queue_en(rf_tx_queue, (uint8_t *)rf_txbuf);
}

/******************************************************************************
 * FunctionName : rf_channel_respond
 * Description  : RF跳频回复
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void rf_channel_respond(void)
{
    rf_txbuf[0] = 0x0A;
    rf_txbuf[1] = (my_host_addr == 0xFFFF) ? 0x0A : 0x02;
    rf_txbuf[2] = (uint8_t)(my_dev_addr >> 16);
    rf_txbuf[3] = (uint8_t)(my_dev_addr >> 8);
    rf_txbuf[4] = (uint8_t)my_dev_addr;
    rf_txbuf[5] = (uint8_t)(my_host_addr >> 8);
    rf_txbuf[6] = (uint8_t)my_host_addr;
    rf_txbuf[7] = RF_DEV_TYPE;
    rf_txbuf[8] = RF_CMD_CHANNEL;
    rf_txbuf[9] = rf_comm.host_to_dev_buf[9];
    rf_txbuf[10] = rf_comm.host_to_dev_buf[10];
    
    /*添加通信发起者标志*/
    rf_txbuf[1] |= 0x40;
    
    /*添加包序号*/
    if (rf_comm.data_config & 0x20)
    {
        rf_txbuf[1] |= 0x20;
        rf_txbuf[0] += 1;
        rf_txbuf[rf_txbuf[0]] = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
    }
    
    /*数据包入队*/
    if (rf_tx_queue->len >= 16)
        queue_de_discard(rf_tx_queue);
    queue_en(rf_tx_queue, (uint8_t *)rf_txbuf);
}

/******************************************************************************
 * FunctionName : rf_group_respond
 * Description  : RF广播分组命令回复
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void rf_group_respond(void)
{
    uint8_t i;
    
    rf_txbuf[0] = 0x11;
    rf_txbuf[1] = (my_host_addr == 0xFFFF) ? 0x0A : 0x02;
    rf_txbuf[2] = (uint8_t)(my_dev_addr >> 16);
    rf_txbuf[3] = (uint8_t)(my_dev_addr >> 8);
    rf_txbuf[4] = (uint8_t)my_dev_addr;
    rf_txbuf[5] = (uint8_t)(my_host_addr >> 8);
    rf_txbuf[6] = (uint8_t)my_host_addr;
    rf_txbuf[7] = RF_DEV_TYPE;
    rf_txbuf[8] = RF_CMD_GROUP;
    rf_txbuf[9] = rf_comm.host_to_dev_buf[9];
    for (i = 0; i < 8; i++)
        rf_txbuf[10 + i] = ((uint8_t *)(&my_group_flag))[i];
    
    /*添加通信发起者标志*/
    rf_txbuf[1] |= 0x40;
    
    /*添加包序号*/
    if (rf_comm.data_config & 0x20)
    {
        rf_txbuf[1] |= 0x20;
        rf_txbuf[0] += 1;
        rf_txbuf[rf_txbuf[0]] = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
    }
    
    /*数据包入队*/
    if (rf_tx_queue->len >= 16)
        queue_de_discard(rf_tx_queue);
    queue_en(rf_tx_queue, (uint8_t *)rf_txbuf);
}

/******************************************************************************
 * FunctionName : rf_query_respond
 * Description  : RF查询命令回复
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void rf_query_respond(void)
{
	  rf_txbuf[0] = 0x0A;
    rf_txbuf[1] = (my_host_addr == 0xFFFF) ? 0x0A : 0x02;
    rf_txbuf[2] = (uint8_t)(my_dev_addr >> 16);
    rf_txbuf[3] = (uint8_t)(my_dev_addr >> 8);
    rf_txbuf[4] = (uint8_t)my_dev_addr;
    rf_txbuf[5] = (uint8_t)(my_host_addr >> 8);
    rf_txbuf[6] = (uint8_t)my_host_addr;
    rf_txbuf[7] = RF_DEV_TYPE;
    rf_txbuf[8] = RF_CMD_QUERY;
    rf_txbuf[9] = 0xFF;
    rf_txbuf[10] = 0xFF;
    
    /*添加通信发起者标志*/
    rf_txbuf[1] |= 0x40;
    
    /*添加包序号*/
    if (rf_comm.data_config & 0x20)
    {
        rf_txbuf[1] |= 0x20;
        rf_txbuf[0] += 1;
        rf_txbuf[rf_txbuf[0]] = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
    }
    
    /*数据包入队*/
    if (rf_tx_queue->len >= 16)
        queue_de_discard(rf_tx_queue);
    queue_en(rf_tx_queue, (uint8_t *)rf_txbuf);
}

/******************************************************************************
 * FunctionName : rf_report_initiate
 * Description  : RF主动上报
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void rf_report_initiate(void)
{
    rf_txbuf[0] = 0x08;
    rf_txbuf[1] = (my_host_addr == 0xFFFF) ? 0x0A : 0x02;
#ifdef RF_REPORT_REPEAT_ON
    rf_txbuf[1] |= 0x04;
#endif
    rf_txbuf[2] = (uint8_t)(my_dev_addr >> 16);
    rf_txbuf[3] = (uint8_t)(my_dev_addr >> 8);
    rf_txbuf[4] = (uint8_t)my_dev_addr;
    rf_txbuf[5] = (uint8_t)(my_host_addr >> 8);
    rf_txbuf[6] = (uint8_t)my_host_addr;
    rf_txbuf[7] = RF_DEV_TYPE;
    rf_txbuf[8] = RF_CMD_REPORT;
    
    /*添加包序号*/
    rf_txbuf[1] |= 0x20;
    rf_txbuf[0] += 1;
    rf_txbuf[rf_txbuf[0]] = rf_comm.packet_num++;
    if (rf_comm.packet_num == 0xFF) rf_comm.packet_num++;
    
    /*数据包入队*/
    if (rf_tx_queue->len >= 16)
        queue_de_discard(rf_tx_queue);
    queue_en(rf_tx_queue, (uint8_t *)rf_txbuf);
}

/******************************************************************************
 * FunctionName : rf_set_respond
 * Description  : RF设置命令回复
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static uint8_t irSetEcode = 0;
static void rf_set_respond(void)
{
    rf_txbuf[0] = 0x09;
    rf_txbuf[1] = (my_host_addr == 0xFFFF) ? 0x0A : 0x02;
    rf_txbuf[2] = (uint8_t)(my_dev_addr >> 16);
    rf_txbuf[3] = (uint8_t)(my_dev_addr >> 8);
    rf_txbuf[4] = (uint8_t)my_dev_addr;
    rf_txbuf[5] = (uint8_t)(my_host_addr >> 8);
    rf_txbuf[6] = (uint8_t)my_host_addr;
    rf_txbuf[7] = RF_DEV_TYPE;
    rf_txbuf[8] = RF_CMD_SET;
    rf_txbuf[9] = irSetEcode;
    
    /*添加通信发起者标志*/
    rf_txbuf[1] |= 0x40;
    
    /*添加包序号*/
    if (rf_comm.data_config & 0x20)
    {
        rf_txbuf[1] |= 0x20;
        rf_txbuf[0] += 1;
        rf_txbuf[rf_txbuf[0]] = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
    }
    
    /*数据包入队*/
    if (rf_tx_queue->len >= 16)
        queue_de_discard(rf_tx_queue);
    queue_en(rf_tx_queue, (uint8_t *)rf_txbuf);
}


static void rf_report_version(void)
{
		char tmp_version[26];
		memset(tmp_version,0,sizeof(tmp_version));
		rf_txbuf[0] = 8 + strlen(VERSION_INFO) -1;
    rf_txbuf[1] = (my_host_addr == 0xFFFF) ? 0x0A : 0x02;
    rf_txbuf[2] = (uint8_t)(my_dev_addr >> 16);
    rf_txbuf[3] = (uint8_t)(my_dev_addr >> 8);
    rf_txbuf[4] = (uint8_t)my_dev_addr;
    rf_txbuf[5] = (uint8_t)(my_host_addr >> 8);
    rf_txbuf[6] = (uint8_t)my_host_addr;
    rf_txbuf[7] = RF_DEV_TYPE;
    rf_txbuf[8] = RF_CMD_VERSION;
		strcat(tmp_version,VERSION_INFO);
		memcpy(rf_txbuf + 9 , tmp_version ,strlen(tmp_version) -1);
    
    /*添加通信发起者标志*/
    rf_txbuf[1] |= 0x40;
    
    /*添加包序号*/
    if (rf_comm.data_config & 0x20)
    {
        rf_txbuf[1] |= 0x20;
        rf_txbuf[0] += 1;
        rf_txbuf[rf_txbuf[0]] = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
    }
    
    /*数据包入队*/
    if (rf_tx_queue->len >= 16)
        queue_de_discard(rf_tx_queue);
    queue_en(rf_tx_queue, (uint8_t *)rf_txbuf);
}
/******************************************************************************
 * FunctionName : rf_set_channel_num_invalid_task
 * Description  : RF设置信道确认失效任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void rf_set_channel_num_invalid_task(void)
{
    rf_comm.ch_wait_state = CH_WAIT_NEW_NUM;
}

/******************************************************************************
 * FunctionName : rf_data_packet_num_check
 * Description  : RF接收包序号有效性判断
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static uint8_t rf_data_packet_num_check(uint8_t old_num, uint8_t new_num)
{
    uint8_t temp = 0;
    
    if (new_num == 0xFF)
        return 1;
    
    if (old_num == 0xFF)
        temp = 1;
    else if ((old_num >= 5))
    {
        if ((new_num < old_num - 5) || (new_num > old_num))
            temp = 1;
    }
    else
    {
        if ((new_num < 255 - (5 - old_num)) && (new_num > old_num))
            temp = 1;
    }
    
    return temp;
}

/******************************************************************************
 * FunctionName : rf_data_packet_num_reset_task
 * Description  : RF接收包序号复位任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void rf_data_packet_num_reset_task(void)
{
    rf_comm.packet_num_one = 0xFF;
    rf_comm.packet_num_group = 0xFF;
}

/******************************************************************************
 * FunctionName : rf_packet_num_repetition_respond
 * Description  : RF包序号重复时的回包处理
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void rf_packet_num_repetition_respond(void)
{
    switch (rf_comm.data_cmd)                                   //命令处理
    {
        case RF_CMD_HANDSHAKE :
            {
                if (rf_comm.data_config & 0x04)
                    rf_handshake_respond_or_initiate(0);        //握手回复
            }
            break;
        case RF_CMD_ADDRESS :
            {
                if (rf_comm.data_config & 0x04)
                    rf_address_respond_or_initiate(0);          //地址回复
            }
            break;
        case RF_CMD_BOUND :
            {
                if (rf_comm.data_config & 0x04)
                    rf_bound_respond();                         //绑定回复
            }
            break;
        case RF_CMD_UNBOUND :
            {
                if (rf_comm.data_config & 0x04)
                    rf_unbound_respond();                       //解绑回复
            }
            break;
        case RF_CMD_CHANNEL :
            {
                if (rf_comm.data_config & 0x04)
                    rf_channel_respond();                       //设置信道回复
            }
            break;
        case RF_CMD_GROUP :
            {
                if (rf_comm.data_config & 0x04)
                    rf_group_respond();                         //广播分组回复
            }
            break;
        case RF_CMD_QUERY :
            {
                if (rf_comm.data_config & 0x04)
                    rf_query_respond();                         //查询回复
            }
            break;
				case RF_CMD_VERSION :
            {
                if (rf_comm.data_config & 0x04)
                    rf_report_version();     
            }
            break;
        case RF_CMD_SET :
            {
                if ((rf_comm.data_config & 0x04) && !(rf_comm.data_config & 0x10))
                    rf_set_respond();                           //设置回复
            }
            break;
        default :
            ;
    }
}

/******************************************************************************
 * FunctionName : rf_receive_data_decode
 * Description  : RF接收数据解析
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void rf_receive_data_decode(void)
{
	
	  HAL_INTASK_TIME_S TempTimeFuncNode;
    memset (&TempTimeFuncNode, 0, sizeof(HAL_INTASK_TIME_S));
	
    rf_comm.data_lenght    = rf_comm.host_to_dev_buf[0];
    rf_comm.data_config    = rf_comm.host_to_dev_buf[1];
    rf_comm.data_dev_addr  = (rf_comm.host_to_dev_buf[2] << 16) + (rf_comm.host_to_dev_buf[3] << 8) + rf_comm.host_to_dev_buf[4];
    rf_comm.data_host_addr = (rf_comm.host_to_dev_buf[5] << 8) + rf_comm.host_to_dev_buf[6];
    rf_comm.data_dev_type  = rf_comm.host_to_dev_buf[7];
    rf_comm.data_cmd       = rf_comm.host_to_dev_buf[8];
    
    rf_comm.my_packet_flag = 0;                                 //数据包过滤开始，复位本机数据包标志
    
    if ((rf_comm.data_config & 0x01) != 0x01)                   //传输方向过滤
        return ;
    if (my_host_addr != 0xFFFF)                                 //主机地址过滤
    {
        if (rf_comm.data_host_addr != my_host_addr)
            return ;
    }
    if (rf_comm.data_dev_type != RF_DEV_TYPE)                   //设备类型过滤
        return ;
    if (!(rf_comm.data_config & 0x10))                          //判断是否为广播命令
    {
        if (rf_comm.data_dev_addr != my_dev_addr)               //设备地址过滤
            return ;
    }
    else
    {
        //--------------------------------------------------------------------------
        //广播组号过滤
        if (rf_comm.data_config & 0x20)
            rf_comm.data_group_num = rf_comm.host_to_dev_buf[rf_comm.data_lenght - 1];
        else
            rf_comm.data_group_num = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
        
        if (rf_comm.data_group_num > 0)
        {
            if (!(my_group_flag & (0x01 << (rf_comm.data_group_num - 1))))
                return;
        }
        //--------------------------------------------------------------------------
    }
    
    //--------------------------------------------------------------------------
    //包序号过滤
    if (rf_comm.data_config & 0x20)
    {
        if (!(rf_comm.data_config & 0x10))
        {
            if (rf_comm.data_config & 0x40)
            {
                if (rf_data_packet_num_check(rf_comm.packet_num_one, rf_comm.host_to_dev_buf[rf_comm.data_lenght]))
                    rf_comm.packet_num_one = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
                else
                {
                    rf_packet_num_repetition_respond();
                    return;
                }
            }
            else
            {
                if (rf_comm.host_to_dev_buf[rf_comm.data_lenght] == (rf_comm.packet_num - 1))
                {
                    rf_comm.my_packet_flag = 1;
                    return;
                }
                else
                    return;
            }
        }
        else
        {
            if (rf_data_packet_num_check(rf_comm.packet_num_group, rf_comm.host_to_dev_buf[rf_comm.data_lenght]))
                rf_comm.packet_num_group = rf_comm.host_to_dev_buf[rf_comm.data_lenght];
            else
                return;
        }
    }
    else
    {
        if (!(rf_comm.data_config & 0x10))
            rf_comm.packet_num_one = 0xFF;
        else
            rf_comm.packet_num_group = 0xFF;
    }
    //--------------------------------------------------------------------------
    
    rf_comm.my_packet_flag = 1;                                 //数据包过滤成功，置位本机数据包标志
    
    switch (rf_comm.data_cmd)                                   //命令处理
    {
        case RF_CMD_HANDSHAKE :
            {
                if (rf_comm.data_config & 0x04)
                    rf_handshake_respond_or_initiate(0);        //握手回复
            }
            break;
        case RF_CMD_ADDRESS :
            {
//                my_dev_addr  = (rf_comm.host_to_dev_buf[9] << 16) + (rf_comm.host_to_dev_buf[10] << 8) + rf_comm.host_to_dev_buf[11];
//                memory_dev_addr_write(my_dev_addr);             //写入FLASH
                
                if (rf_comm.data_config & 0x04)
                    rf_address_respond_or_initiate(0);          //地址回复
            }
            break;
        case RF_CMD_BOUND :
            {
                my_host_addr = rf_comm.data_host_addr;
                memory_host_addr_write(my_host_addr);           //写入FLASH
                
                if (rf_comm.data_config & 0x04)
                    rf_bound_respond();                         //绑定回复
            }
            break;
        case RF_CMD_UNBOUND :
            {
                my_host_addr = 0xFFFF;
                memory_host_addr_write(my_host_addr);           //写入FLASH
                
                if (rf_comm.data_config & 0x04)
                    rf_unbound_respond();                       //解绑回复
            }
            break;
				case RF_CMD_VERSION :
            {
                if (rf_comm.data_config & 0x04)
                    rf_report_version();     
            }
            break;
        case RF_CMD_CHANNEL :
            {
                switch (rf_comm.ch_wait_state)
                {
                    case CH_WAIT_NEW_NUM :
                        if (rf_comm.host_to_dev_buf[9] == 0x00)
                        {
                            rf_comm.channel_num_temp = rf_comm.host_to_dev_buf[10];
                            rf_comm.ch_wait_state = CH_WAIT_CHECK;
														TempTimeFuncNode.RunFuncPoint = rf_set_channel_num_invalid_task;       
														TempTimeFuncNode.u32DelayTime = 3000;
														Task_COM_AddCycFunc(pt433CommunicateInTaskFunc,&TempTimeFuncNode);
														TempTimeFuncNode.u32DelayTime = 0;
                            
                            if (rf_comm.data_config & 0x04)
                                rf_channel_respond();                   //设置信道回复
                        }
                        else if (rf_comm.host_to_dev_buf[9] == 0x02)
                        {
                            my_channel_num = rf_comm.host_to_dev_buf[10];
                            memory_channel_num_write(my_channel_num);   //写入FLASH
                            rf_comm.ch_wait_state = CH_WAIT_NEW_NUM;
                            
                            if (rf_comm.data_config & 0x04)
                                rf_channel_respond();                   //设置信道回复
                            else
																CMT2300A_SetFrequencyChannel(my_channel_num);     //更新信道
                        }
                        break;
                    case CH_WAIT_CHECK :
                        if ((rf_comm.host_to_dev_buf[9] == 0x01) && (rf_comm.host_to_dev_buf[10] == rf_comm.channel_num_temp))
                        {
                            Task_Com_DelCycFunc(pt433CommunicateInTaskFunc,rf_set_channel_num_invalid_task);
                            
                            my_channel_num = rf_comm.channel_num_temp;
                            memory_channel_num_write(my_channel_num);   //写入FLASH
                            rf_comm.ch_wait_state = CH_WAIT_NEW_NUM;
                            
                            if (rf_comm.data_config & 0x04)
                                rf_channel_respond();                   //设置信道回复
                            else
                                CMT2300A_SetFrequencyChannel(my_channel_num);     //更新信道
                        }
                        break;
                    default :
                        ;
                }
            }
            break;
        case RF_CMD_GROUP :
            {
                if (rf_comm.host_to_dev_buf[9] == 0x00)
                {
                    memcpy(&my_group_flag, rf_comm.host_to_dev_buf + 10, 8);
                    memory_group_flag_write(my_group_flag);     //写入FLASH
                }
                
                if (rf_comm.data_config & 0x04)
                    rf_group_respond();                         //广播分组回复
            }
            break;
        case RF_CMD_QUERY :
            {
                if (rf_comm.data_config & 0x04)
                    rf_query_respond();                         //查询回复
            }
            break;
        case RF_CMD_SET :
            {
                //提取数据并修改本机各状态变量
//							uint8_t tmp[4] = {0,4,0,1};
//								irSetEcode = ircode_emit_addcode(rf_comm.host_to_dev_buf[9],tmp,rf_comm.host_to_dev_buf[10]);
                irSetEcode = ircode_emit_addcode(rf_comm.host_to_dev_buf[9],rf_comm.host_to_dev_buf + 11,rf_comm.host_to_dev_buf[10]);
                if ((rf_comm.data_config & 0x04) && !(rf_comm.data_config & 0x10))
                    rf_set_respond();                           //设置回复
            }
            break;
						
        default :
            ;
    }
}

/******************************************************************************
 * FunctionName : rf_comm_task
 * Description  : RF通信任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void rf_comm_task(void)
{
    static uint8_t send_repeat_time_cnt = 0;    //重发间隔
    static uint8_t send_repeat_times = 0;       //重发次数
    static uint8_t wait_ack_flag = 0;           //等待回应标志
    
    switch (rf_comm.comm_state)                 //RF通信状态机
    {
        case COMM_RECEIVE :                     //等待接收状态
            {
                if (queue_de(rf_rx_queue, rf_comm.host_to_dev_buf))
                {
                    rf_receive_data_decode();                       //数据包解析
                }
                if (queue_de(rf_tx_queue, rf_comm.dev_to_host_buf))
                {
                    rf_comm.comm_state = COMM_SEND;
                }
                else break;
            }break;
        case COMM_SEND :                        //发送数据状态
            {
                RF_SEND_REPEAT:
                if (rf_sen_fail_count > 0)
                {
                    rf_sen_fail_count--;
                    break;
                }
                RFSendPacket((rf_comm.dev_to_host_buf), rf_comm.dev_to_host_buf[0] + 1);
                if (rf_sen_fail_flag == 1) 
                {
                    rf_sen_fail_count = my_dev_addr % 7;
                    break;
                }
                
                wait_ack_flag = rf_comm.dev_to_host_buf[1] & 0x04;  //判断是否需要等待回应
                if (wait_ack_flag)
                    rf_comm.comm_state = COMM_WAIT_ACK;
                else
                    rf_comm.comm_state = COMM_RECEIVE;
                
                if ((rf_comm.dev_to_host_buf[8] == RF_CMD_CHANNEL) &&
                    (rf_comm.dev_to_host_buf[9] > 0x00))
                {
                    CMT2300A_SetFrequencyChannel(my_channel_num);             //更新信道
                }
            }
            break;
        case COMM_WAIT_ACK :                    //等待回应状态
            {
                if (queue_de(rf_rx_queue, rf_comm.host_to_dev_buf))
                {
                    rf_receive_data_decode();                       //数据包解析
                    
                    //避免等待回应状态被其他命令打断！
                    if (rf_comm.my_packet_flag)
                    {
                        uint8_t match_flag = 0;
                        
                        switch (rf_comm.dev_to_host_buf[8])
                        {
                            case RF_CMD_ADDRESS :
                                {
                                    if ((rf_comm.data_cmd == RF_CMD_ADDRESS) ||
                                        (rf_comm.data_cmd == RF_CMD_HANDSHAKE))
                                        match_flag = 1;
                                }
                                break;
                            default :
                                {
                                    if (rf_comm.data_cmd == rf_comm.dev_to_host_buf[8])
                                        match_flag = 1;
                                }
                        }
                        
                        if (match_flag == 1)
                        {
                            send_repeat_times = 0;
                            send_repeat_time_cnt = 0;
                            rf_comm.comm_state = COMM_RECEIVE;
                            break;
                        }
                    }
                }
                
                //等待回应处理
                send_repeat_time_cnt++;
                if (rf_comm.dev_to_host_buf[8] == RF_CMD_ADDRESS)
                {
                    if (send_repeat_time_cnt >= 200)                //重发间隔1s
                    {
                        send_repeat_time_cnt = 0;
                        if (send_repeat_times < 180)                //重发次数180次
                        {
                            rf_comm.comm_state = COMM_SEND;
                            send_repeat_times++;
                            goto RF_SEND_REPEAT;
                        }
                        else
                        {
                            rf_comm.comm_state = COMM_RECEIVE;
                            send_repeat_times = 0;
                        }
                    }
                }
                else
                {
                    if (send_repeat_time_cnt >= 20)                 //重发间隔100ms
                    {
                        send_repeat_time_cnt = 0;
                        if (send_repeat_times < 4)                  //重发次数4次
                        {
                            rf_comm.comm_state = COMM_SEND;
                            send_repeat_times++;
                            goto RF_SEND_REPEAT;
                        }
                        else
                        {
                            rf_comm.comm_state = COMM_RECEIVE;
                            send_repeat_times = 0;
                        }
                    }
                }
            }
            break;
        default :                               //异常状态处理
            rf_comm.comm_state = COMM_RECEIVE;
    }
}

/******************************************************************************
 * FunctionName : rf_rx_cal_task
 * Description  : RF接收校准任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
//static void rf_rx_cal_task(void)
//{
//    cc1101_rx_cal();
//}

/******************************************************************************
 * FunctionName : rf_power_on_delay_report_task
 * Description  : RF开机延时上报任务
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void rf_power_on_delay_report_task(void)
{
    rf_handshake_respond_or_initiate(1);
}

RF_COMM_STRUCT* rf_comm_RequestRFInfo(void)
{
    return &rf_comm;
}
