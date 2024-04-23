/******************************************************************************
 *                                  ͷ�ļ�
*******************************************************************************/
#include "system_malloc.h"	    

/******************************************************************************
 *                                  �����ڴ�أ�32�ֽڶ��룩
*******************************************************************************/
#if SRAMBANK == 2
__align(32) u8 mem1base[MEM1_MAX_SIZE];                                                 //�ڲ�SRAM�ڴ��
__align(32) u8 mem2base[MEM2_MAX_SIZE] __attribute__((at(0X68000000)));                 //�ⲿSRAM�ڴ��
#elif SRAMBANK == 1
__align(32) u8 mem1base[MEM1_MAX_SIZE] __attribute__((at(0x20001400)));                                                 //�ڲ�SRAM�ڴ��
#endif

/******************************************************************************
 *                                  �����ڴ�����
*******************************************************************************/
#if SRAMBANK == 2
u16 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];                                                 //�ڲ�SRAM�ڴ��MAP
u16 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000 + MEM2_MAX_SIZE))); //�ⲿSRAM�ڴ��MAP
#elif SRAMBANK == 1
u16 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];                                                 //�ڲ�SRAM�ڴ��MAP
#endif

/******************************************************************************
 *                                  �����ڴ�������
*******************************************************************************/
#if SRAMBANK == 2
const u32 memtblsize[SRAMBANK] = { MEM1_ALLOC_TABLE_SIZE, MEM2_ALLOC_TABLE_SIZE };      //�ڴ���С
const u32 memblksize[SRAMBANK] = { MEM1_BLOCK_SIZE, MEM2_BLOCK_SIZE };                  //�ڴ�ֿ��С
const u32 memsize[SRAMBANK]    = { MEM1_MAX_SIZE, MEM2_MAX_SIZE };                      //�ڴ��ܴ�С
#elif SRAMBANK == 1
const u32 memtblsize[SRAMBANK] = { MEM1_ALLOC_TABLE_SIZE };                             //�ڴ���С
const u32 memblksize[SRAMBANK] = { MEM1_BLOCK_SIZE };                                   //�ڴ�ֿ��С
const u32 memsize[SRAMBANK]    = { MEM1_MAX_SIZE };                                     //�ڴ��ܴ�С
#endif

/******************************************************************************
 *                                  �����ڴ���������
*******************************************************************************/
#if SRAMBANK == 2
struct _m_mallco_dev mallco_dev =
{
    my_mem_init,                //�ڴ��ʼ��
    my_mem_perused,             //�ڴ�ʹ����
    mem1base,mem2base,          //�ڴ��
    mem1mapbase,mem2mapbase,    //�ڴ�����
    0,0                         //�ڴ����δ����
};
#elif SRAMBANK == 1
struct _m_mallco_dev mallco_dev =
{
    my_mem_init,                //�ڴ��ʼ��
    my_mem_perused,             //�ڴ�ʹ����
    mem1base,                   //�ڴ��
    mem1mapbase,                //�ڴ�����
    0                           //�ڴ����δ����
};
#endif

/******************************************************************************
 * FunctionName : mymemcpy
 * Description  : �����ڴ�
 * Parameters   : *des��Ŀ�ĵ�ַ
 *                *src��Դ��ַ
 *                n����Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
 * Returns      : none
*******************************************************************************/
void mymemcpy(void *des, void *src, u32 n)
{
    u8 *xdes = des;
    u8 *xsrc = src;
    
    while (n--) *xdes++ = *xsrc++;
}

/******************************************************************************
 * FunctionName : mymemset
 * Description  : �����ڴ�
 * Parameters   : *s���ڴ��׵�ַ
 *                c��Ҫ���õ�ֵ
 *                count����Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
 * Returns      : none
*******************************************************************************/
static void mymemset(void *s, u8 c, u32 count)
{
    u8 *xs = s;
    
    while (count--) *xs++ = c;
}

/******************************************************************************
 * FunctionName : my_mem_init
 * Description  : �ڴ�����ʼ��
 * Parameters   : memx���ڴ�ز�������ָ��Ҫ�������ڴ��
 * Returns      : none
*******************************************************************************/
void my_mem_init(u8 memx)
{
    mymemset(mallco_dev.memmap[memx], 0, memtblsize[memx] * 2);     //�ڴ�����������������
    mymemset(mallco_dev.membase[memx], 0, memsize[memx]);           //�ڴ��������������
    mallco_dev.memrdy[memx] = 1;                                    //�ڴ�����ʼ��OK
}

/******************************************************************************
 * FunctionName : my_mem_perused
 * Description  : ��ȡ�ڴ�ʹ����
 * Parameters   : memx���ڴ�ز�������ָ��Ҫ�������ڴ��
 * Returns      : ʹ����(0~100)
*******************************************************************************/
u8 my_mem_perused(u8 memx)
{
    u32 used = 0;
    u32 i;
    
    for (i = 0; i < memtblsize[memx]; i++)      //ͳ���ѷ�����ڴ�������
    {
        if (mallco_dev.memmap[memx][i])
            used++;
    }
    
    return (used * 100) / (memtblsize[memx]);   //����ʹ���ʲ�����
}

/******************************************************************************
 * FunctionName : my_mem_malloc
 * Description  : �ڴ����(�ڲ�����)
 * Parameters   : memx���ڴ�ز�������ָ��Ҫ�������ڴ��
 *                size��Ҫ������ڴ��С(�ֽ�)
 * Returns      : 0xFFFFFFFF���������
 *                �������ڴ�ƫ�Ƶ�ַ
*******************************************************************************/
static u32 my_mem_malloc(u8 memx, u32 size)
{
    signed long offset = 0;
    u32 nmemb;                      //��Ҫ���ڴ����
    u32 cmemb = 0;                  //�������ڴ����
    u32 i;
    
    if (!mallco_dev.memrdy[memx]) mallco_dev.init(memx);        //δ��ʼ������ִ�г�ʼ��
    
    if (size == 0) return 0xFFFFFFFF;                           //����Ҫ����
    
    nmemb = size / memblksize[memx];                            //��ȡ��Ҫ����������ڴ����
    if (size % memblksize[memx]) nmemb++;
    
    for (offset = memtblsize[memx] - 1; offset >= 0; offset--)  //���������ڴ������
    {
        if (!mallco_dev.memmap[memx][offset]) cmemb++;          //�������ڴ��������
        else cmemb = 0;                                         //�����ڴ������
        
        if (cmemb == nmemb)                                     //�ҵ�������nmemb�����ڴ��
        {
            for (i = 0; i < nmemb; i++)                         //��ע�ڴ��ǿ�
            {
                mallco_dev.memmap[memx][offset + i] = nmemb;
            }
            
            return (offset * memblksize[memx]);                 //����ƫ�Ƶ�ַ
        }
    }
    
    return 0xFFFFFFFF;                                          //δ�ҵ����Ϸ����������ڴ��
}

/******************************************************************************
 * FunctionName : my_mem_free
 * Description  : �ͷ��ڴ�(�ڲ�����)
 * Parameters   : memx���ڴ�ز�������ָ��Ҫ�������ڴ��
 *                offset���ڴ��ַƫ��
 * Returns      : ����ֵ��0���ͷųɹ�
 *                        1���ͷ�ʧ��
*******************************************************************************/
static u8 my_mem_free(u8 memx, u32 offset)
{
    int i;
    
    if (!mallco_dev.memrdy[memx])                               //δ��ʼ��,��ִ�г�ʼ��
    {
        mallco_dev.init(memx);
        return 1;                                               //δ��ʼ��  
    }
    
    if (offset < memsize[memx])                                 //ƫ�����ڴ����
    {
        int index = offset / memblksize[memx];                  //ƫ�������ڴ�����  
        int nmemb = mallco_dev.memmap[memx][index];             //�ڴ������
        
        for (i = 0; i < nmemb; i++)                             //�ڴ������
        {
            mallco_dev.memmap[memx][index + i] = 0;
        }
        return 0;
    }
    else
        return 2;                                               //ƫ�Ƴ�����
}

/******************************************************************************
 * FunctionName : myfree
 * Description  : �ͷ��ڴ�(�ⲿ����)
 * Parameters   : memx���ڴ�ز�������ָ��Ҫ�������ڴ��
 *                ptr���ڴ��׵�ַ
 * Returns      : none
*******************************************************************************/
void myfree(u8 memx, void *ptr)
{
    u32 offset;
    
    if (ptr == NULL)                                    //��ַΪ0���޷������ͷ�
        return;
    
    offset = (u32)ptr - (u32)mallco_dev.membase[memx];  //��ȡƫ�Ƶ�ַ
    my_mem_free(memx, offset);                          //�ͷ��ڴ�
}

/******************************************************************************
 * FunctionName : mymalloc
 * Description  : �����ڴ�(�ⲿ����)
 * Parameters   : memx���ڴ�ز�������ָ��Ҫ�������ڴ��
 *                size���ڴ��С(�ֽ�)
 * Returns      : ����ֵ�����䵽���ڴ��׵�ַ
*******************************************************************************/
void *mymalloc(u8 memx, u32 size)
{
    u32 offset;
    
    offset = my_mem_malloc(memx, size);
    if (offset == 0xFFFFFFFF)
        return NULL;
    else
        return (void *)((u32)mallco_dev.membase[memx] + offset);
}

/******************************************************************************
 * FunctionName : myrealloc
 * Description  : ���·����ڴ�(�ⲿ����)
 * Parameters   : memx���ڴ�ز�������ָ��Ҫ�������ڴ��
 *                ptr�����ڴ��׵�ַ
 *                size��Ҫ������ڴ��С(�ֽ�)
 * Returns      : ����ֵ���·��䵽���ڴ��׵�ַ
*******************************************************************************/
void *myrealloc(u8 memx, void *ptr, u32 size)
{
    u32 offset;
    
    offset = my_mem_malloc(memx, size);
    if (offset == 0xFFFFFFFF)
        return NULL;
    else
    {
        mymemcpy((void *)((u32)mallco_dev.membase[memx] + offset), ptr, size);  //�������ڴ����ݵ����ڴ�
        myfree(memx, ptr);                                                      //�ͷž��ڴ�
        return (void *)((u32)mallco_dev.membase[memx] + offset);                //�������ڴ��׵�ַ
    }
}
