#include "norflash.h"
#include "qspi.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//NOR FLASH(W25QXX) QPI模式驱动代码	
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2019/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

u16 NORFLASH_TYPE=W25Q128;	//默认是W25Q128
u8 NORFLASH_QPI_MODE=0;		//QSPI模式标志:0,SPI模式;1,QPI模式.

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q64
//容量为8M字节,共有128个Block,2048个Sector 
													 
//初始化SPI FLASH的IO口
void NORFLASH_Init(void)
{ 
	u8 temp;
	QSPI_Init();					//初始化QSPI
 	NORFLASH_Qspi_Enable();			//使能QSPI模式
	NORFLASH_TYPE=NORFLASH_ReadID();//读取FLASH ID.
	if(NORFLASH_TYPE == W25Q128)      //SPI W25Q128
    {
		NORFLASH_Write_Enable();	//写使能
		QSPI_Send_CMD(W25X_SetReadParam,0,(3<<6)|(0<<4)|(0<<2)|(3<<0),0);	//QPI,设置读参数指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
		temp=3<<4;					//设置P4&P5=11,8个dummy clocks,104M
		QSPI_Transmit(&temp,1);		//发送1个字节	
    }

}  
//W25QXX进入QSPI模式 
void NORFLASH_Qspi_Enable(void)
{
	u8 stareg2=0;
	stareg2=NORFLASH_ReadSR(2);		//先读出状态寄存器2的原始值 
	//printf("stareg2:%x\r\n",stareg2);
	if((stareg2&0X02)==0)			//QE位未使能
	{ 
		NORFLASH_Write_Enable();		//写使能 
		stareg2|=1<<1;				//使能QE位		
		NORFLASH_Write_SR(2,stareg2);	//写状态寄存器2
	}
	QSPI_Send_CMD(W25X_EnterQPIMode,0,(0<<6)|(0<<4)|(0<<2)|(1<<0),0);	//写command指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据
	NORFLASH_QPI_MODE=1;				//标记QSPI模式
}

//W25QXX退出QSPI模式 
void NORFLASH_Qspi_Disable(void)
{ 
	QSPI_Send_CMD(W25X_ExitQPIMode,0,(0<<6)|(0<<4)|(0<<2)|(3<<0),0);	//写command指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	NORFLASH_QPI_MODE=0;				//标记SPI模式
}

//读取W25QXX的状态寄存器，W25QXX一共有3个状态寄存器
//状态寄存器1：
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
//状态寄存器2：
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//状态寄存器3：
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:状态寄存器号，范:1~3
//返回值:状态寄存器值
u8 NORFLASH_ReadSR(u8 regno)   
{  
	u8 byte=0,command=0; 
    switch(regno)
    {
        case 1:
            command=W25X_ReadStatusReg1;    //读状态寄存器1指令
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //读状态寄存器2指令
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //读状态寄存器3指令
            break;
        default:
            command=W25X_ReadStatusReg1;    
            break;
    }   
	if(NORFLASH_QPI_MODE)QSPI_Send_CMD(command,0,(3<<6)|(0<<4)|(0<<2)|(3<<0),0);//QPI,写command指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
	else QSPI_Send_CMD(command,0,(1<<6)|(0<<4)|(0<<2)|(1<<0),0);				//SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
	QSPI_Receive(&byte,1);	        
	return byte;   
}   

//写W25QXX状态寄存器
void NORFLASH_Write_SR(u8 regno,u8 sr)   
{   
    u8 command=0;
    switch(regno)
    {
        case 1:
            command=W25X_WriteStatusReg1;    //写状态寄存器1指令
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //写状态寄存器2指令
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //写状态寄存器3指令
            break;
        default:
            command=W25X_WriteStatusReg1;    
            break;
    }   
	if(NORFLASH_QPI_MODE)QSPI_Send_CMD(command,0,(3<<6)|(0<<4)|(0<<2)|(3<<0),0);//QPI,写command指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
	else QSPI_Send_CMD(command,0,(1<<6)|(0<<4)|(0<<2)|(1<<0),0);				//SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
	QSPI_Transmit(&sr,1);	         	      
}  

//W25QXX写使能	
//将S1寄存器的WEL置位   
void NORFLASH_Write_Enable(void)   
{
	if(NORFLASH_QPI_MODE)QSPI_Send_CMD(W25X_WriteEnable,0,(0<<6)|(0<<4)|(0<<2)|(3<<0),0);	//QPI,写使能指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	else QSPI_Send_CMD(W25X_WriteEnable,0,(0<<6)|(0<<4)|(0<<2)|(1<<0),0);					//SPI,写使能指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据
} 

//W25QXX写禁止	
//将WEL清零  
void NORFLASH_Write_Disable(void)   
{  
	if(NORFLASH_QPI_MODE)QSPI_Send_CMD(W25X_WriteDisable,0,(0<<6)|(0<<4)|(0<<2)|(3<<0),0);	//QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	else QSPI_Send_CMD(W25X_WriteDisable,0,(0<<6)|(0<<4)|(0<<2)|(1<<0),0);					//SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据 
} 

//返回值如下:				   
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64 
//0XEF17,表示芯片型号为W25Q128 	  
//0XEF18,表示芯片型号为W25Q256
u16 NORFLASH_ReadID(void)
{
	u8 temp[2];
	u16 deviceid;
	if(NORFLASH_QPI_MODE)QSPI_Send_CMD(W25X_ManufactDeviceID,0,(3<<6)|(2<<4)|(3<<2)|(3<<0),0);	//QPI,读id,地址为0,4线传输数据_24位地址_4线传输地址_4线传输指令,无空周期,2个字节数据
	else QSPI_Send_CMD(W25X_ManufactDeviceID,0,(1<<6)|(2<<4)|(1<<2)|(1<<0),0);					//SPI,读id,地址为0,单线传输数据_24位地址_单线传输地址_单线传输指令,无空周期,2个字节数据
	QSPI_Receive(temp,2);
	deviceid=(temp[0]<<8)|temp[1];
	return deviceid;
}    

//读取SPI FLASH,仅支持QPI模式  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(最大32bit)
//NumByteToRead:要读取的字节数(最大65535)
void NORFLASH_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
	QSPI_Send_CMD(W25X_FastReadData,ReadAddr,(3<<6)|(2<<4)|(3<<2)|(3<<0),8);	//QPI,快速读数据,地址为ReadAddr,4线传输数据_24位地址_4线传输地址_4线传输指令,8空周期,NumByteToRead个数据
	QSPI_Receive(pBuffer,NumByteToRead); 
}  


//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(最大32bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void NORFLASH_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	NORFLASH_Write_Enable();					//写使能
	QSPI_Send_CMD(W25X_PageProgram,WriteAddr,(3<<6)|(2<<4)|(3<<2)|(3<<0),0);	//QPI,页写指令,地址为WriteAddr,4线传输数据_24位地址_4线传输地址_4线传输指令,无空周期,NumByteToWrite个数据
	QSPI_Transmit(pBuffer,NumByteToWrite);	         	      
	NORFLASH_Wait_Busy();					   //等待写入结束
} 

//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(最大32bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void NORFLASH_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		NORFLASH_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	}   
} 

//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(最大32bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
u8 NORFLASH_BUFFER[4096];		 
void NORFLASH_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * NORFLASH_BUF;	  
   	NORFLASH_BUF=NORFLASH_BUFFER;	     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		NORFLASH_Read(NORFLASH_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(NORFLASH_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			NORFLASH_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				NORFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			NORFLASH_Write_NoCheck(NORFLASH_BUF,secpos*4096,4096);	//写入整个扇区   
		}else NORFLASH_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 
}

//擦除整个芯片		  
//等待时间超长...
void NORFLASH_Erase_Chip(void)   
{                                   
    NORFLASH_Write_Enable();					//SET WEL 
    NORFLASH_Wait_Busy();   
	QSPI_Send_CMD(W25X_ChipErase,0,(0<<6)|(0<<4)|(0<<2)|(3<<0),0);//QPI,写全片擦除指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	NORFLASH_Wait_Busy();						//等待芯片擦除结束
} 

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个扇区的最少时间:150ms
void NORFLASH_Erase_Sector(u32 Dst_Addr)   
{  
	 
 	//printf("fe:%x\r\n",Dst_Addr);		//监视falsh擦除情况,测试用  	  
 	Dst_Addr*=4096;
    NORFLASH_Write_Enable();			//SET WEL 	 
    NORFLASH_Wait_Busy();  
	QSPI_Send_CMD(W25X_SectorErase,Dst_Addr,(0<<6)|(2<<4)|(3<<2)|(3<<0),0);//QPI,写扇区擦除指令,地址为0,无数据_24位地址_4线传输地址_4线传输指令,无空周期,0个字节数据
    NORFLASH_Wait_Busy();				//等待擦除完成
}

//等待空闲
void NORFLASH_Wait_Busy(void)   
{   
	while((NORFLASH_ReadSR(1)&0x01)==0x01);	// 等待BUSY位清空
}   
























