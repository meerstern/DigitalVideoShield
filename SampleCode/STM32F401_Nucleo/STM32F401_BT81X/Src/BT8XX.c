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
	EveWriteData8(REG_PCLK, 2);
	HAL_Delay(300);
}



void EveDemo(){


	//Load Logo
	EveWriteData32(RAM_CMD+0,	CMD_LOGO);
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
