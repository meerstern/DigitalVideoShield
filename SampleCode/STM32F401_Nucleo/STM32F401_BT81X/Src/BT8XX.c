/* * * * * * * * * * * * * * * * * * * * * * * * * *
 * FTDI EVE Firmware for BT81X  				   *
 * Copyright (c) 2019  							   *
 * K.Watanabe,Crescent 							   *
 * Released under the MIT license 				   *
 * http://opensource.org/licenses/mit-license.php  *
 * 19/02/16 v1.0 Initial Release                   *
 * 19/02/27 v1.1 Fix initialization stability      *
 * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "BT8XX.h"

uint16_t eve_cmd_fifo;


void EveInit(){

	EVE_CS_1();
	EVE_PD_1();
	HAL_Delay(80);
	EVE_PD_0();
	HAL_Delay(80);
	EVE_PD_1();
	HAL_Delay(80);

	EveWriteCmd(CLKEXT);
	EveWriteCmd(CLK36M);
	EveWriteCmd(ACTIVE);
	HAL_Delay(300);

}

uint8_t  EveReadChipID(){

	return EveReadData8(REG_ID);
}

void  EveCheckChipID(){
	uint32_t chipid;
	chipid=EveReadData32(CHIP_ID_REG);
	printf("ID: 0x%04x  0x%04x\n\r",(uint16_t)(chipid>>16), (uint16_t)(chipid&0xFFFF));

}

uint16_t EveCheckStatus(){

	return EveReadData16(REG_CPURESET);
}

void EveSetCPUFrq(){
	return EveWriteData32(REG_FREQUENCY,0x3938700);
}

void EveSetResolution(){

	EveWriteData16(REG_HCYCLE,	USR_HCYCLE);
	EveWriteData16(REG_HOFFSET,	USR_HOFFSET);
	EveWriteData16(REG_HSYNC0,	USR_HSYNC0);
	EveWriteData16(REG_HSYNC1,	USR_HSYNC1);
	EveWriteData16(REG_VCYCLE,	USR_VCYCLE);
	EveWriteData16(REG_VOFFSET,	USR_VOFFSET);
	EveWriteData16(REG_VSYNC0,	USR_VSYNC0);
	EveWriteData16(REG_VSYNC1,	USR_VSYNC1);
	EveWriteData8(REG_SWIZZLE,	USR_SWIZZLE);
	EveWriteData8(REG_PCLK_POL,	USR_PCLK_POL);
	EveWriteData16(REG_HSIZE,	USR_HSIZE);
	EveWriteData16(REG_VSIZE,	USR_VSIZE);
	EveWriteData8(REG_CSPREAD,	USR_CSPREAD);
	EveWriteData8(REG_DITHER,	USR_DITHER);


	EveWriteData16(REG_GPIOX_DIR,	0xFFFF);
	EveWriteData16(REG_GPIOX,	0xFFFF);

	//Clear Dispaly
	EveWriteData32(RAM_DL+0,	CLEAR_COLOR_RGB(255,255,255));
	EveWriteData32(RAM_DL+4,	CLEAR(1,1,1));
	EveWriteData32(RAM_DL+8,	DISPLAY());
	EveWriteData32(REG_CMD_WRITE,12);

	//Display swap
	EveWriteData8(REG_DLSWAP, DLSWAP_FRAME);


	//LCD visible
	EveWriteData8(REG_PCLK, USR_PCLK);
	HAL_Delay(800);
}



void EveDemo(){


	//Load Logo
	EveWriteData32(RAM_CMD+0,	CMD_LOGO);
	//EveWriteData32(RAM_CMD+4,0) ;
	EveWriteData32(REG_CMD_WRITE,4) ;
	HAL_Delay(1);
	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
	HAL_Delay(2000);

	//Display Hello
	EveWriteData32(RAM_DL+0,   CLEAR_COLOR_RGB(255,255,255));
	EveWriteData32(RAM_DL + 4, CLEAR(1, 1, 1)); // clear screen
	EveWriteData32(RAM_DL + 8, COLOR_RGB(1, 1, 1)); // change colour to red
	EveWriteData32(RAM_DL + 12, BEGIN(BITMAPS)); // start drawing bitmaps
	EveWriteData32(RAM_DL + 16, VERTEX2II(220, 110, 31, 'H')); // ascii H
	EveWriteData32(RAM_DL + 20, VERTEX2II(248, 110, 31, 'E')); // ascii E
	EveWriteData32(RAM_DL + 24, VERTEX2II(272, 110, 31, 'L')); // ascii L
	EveWriteData32(RAM_DL + 28, VERTEX2II(299, 110, 31, 'L')); // ascii L
	EveWriteData32(RAM_DL + 32, VERTEX2II(326, 110, 31, 'O')); // ascii O
	EveWriteData32(RAM_DL + 36, END());
	EveWriteData32(RAM_DL + 40, COLOR_RGB(255, 0, 0)); // change colour to red
	EveWriteData32(RAM_DL + 44, POINT_SIZE(320)); // set point size to 20 pixels in radius
	EveWriteData32(RAM_DL + 48, BEGIN(POINTS)); // start drawing points
	EveWriteData32(RAM_DL + 52, VERTEX2II(192, 133, 0, 0)); // red point
	EveWriteData32(RAM_DL + 56, END());
	EveWriteData32(RAM_DL + 60, DISPLAY()); // display the image
	EveWriteData8(REG_DLSWAP, DLSWAP_FRAME);
	HAL_Delay(3000);

}


void EveFlashTest(){

	uint8_t rdat[256];
	uint8_t wdat[256];

	EveFlashEraseAll();
//	EveFlashReadArray(0, 0, 256, rdat);
//	printf("Flash Read Data0: %d %d %d\n\r",rdat[0],rdat[1],rdat[2]);
	wdat[0]=1;
	wdat[1]=2;
	wdat[2]=3;
	EveFlashWrite(0, wdat, 256);
	printf("Flash Data Written\n\r");
	EveFlashReadArray(0, 0, 256, rdat);
	printf("Flash Read Data1: %d %d %d\n\r",rdat[0],rdat[1],rdat[2]);

}

void EveReadMemory (uint32_t addr, uint8_t *buffer, uint32_t len){

	uint8_t wdata[4];

	wdata[0] = addr >> 16;
	wdata[1] = (addr & 0xFF00) >> 8;
	wdata[2] = addr & 0xFF;
	wdata[3] = 0;

	EVE_CS_0();
	HAL_SPI_Transmit (&EVE_SPI_HANDLE, wdata, sizeof(wdata), HAL_MAX_DELAY);
	HAL_SPI_Receive (&EVE_SPI_HANDLE, (uint8_t*)buffer, len, HAL_MAX_DELAY);
	EVE_CS_1();
}

void EveFlashReadArray(uint32_t dest, uint32_t addr, uint32_t num, uint8_t *data){

	EveFlashRead(dest, addr, num);
	EveWaitCmdFifoEmpty();
	EveReadMemory(dest, data, num);
	//EveWaitCmdFifoEmpty();
}


uint8_t EveFlashReadState(){

	//BASIC:2, DETACHED:1, FULL:3, INIT:0
	return EveReadData8(REG_FLASH_STATUS);
}

void EveCheckFlashState(){

	uint8_t res=EveFlashReadState();
	if(res==0)		printf("Flash [INIT] State\n\r");
	else if(res==1) printf("Flash [Detached] State\n\r");
	else if(res==2) printf("Flash [Basic] State\n\r");
	else if(res==3) printf("Flash [Full] State\n\r");
}


void EveFlashEraseAll(){

	EveSendCmd(CMD_FLASHERASE);
	//EveWaitCmdFifoEmpty();

}


void EveUpdateCmdFifo(uint32_t count){

	eve_cmd_fifo  = (eve_cmd_fifo + count) & 4095;
	eve_cmd_fifo = (eve_cmd_fifo + 3) & 0xFFC;
	EveWriteData16(REG_CMD_WRITE,eve_cmd_fifo);
}


void EveWaitCmdFifoEmpty(){

	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
	eve_cmd_fifo =EveReadData16(REG_CMD_WRITE);

}

uint16_t EveGetCmdFifoSpace(){

	uint16_t fullness,val;

	fullness = (eve_cmd_fifo-EveReadData16(REG_CMD_READ))&4095;
	val = (4096 - 4) - fullness;

	return val;

}

void EveCheckCmdBuffer(uint32_t cnt){

	uint16_t space;
	do{
		space = EveGetCmdFifoSpace();
	}while(space < cnt);
}


void EveSendCmd (uint32_t cmd)
{
	EveCheckCmdBuffer(sizeof(cmd));
	EveWriteData32(RAM_CMD + eve_cmd_fifo,cmd);
	EveUpdateCmdFifo(sizeof(cmd));
}


void EveFlashWriteExecution(uint32_t dest, uint32_t num, uint8_t *data)
{
	uint32_t i, send_data32=0;

	EveSendCmd(CMD_FLASHWRITE);
	EveSendCmd(dest);
	EveSendCmd(num);

	for (i = 0; i < num; i=i+4){
	  send_data32 = *data++;
	  send_data32 |= (*data++) << 8;
	  send_data32 |= (*data++) << 16;
	  send_data32 |= (*data++) << 24;
	  EveSendCmd(send_data32);
	}


}

void EveClearCache(){

	EveSendCmd(CMD_CLEARCACHE);
	EveWaitCmdFifoEmpty();
}

void EveFlashWrite(uint32_t addr, uint8_t *data, uint32_t num){

	EveFlashWriteExecution( addr, num, data);
	EveWaitCmdFifoEmpty();
}


void EveFlashRead(uint32_t dest, uint32_t addr, uint32_t num){

	EveSendCmd(CMD_FLASHREAD);
	EveSendCmd(dest);
	EveSendCmd(addr);
	EveSendCmd(num);

}


void EveWriteCmd(uint8_t cmd){

	uint8_t sdata[3];
	uint8_t rdata[3];
	sdata[0]=(cmd);
	sdata[1]=0;
	sdata[2]=0;

	EVE_CS_0();
	HAL_SPI_TransmitReceive(&EVE_SPI_HANDLE,(uint8_t*)&sdata,(uint8_t*)&rdata,3,1000);
	EVE_CS_1();
}

uint8_t EveReadData8(uint32_t addr){

	uint8_t data = 0;
	uint8_t sdata[5];
	uint8_t rdata[5];
	sdata[0]=(uint8_t)((addr >> 16) | MEM_READ);
	sdata[1]=(uint8_t)(addr >> 8);
	sdata[2]=(uint8_t)(addr);
	sdata[3]=(uint8_t)(0);//dummy byte
	sdata[4]=(uint8_t)(0);

	EVE_CS_0();
	HAL_SPI_TransmitReceive(&EVE_SPI_HANDLE,(uint8_t*)&sdata,(uint8_t*)&rdata,5,1000);
	EVE_CS_1();
	data=rdata[4];

	return data;

}

uint16_t EveReadData16(uint32_t addr){

	uint16_t data = 0;
	uint8_t sdata[6];
	uint8_t rdata[6];
	uint16_t res[2];
	sdata[0]=(uint8_t)((addr >> 16) | MEM_READ);
	sdata[1]=(uint8_t)(addr >> 8);
	sdata[2]=(uint8_t)(addr);
	sdata[3]=(uint8_t)(0);//dummy byte
	sdata[4]=(uint8_t)(0);
	sdata[5]=(uint8_t)(0);

	EVE_CS_0();
	HAL_SPI_TransmitReceive(&EVE_SPI_HANDLE,(uint8_t*)&sdata,(uint8_t*)&rdata,6,1000);
	EVE_CS_1();
	res[0]=(uint16_t)rdata[4];
	res[1]=(uint16_t)rdata[5];

	data=res[0]+(res[1]<<8);

	return data;

}



uint32_t EveReadData32(uint32_t addr){

	uint32_t data = 0;
	uint8_t sdata[8];
	uint8_t rdata[8];
	uint32_t res[4];
	sdata[0]=(uint8_t)((addr >> 16) | MEM_READ);
	sdata[1]=(uint8_t)(addr >> 8);
	sdata[2]=(uint8_t)(addr);
	sdata[3]=(uint8_t)(0);//dummy byte
	sdata[4]=(uint8_t)(0);
	sdata[5]=(uint8_t)(0);
	sdata[6]=(uint8_t)(0);
	sdata[7]=(uint8_t)(0);

	EVE_CS_0();
	HAL_SPI_TransmitReceive(&EVE_SPI_HANDLE,(uint8_t*)&sdata,(uint8_t*)&rdata,8,1000);
	EVE_CS_1();
	res[0]=(uint32_t)rdata[4];
	res[1]=(uint32_t)rdata[5];
	res[2]=(uint32_t)rdata[6];
	res[3]=(uint32_t)rdata[7];

	data=res[0]+(res[1]<<8)+(res[2]<<16)+(res[3]<<24);

	return data;

}

void EveWriteData8(uint32_t addr, uint8_t data8)
{
	uint8_t sdata[4];
	uint8_t rdata[4];
	sdata[0]=((uint8_t)(addr >> 16) | MEM_WRITE);
	sdata[1]=(uint8_t)(addr >> 8);
	sdata[2]=(uint8_t)(addr >> 0);
	sdata[3]=(uint8_t)(data8 >> 0);

	EVE_CS_0();
	HAL_SPI_TransmitReceive(&EVE_SPI_HANDLE,(uint8_t*)&sdata,(uint8_t*)&rdata,4,1000);
	EVE_CS_1();
}

void EveWriteData16(uint32_t addr, uint16_t data16)
{
	uint8_t sdata[7];
	uint8_t rdata[7];
	sdata[0]=((uint8_t)(addr >> 16) | MEM_WRITE);
	sdata[1]=(uint8_t)(addr >> 8);
	sdata[2]=(uint8_t)(addr >> 0);
	sdata[3]=(uint8_t)(data16 >> 0);
	sdata[4]=(uint8_t)(data16 >> 8);

	EVE_CS_0();
	HAL_SPI_TransmitReceive(&EVE_SPI_HANDLE,(uint8_t*)&sdata,(uint8_t*)&rdata,5,1000);
	EVE_CS_1();

}


void EveWriteData32(uint32_t addr, uint32_t data32)
{

	uint8_t sdata[7];
	uint8_t rdata[7];
	sdata[0]=((uint8_t)(addr >> 16) | MEM_WRITE);
	sdata[1]=(uint8_t)(addr >> 8);
	sdata[2]=(uint8_t)(addr);
	sdata[3]=(uint8_t)(data32 );
	sdata[4]=(uint8_t)(data32 >> 8);
	sdata[5]=(uint8_t)(data32 >>16);
	sdata[6]=(uint8_t)(data32 >>24);

	EVE_CS_0();
	HAL_SPI_TransmitReceive(&EVE_SPI_HANDLE,(uint8_t*)&sdata,(uint8_t*)&rdata,7,1000);
	EVE_CS_1();

}
