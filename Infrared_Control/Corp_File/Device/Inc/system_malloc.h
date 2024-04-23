#ifndef __SYSTEM_MALLOC_H__
#define __SYSTEM_MALLOC_H__

//#include "stm32f1xx.h"
#include "stdint.h"
/******************************************************************************
 *                              ����ͨ�ú�
*******************************************************************************/
#ifndef NULL
#define NULL 0
#endif

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

/******************************************************************************
 *                              ���������ڴ�صĲ�����
*******************************************************************************/
#define SRAMIN	    0		            //�����ڲ��ڴ��
#define SRAMEX      1		            //�����ⲿ�ڴ��

/******************************************************************************
 *                              ����֧�ֵ�SRAM����
*******************************************************************************/
#define SRAMBANK 	1                   //1��ʾֻʹ���ڲ��ڴ�أ�2��ʾͬʱʹ���ڲ����ⲿ�ڴ��

/******************************************************************************
 *                              �����ڴ�ز���
*******************************************************************************/
//mem1�ڴ�����趨.mem1��ȫ�����ڲ�SRAM����
#define MEM1_BLOCK_SIZE			32                                  //�ڴ���СΪ32�ֽ�
#define MEM1_MAX_SIZE			1024//1 * 1024                            //�������ڴ�1K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE / MEM1_BLOCK_SIZE     //�ڴ����С

//mem2�ڴ�����趨.mem2���ڴ�ش����ⲿSRAM����
#define MEM2_BLOCK_SIZE			32                                  //�ڴ���СΪ32�ֽ�
#define MEM2_MAX_SIZE			960 * 1024                          //�������ڴ�960K
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE / MEM2_BLOCK_SIZE     //�ڴ����С 

/******************************************************************************
 *                              �����ڴ��������������
*******************************************************************************/
struct _m_mallco_dev
{
    void (*init)(u8);                   //�ڴ������ʼ���ص�����
    u8 (*perused)(u8);                  //��ȡ�ڴ�ʹ���ʻص�����
    u8 *membase[SRAMBANK];              //�ڴ��
    u16 *memmap[SRAMBANK];              //�ڴ������
    u8 memrdy[SRAMBANK];                //�ڴ��������״̬
};

/******************************************************************************
 *                              �ⲿ��������
*******************************************************************************/
extern struct _m_mallco_dev mallco_dev;

/******************************************************************************
 *                              �ⲿ��������
*******************************************************************************/
extern void mymemcpy(void *des, void *src, u32 n);
extern void my_mem_init(u8 memx);
extern u8 my_mem_perused(u8 memx);
extern void myfree(u8 memx, void *ptr);
extern void *mymalloc(u8 memx, u32 size);
extern void *myrealloc(u8 memx, void *ptr, u32 size);
#endif