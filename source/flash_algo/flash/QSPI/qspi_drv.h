#ifndef __QSPI_H
#define __QSPI_H

u8 QSPI_Wait_Flag(u32 flag,u8 sta,u32 wtime);					//QSPI等待某个状态
u8 QSPI_Init(void);												//初始化QSPI
void QSPI_Send_CMD(u8 cmd,u32 addr,u8 mode,u8 dmcycle);			//QSPI发送命令
u8 QSPI_Receive(u8* buf,u32 datalen);							//QSPI接收数据
u8 QSPI_Transmit(u8* buf,u32 datalen);							//QSPI发送数据

#endif

