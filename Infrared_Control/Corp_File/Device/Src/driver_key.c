/******************************************************************************
 *                                  ͷ�ļ�
*******************************************************************************/
#include "driver_key.h"
#include "HAL_Control_ADC.h"
#include "HAL_Control_GPIO.h"
/******************************************************************************
 *                                ����ȫ�ֱ���
*******************************************************************************/
static uint8_t key_num = 0, key_num_bak = 0;                    //��ǰ��ֵ�����ݼ�ֵ��0=�޼����£�1=key1��2=key2��...��5=key5
static uint8_t key_jz_t = 0;                                    //�̻���ֹ�ظ���Ӧ��־��0=������Ӧ��1=��ֹ��Ӧ
static uint8_t key_xd_counter = KEY_XD_DL;                      //����������������ʱ�ݼ��������ж��Ƿ�Ϊ��Ч����
static uint8_t key_lj_counter = KEY_LJ_DL;                      //������ʱ������������ʱ�ݼ�������ȷ��������Ӧʱ����
static uint8_t key_cj_counter = 0;                              //������ʱ������������������ʼ�����������ж϶̻��볤��
uint8_t key_lj_valid_flag = 0;                                  //������Ч״̬��־��0=���������ֻ�δ������1=������Ч

KEY_EVEN key_1, key_2, key_3, key_4, key_5, key_6, key_test;    //�����¼�����

/******************************************************************************
 * FunctionName : key_scan
 * Description  : �����˿�ɨ�躯��
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
 * Description  : ����ɨ����ж�����
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void key_scan_task(void)
{
    key_num = key_scan();                                           //ɨ�谴���˿ڣ���ȡ��ǰ��ֵ
    if (!key_num)                                                   //����ǰ�ް������»��߰����ѵ���
    {
        if ((key_cj_counter > 0) && (key_cj_counter < KEY_CJ_DL))   //0<key_cj_counter<KEY_CJ_DL�����ж�Ϊһ�ζ̻��ͷţ�����Ӧ
        {                                                           //key_cj_counter==0�����ж�Ϊ�ް������»�������δ�����������ͷţ�������Ӧ
                                                                    //key_cj_counter>=KEY_CJ_DL�����ж�Ϊһ�γ����ͷţ�������Ӧ
            /*�̻��ͷź���Ӧ*/
            switch (key_num_bak)                                    //ע�⣺��ʱֻ���ñ��ݼ�ֵ���жϣ���Ϊ��ǰ��ֵ�����㣡
            {
                case KEY_VALUE_TEST :
                    key_test.even_short_release = 1;
                    break;
                default :
                    ;
            }
        }
        key_num_bak = 0;                                            //���±��ݼ�ֵΪ0
        key_jz_t = 0;                                               //������Ӧ����
        key_xd_counter = KEY_XD_DL;                                 //������������ʼ��
        key_lj_counter = KEY_LJ_DL;                                 //������ʱ��������ʼ��
        key_cj_counter = 0;                                         //������ʱ����������
        key_lj_valid_flag = 0;                                      //������Ч״̬
        return ;                                                    //�����а��������ȴ��°����¼�
    }
    
    if (key_num != key_num_bak)                                     //����ǰ��ֵ�����ڱ��ݼ�ֵ����һ��ɨ�赽�ð�����
    {
        key_num_bak = key_num;                                      //���±��ݼ�ֵ
        key_jz_t = 0;                                               //������Ӧ����
        key_xd_counter = KEY_XD_DL;                                 //������������ʼ��
        key_lj_counter = KEY_LJ_DL;                                 //������ʱ��������ʼ��
        key_cj_counter = 0;                                         //������ʱ����������
        key_lj_valid_flag = 0;                                      //������Ч״̬
    }
    
    if (key_xd_counter)                                             //������ʼ�����ж��ټ������ܱ�֤����׼ȷ���������ȼ������жϻᵼ��ʵ���������ȼ���ֵ��1����
    {
        key_xd_counter--;
        return ;                                                    //����δ��ɣ������а�������
    }
    
    if (key_jz_t == 0)                                              //�������������̻�δ��Ӧ������������Ӧ
    {
        /*�̻�������Ӧ*/
        switch (key_num)
        {
            case KEY_VALUE_TEST :
                key_test.even_short_confirm = 1;
                break;
            default :
                ;
        }
        key_jz_t = 1;                                               //�̻�����Ӧһ�Σ���ֹ�ظ���Ӧ
    }
    
    if (key_lj_counter)                                             //������������������ʱδ����������ʱ������-1����10ms��
        key_lj_counter--;
    else                                                            //������������������ʱ�ѵ��������������Ӧ������������ʱ������
    {
        /*����������������Ӧ*/
        switch (key_num)
        {
            case KEY_VALUE_TEST :
                key_test.even_continue = 1;
                break;
            default :
                ;
        }
        key_lj_valid_flag = 1;                                      //������Ч״̬
        key_lj_counter = KEY_LJ_DL - 1;                             //ע�⣺��������ʱ��ֵΪKEY_LJ_DL-1������KEY_LJ_DL����
    }
    
    if (key_cj_counter < KEY_CJ_DL)                                 //������������������ʱδ����������ʱ������+1����10ms��
        key_cj_counter++;
    else if (key_cj_counter == KEY_CJ_DL)                           //������������������ʱ�պõ��ڳ�����ʱ����ʱ����Ӧ����
    {
        /*����������Ӧ*/
        switch (key_num)
        {
            case KEY_VALUE_TEST :
                key_test.even_long_confirm = 1;
                break;
            default :
                ;
        }
        key_cj_counter++;                                           //������Ӧ����key_cj_count����KEY_CJ_DL����ʹ�������ͷţ�����Ҳ�����ظ���Ӧ
    }
}

/******************************************************************************
 * FunctionName : key_clear_evens
 * Description  : ��հ����¼�
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
 * Description  : �������ܳ�ʼ��
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
//void key_init(void)
//{
//    sch_add_task(key_scan_task, 0, 10, HSPD);   //����ɨ����ж�����10ms���ڣ������ȼ�
//}
