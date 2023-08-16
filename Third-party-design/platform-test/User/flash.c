#include "flash.h"

uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节

/**
  * @brief  Unlock the FLASH control register access
  * @retval LL Status
  */
LL_StatusTypeDef LL_Flash_Unlock(void)
{
	if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
  {
    /* Authorize the FLASH Registers access */
    WRITE_REG(FLASH->KEYR, FLASH_KEY1);
    WRITE_REG(FLASH->KEYR, FLASH_KEY2);

    /* Verify Flash is unlocked */
    if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
    {
      return LL_ERROR;
    }
  }
	else
		return LL_ERROR;
	return LL_OK;
}

/**
  * @brief  Locks the FLASH control register access
  * @retval LL Status
  */
LL_StatusTypeDef LL_FLASH_Lock(void)
{
  /* Set the LOCK Bit to lock the FLASH Registers access */
  SET_BIT(FLASH->CR, FLASH_CR_LOCK);
  
  return LL_OK;
}

/**
  * @brief  Get the FLASH Status
  * @retval LL Status
  */
LL_StatusTypeDef LL_FLASH_GetStatus(void)
{	
	uint32_t res;		
	res=FLASH->SR;
	if(res&(1<<0))return LL_BUSY;		    	//忙
	else if(res&(1<<2))return LL_ERROR;		//编程错误
	else if(res&(1<<4))return LL_PROTECT;	//写保护错误
	return LL_OK;													//操作完成
}

LL_StatusTypeDef LL_FLASH_Wait(void)
{
	LL_StatusTypeDef res;
	uint16_t time = 50000;
	res = LL_FLASH_GetStatus();
	while(res == LL_BUSY)
	{
		res = LL_FLASH_GetStatus();
		if(time==0)
		{
			res = LL_TIMEOUT;
			break;
		}
		time--;
	}
	 return res;
}

LL_StatusTypeDef LL_FLASH_ErasePage(uint32_t PageAddress)
{
	LL_StatusTypeDef res;
	res = LL_FLASH_Wait();
	
	if(res == LL_OK)
	{
		SET_BIT(FLASH->CR, FLASH_CR_PER);			//页擦除
		WRITE_REG(FLASH->AR, PageAddress);		//设置页地址
		SET_BIT(FLASH->CR, FLASH_CR_STRT);		//开始擦除
		res = LL_FLASH_Wait();								//等待操作结束
		if(res != LL_BUSY)
			FLASH->CR &= ~FLASH_CR_PER;					//清除PER位
	}
	return res;
}

LL_StatusTypeDef LL_FLASH_ProgramHalfWord(uint32_t Address,uint16_t Data)
{
	LL_StatusTypeDef res;
	res=LL_FLASH_Wait();
	if(res == LL_OK)
	{
		SET_BIT(FLASH->CR, FLASH_CR_PG);			//编程使能
		*(__IO uint16_t*)Address = Data;			//写入数据
		res=LL_FLASH_Wait();									//等待操作
		if(res != LL_BUSY)										//操作成功
			FLASH->CR &= ~FLASH_CR_PG;					//清除PG位
	}
	return res;
}

#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)
{
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
		LL_FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	  WriteAddr+=2;//地址增加2
	}
}

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=LL_FLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}

//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)
{
	uint32_t secpos;	   	//扇区地址
	uint16_t secoff;	   	//扇区内偏移地址(16位字计算)
	uint16_t secremain; 	//扇区内剩余地址(16位字计算)	   
 	uint16_t i;    
	uint32_t offaddr;   	//去掉0X08000000后的地址
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址
	LL_Flash_Unlock();										//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;				//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	while(1)
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)	//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)				//需要擦除
		{
			LL_FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain*2;	//写地址偏移(16位数据地址,需要*2)	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	}
	LL_FLASH_Lock();//上锁
}
#endif

//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
uint16_t LL_FLASH_ReadHalfWord(uint32_t faddr)
{
	return *(__IO uint16_t*)faddr;
}

uint32_t LL_FLASH_ReadWord(uint32_t faddr)
{
	uint32_t i = 0;
	i = LL_FLASH_ReadHalfWord(faddr+2);
	i = i<<16;
	i |= LL_FLASH_ReadHalfWord(faddr);
	return i;
}

void LL_FLASH_WriteHalfWord(uint32_t WriteAddr,uint16_t data)
{
	STMFLASH_Write(WriteAddr,&data,1);
}

void LL_FLASH_WriteWord(uint32_t WriteAddr,uint32_t data)
{
	uint16_t buffer[2] = {data,data>>16};
	STMFLASH_Write(WriteAddr,buffer,2);
}

