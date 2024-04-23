/******************************************************************************
 *                                  头文件
*******************************************************************************/
#include "driver_flash.h"

/******************************************************************************
 *                             定义FLASH扇区数据缓存变量
*******************************************************************************/
uint64_t flash_sector_buf[STM32_SECTOR_SIZE / 8];

//-----------------------------------------------------------------------
void Int_FLASH_Unlock(void)
{
    /* Wait for last operation to be completed */
    while ((FLASH->SR & FLASH_SR_BSY1) != 0)
    {
        /* For robust implementation, add here time-out management */
    }
    
    /* Authorize the FPEC of Bank1 Access */
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
}

void Int_FLASH_Lock(void)
{
    /* Wait for last operation to be completed */
    while ((FLASH->SR & FLASH_SR_BSY1) != 0)
    {
        /* For robust implementation, add here time-out management */
    }
    
    /* Set the Lock Bit to lock the FPEC and the CR of  Bank1 */
    FLASH->CR |= FLASH_CR_LOCK;
}

void Int_FLASH_ErasePage(uint32_t page_addr)
{
    /* Wait for last operation to be completed */
    while ((FLASH->SR & FLASH_SR_BSY1) != 0)
    {
        /* For robust implementation, add here time-out management */
    }
    
    /* if the previous operation is completed, proceed to erase the page */
    FLASH->CR |= FLASH_CR_PER;
    FLASH->CR &= ~FLASH_CR_PNB_Msk;
    FLASH->CR |= ((page_addr - STM32_FLASH_BASE) / STM32_SECTOR_SIZE) << FLASH_CR_PNB_Pos;
    FLASH->CR |= FLASH_CR_STRT;
    
    /* Wait for last operation to be completed */
    while ((FLASH->SR & FLASH_SR_BSY1) != 0)
    {
        /* For robust implementation, add here time-out management */
    }
    
    /* Disable the PER Bit */
    FLASH->CR &= ~FLASH_CR_PER;
}

void Int_FLASH_ClearFlag(uint32_t FLASH_FLAG)
{
	  while ((FLASH->SR & FLASH_SR_BSY1) != 0)
    {
        /* For robust implementation, add here time-out management */
    }
		/* Clear the flags */
		FLASH->SR = FLASH_FLAG;
		__ISB();
}

void Int_FLASH_ProgramDoubleWord(volatile uint32_t flash_addr,volatile uint64_t data)
{
    /* Wait for last operation to be completed */
    while ((FLASH->SR & FLASH_SR_BSY1) != 0)
    {
        /* For robust implementation, add here time-out management */
    }

    /* Set PG bit */
    FLASH->CR |= FLASH_CR_PG;
    
    /* Program first word */
    *(uint32_t *)flash_addr = (uint32_t)data;
    
    /* Barrier to ensure programming is performed in 2 steps, in right order
      (independently of compiler optimization behavior) */
    __ISB();
    
    /* Program second word */
    *(uint32_t *)(flash_addr + 4U) = (uint32_t)(data >> 32U);
    
    /* Wait for last operation to be completed */
    while ((FLASH->SR & FLASH_SR_BSY1) != 0)
    {
        /* For robust implementation, add here time-out management */
    }

    /* Disable the PG Bit */
    FLASH->CR &= ~FLASH_CR_PG;
		
		while ((FLASH->SR & FLASH_SR_BSY1) != 0)
		{
				/* For robust implementation, add here time-out management */
		}
}
//-----------------------------------------------------------------------

/******************************************************************************
 * FunctionName : flash_read_double_word
 * Description  : 读取指定地址的双字(64位数据)
 * Parameters   : read_addr ：读地址(此地址必须为8的倍数!!)
 * Returns      : 对应数据
*******************************************************************************/
uint64_t Int_flash_read_double_word(uint32_t read_addr)
{
	while ((FLASH->SR & FLASH_SR_BSY1) != 0)
	{
			/* For robust implementation, add here time-out management */
	}
	return *(volatile uint64_t *)read_addr;
}

/******************************************************************************
 * FunctionName : flash_read_word
 * Description  : 读取指定地址的字(32位数据)
 * Parameters   : read_addr ：读地址(此地址必须为4的倍数!!)
 * Returns      : 对应数据
*******************************************************************************/
uint32_t Int_flash_read_word(uint32_t read_addr)
{
	while ((FLASH->SR & FLASH_SR_BSY1) != 0)
	{
			/* For robust implementation, add here time-out management */
	}
	return *(volatile uint32_t *)read_addr;
}

/******************************************************************************
 * FunctionName : flash_read_half_word
 * Description  : 读取指定地址的半字(16位数据)
 * Parameters   : read_addr ：读地址(此地址必须为2的倍数!!)
 * Returns      : 对应数据
*******************************************************************************/
uint16_t Int_flash_read_half_word(uint32_t read_addr)
{
	while ((FLASH->SR & FLASH_SR_BSY1) != 0)
	{
			/* For robust implementation, add here time-out management */
	}
	return *(volatile uint16_t *)read_addr;
}

/******************************************************************************
 * FunctionName : flash_read_byte
 * Description  : 读取指定地址的字节(8位数据)
 * Parameters   : read_addr ：读地址
 * Returns      : 对应数据
*******************************************************************************/
uint8_t Int_flash_read_byte(uint32_t read_addr)
{
	while ((FLASH->SR & FLASH_SR_BSY1) != 0)
	{
			/* For robust implementation, add here time-out management */
	}
	return *(volatile uint8_t *)read_addr;
}

/******************************************************************************
 * FunctionName : flash_read
 * Description  : 从指定地址开始读出指定长度的数据
 * Parameters   : read_addr    ：起始地址
 *                pbuffer      ：数据指针
 *                num_to_write ：数据个数
 * Returns      : none
*******************************************************************************/
void Int_flash_read(uint32_t read_addr, uint64_t *pbuffer, uint16_t num_to_read)
{
	uint16_t i;
    
	for (i = 0; i < num_to_read; i++)
	{
		pbuffer[i] = Int_flash_read_double_word(read_addr);     //读取8个字节
		read_addr += 8;                                     //地址增加8
	}
}

/******************************************************************************
 * FunctionName : flash_write_nocheck
 * Description  : 不检查的写入
 * Parameters   : write_addr   ：起始地址
 *                pbuffer      ：数据指针
 *                num_to_write ：数据个数
 * Returns      : none
*******************************************************************************/
void Int_flash_write_nocheck(volatile uint32_t write_addr,volatile uint64_t* pbuffer,volatile uint16_t num_to_write)   
{
	uint16_t i;
    
	for (i = 0; i < num_to_write; i++)
	{
		Int_FLASH_ProgramDoubleWord(write_addr, pbuffer[i]);    //写入8个字节
	    write_addr += 8;                                    //地址增加8
	}
}

/******************************************************************************
 * FunctionName : flash_write
 * Description  : 从指定地址开始写入指定长度的数据
 * Parameters   : write_addr   ：起始地址(此地址必须为8的倍数!!)
 *                pbuffer      ：数据指针
 *                num_to_write ：数据个数
 * Returns      : none
*******************************************************************************/
void Int_flash_write(uint32_t write_addr, uint64_t *pbuffer, uint16_t num_to_write)
{
    uint32_t offaddr;      //FLASH内偏移地址（8位）
    uint32_t secpos;       //扇区地址
    uint32_t secoff;       //扇区内偏移地址（64位）
    uint32_t secremain;    //扇区内剩余空间（64位）
    uint32_t i;
    
    if ((write_addr < STM32_FLASH_BASE) ||
        (write_addr >= (STM32_FLASH_BASE + STM32_SECTOR_SIZE * STM32_FLASH_SIZE)))           //非法地址不进行处理
        return ;
    

    
    offaddr = write_addr - STM32_FLASH_BASE;                                    //计算FLASH内偏移地址（8位）
    secpos = offaddr / STM32_SECTOR_SIZE;			                            //计算扇区地址
    secoff = (offaddr % STM32_SECTOR_SIZE) / 8;		                            //计算扇区内偏移地址（64位）
    secremain = STM32_SECTOR_SIZE / 8 - secoff;		                            //计算扇区内剩余空间（64位）
    if (num_to_write <= secremain)                                              //若新数据不能写满扇区剩余空间，则只写入实际数据大小
        secremain = num_to_write;
    
    while (1)
    {
        Int_flash_read(STM32_FLASH_BASE + secpos * STM32_SECTOR_SIZE,
                   flash_sector_buf, STM32_SECTOR_SIZE / 8);                    //读出整个扇区的内容
        for (i = 0; i < secremain; i++)                                         //校验数据
        {
            if (flash_sector_buf[secoff + i] != 0xFFFFFFFFFFFFFFFF)             //若待写入区已有数据则需要擦除
                break;
        }
        if (i < secremain)                                                      //需要擦除
        {
            Int_FLASH_Unlock();                                                             //解锁
            Int_FLASH_ClearFlag(FLASH_SR_EOP|FLASH_SR_BSY1|FLASH_SR_PROGERR|FLASH_SR_PGAERR|FLASH_SR_WRPERR);
						Int_FLASH_ErasePage(STM32_FLASH_BASE + secpos * STM32_SECTOR_SIZE);     //擦除这个扇区
            for (i = 0; i < secremain; i++)                                     //复制新数据到扇区缓存的尾部
            {
                flash_sector_buf[secoff + i] = pbuffer[i];
            }
            Int_flash_write_nocheck(STM32_FLASH_BASE + secpos * STM32_SECTOR_SIZE,
                                flash_sector_buf, STM32_SECTOR_SIZE / 8);       //写入整个扇区
        }
        else                                                                    //不需要擦除
        {
						Int_FLASH_Unlock();                                                             //解锁
						Int_FLASH_ClearFlag(FLASH_SR_EOP|FLASH_SR_BSY1|FLASH_SR_PROGERR|FLASH_SR_PGAERR|FLASH_SR_WRPERR);
            Int_flash_write_nocheck(write_addr, pbuffer, secremain);                //直接写入扇区剩余空间
        }
        
        if (num_to_write == secremain)                                          //写入结束
        {
            break;
        }
        else                                                                    //写入未结束
        {
            secpos++;                                                           //扇区地址增1
            secoff = 0;                                                         //扇区内偏移地址为0
            pbuffer += secremain;                                               //数据指针偏移
            write_addr += secremain;                                            //写地址偏移
            num_to_write -= secremain;                                          //待写入数据的数量递减
            if (num_to_write > (STM32_SECTOR_SIZE / 8))                         //下一个扇区还是写不完
                secremain = STM32_SECTOR_SIZE / 8;
            else                                                                //下一个扇区可以写完了
                secremain = num_to_write;
        }
    }
//    
    Int_FLASH_Lock();                                                               //上锁
}

/******************************************************************************
 * FunctionName : test_write
 * Description  : 一个64位数据的测试写入
 * Parameters   : write_addr ：起始地址(此地址必须为8的倍数!!)
 *                write_data ：要写入的数据
 * Returns      : none
*******************************************************************************/
//void test_write(uint32_t write_addr, uint64_t write_data)
//{
//	flash_write(write_addr, &write_data, 1);
//}

/******************************************************************************
 * FunctionName : flash_init
 * Description  : FLASH功能初始化
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void flash_init(void)
{
    //-----------------------------------------------------------------
//    volatile uint64_t data_test;
//    
//    test_write(STM32_FLASH_BASE + 2048 * 15, 5);
//    data_test = flash_read_double_word(STM32_FLASH_BASE + 2048 * 15);
    //-----------------------------------------------------------------
}
