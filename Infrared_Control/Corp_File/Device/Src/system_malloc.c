/******************************************************************************
 *                                  头文件
*******************************************************************************/
#include "system_malloc.h"	    

/******************************************************************************
 *                                  定义内存池（32字节对齐）
*******************************************************************************/
#if SRAMBANK == 2
__align(32) u8 mem1base[MEM1_MAX_SIZE];                                                 //内部SRAM内存池
__align(32) u8 mem2base[MEM2_MAX_SIZE] __attribute__((at(0X68000000)));                 //外部SRAM内存池
#elif SRAMBANK == 1
__align(32) u8 mem1base[MEM1_MAX_SIZE] __attribute__((at(0x20001400)));                                                 //内部SRAM内存池
#endif

/******************************************************************************
 *                                  定义内存管理表
*******************************************************************************/
#if SRAMBANK == 2
u16 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];                                                 //内部SRAM内存池MAP
u16 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000 + MEM2_MAX_SIZE))); //外部SRAM内存池MAP
#elif SRAMBANK == 1
u16 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];                                                 //内部SRAM内存池MAP
#endif

/******************************************************************************
 *                                  定义内存管理参数
*******************************************************************************/
#if SRAMBANK == 2
const u32 memtblsize[SRAMBANK] = { MEM1_ALLOC_TABLE_SIZE, MEM2_ALLOC_TABLE_SIZE };      //内存表大小
const u32 memblksize[SRAMBANK] = { MEM1_BLOCK_SIZE, MEM2_BLOCK_SIZE };                  //内存分块大小
const u32 memsize[SRAMBANK]    = { MEM1_MAX_SIZE, MEM2_MAX_SIZE };                      //内存总大小
#elif SRAMBANK == 1
const u32 memtblsize[SRAMBANK] = { MEM1_ALLOC_TABLE_SIZE };                             //内存表大小
const u32 memblksize[SRAMBANK] = { MEM1_BLOCK_SIZE };                                   //内存分块大小
const u32 memsize[SRAMBANK]    = { MEM1_MAX_SIZE };                                     //内存总大小
#endif

/******************************************************************************
 *                                  定义内存管理控制器
*******************************************************************************/
#if SRAMBANK == 2
struct _m_mallco_dev mallco_dev =
{
    my_mem_init,                //内存初始化
    my_mem_perused,             //内存使用率
    mem1base,mem2base,          //内存池
    mem1mapbase,mem2mapbase,    //内存管理表
    0,0                         //内存管理未就绪
};
#elif SRAMBANK == 1
struct _m_mallco_dev mallco_dev =
{
    my_mem_init,                //内存初始化
    my_mem_perused,             //内存使用率
    mem1base,                   //内存池
    mem1mapbase,                //内存管理表
    0                           //内存管理未就绪
};
#endif

/******************************************************************************
 * FunctionName : mymemcpy
 * Description  : 复制内存
 * Parameters   : *des：目的地址
 *                *src：源地址
 *                n：需要复制的内存长度(字节为单位)
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
 * Description  : 设置内存
 * Parameters   : *s：内存首地址
 *                c：要设置的值
 *                count：需要设置的内存大小(字节为单位)
 * Returns      : none
*******************************************************************************/
static void mymemset(void *s, u8 c, u32 count)
{
    u8 *xs = s;
    
    while (count--) *xs++ = c;
}

/******************************************************************************
 * FunctionName : my_mem_init
 * Description  : 内存管理初始化
 * Parameters   : memx：内存池操作符，指定要操作的内存池
 * Returns      : none
*******************************************************************************/
void my_mem_init(u8 memx)
{
    mymemset(mallco_dev.memmap[memx], 0, memtblsize[memx] * 2);     //内存管理表所有数据清零
    mymemset(mallco_dev.membase[memx], 0, memsize[memx]);           //内存池所有数据清零
    mallco_dev.memrdy[memx] = 1;                                    //内存管理初始化OK
}

/******************************************************************************
 * FunctionName : my_mem_perused
 * Description  : 获取内存使用率
 * Parameters   : memx：内存池操作符，指定要操作的内存池
 * Returns      : 使用率(0~100)
*******************************************************************************/
u8 my_mem_perused(u8 memx)
{
    u32 used = 0;
    u32 i;
    
    for (i = 0; i < memtblsize[memx]; i++)      //统计已分配的内存块的数量
    {
        if (mallco_dev.memmap[memx][i])
            used++;
    }
    
    return (used * 100) / (memtblsize[memx]);   //计算使用率并返回
}

/******************************************************************************
 * FunctionName : my_mem_malloc
 * Description  : 内存分配(内部调用)
 * Parameters   : memx：内存池操作符，指定要操作的内存池
 *                size：要分配的内存大小(字节)
 * Returns      : 0xFFFFFFFF：代表错误
 *                其他：内存偏移地址
*******************************************************************************/
static u32 my_mem_malloc(u8 memx, u32 size)
{
    signed long offset = 0;
    u32 nmemb;                      //需要的内存块数
    u32 cmemb = 0;                  //连续空内存块数
    u32 i;
    
    if (!mallco_dev.memrdy[memx]) mallco_dev.init(memx);        //未初始化，先执行初始化
    
    if (size == 0) return 0xFFFFFFFF;                           //不需要分配
    
    nmemb = size / memblksize[memx];                            //获取需要分配的连续内存块数
    if (size % memblksize[memx]) nmemb++;
    
    for (offset = memtblsize[memx] - 1; offset >= 0; offset--)  //搜索整个内存控制区
    {
        if (!mallco_dev.memmap[memx][offset]) cmemb++;          //连续空内存块数增加
        else cmemb = 0;                                         //连续内存块清零
        
        if (cmemb == nmemb)                                     //找到了连续nmemb个空内存块
        {
            for (i = 0; i < nmemb; i++)                         //标注内存块非空
            {
                mallco_dev.memmap[memx][offset + i] = nmemb;
            }
            
            return (offset * memblksize[memx]);                 //返回偏移地址
        }
    }
    
    return 0xFFFFFFFF;                                          //未找到符合分配条件的内存块
}

/******************************************************************************
 * FunctionName : my_mem_free
 * Description  : 释放内存(内部调用)
 * Parameters   : memx：内存池操作符，指定要操作的内存池
 *                offset：内存地址偏移
 * Returns      : 返回值：0，释放成功
 *                        1，释放失败
*******************************************************************************/
static u8 my_mem_free(u8 memx, u32 offset)
{
    int i;
    
    if (!mallco_dev.memrdy[memx])                               //未初始化,先执行初始化
    {
        mallco_dev.init(memx);
        return 1;                                               //未初始化  
    }
    
    if (offset < memsize[memx])                                 //偏移在内存池内
    {
        int index = offset / memblksize[memx];                  //偏移所在内存块号码  
        int nmemb = mallco_dev.memmap[memx][index];             //内存块数量
        
        for (i = 0; i < nmemb; i++)                             //内存块清零
        {
            mallco_dev.memmap[memx][index + i] = 0;
        }
        return 0;
    }
    else
        return 2;                                               //偏移超区了
}

/******************************************************************************
 * FunctionName : myfree
 * Description  : 释放内存(外部调用)
 * Parameters   : memx：内存池操作符，指定要操作的内存池
 *                ptr：内存首地址
 * Returns      : none
*******************************************************************************/
void myfree(u8 memx, void *ptr)
{
    u32 offset;
    
    if (ptr == NULL)                                    //地址为0，无法进行释放
        return;
    
    offset = (u32)ptr - (u32)mallco_dev.membase[memx];  //获取偏移地址
    my_mem_free(memx, offset);                          //释放内存
}

/******************************************************************************
 * FunctionName : mymalloc
 * Description  : 分配内存(外部调用)
 * Parameters   : memx：内存池操作符，指定要操作的内存池
 *                size：内存大小(字节)
 * Returns      : 返回值：分配到的内存首地址
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
 * Description  : 重新分配内存(外部调用)
 * Parameters   : memx：内存池操作符，指定要操作的内存池
 *                ptr：旧内存首地址
 *                size：要分配的内存大小(字节)
 * Returns      : 返回值：新分配到的内存首地址
*******************************************************************************/
void *myrealloc(u8 memx, void *ptr, u32 size)
{
    u32 offset;
    
    offset = my_mem_malloc(memx, size);
    if (offset == 0xFFFFFFFF)
        return NULL;
    else
    {
        mymemcpy((void *)((u32)mallco_dev.membase[memx] + offset), ptr, size);  //拷贝旧内存内容到新内存
        myfree(memx, ptr);                                                      //释放旧内存
        return (void *)((u32)mallco_dev.membase[memx] + offset);                //返回新内存首地址
    }
}
