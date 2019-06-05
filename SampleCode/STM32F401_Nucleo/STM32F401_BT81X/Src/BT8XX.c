/* * * * * * * * * * * * * * * * * * * * * * * * * *
 * FTDI EVE Firmware for BT81X  				   *
 * Copyright (c) 2019  							   *
 * K.Watanabe,Crescent 							   *
 * Released under the MIT license 				   *
 * http://opensource.org/licenses/mit-license.php  *
 * 19/02/16 v1.0 Initial Release                   *
 * 19/02/27 v1.1 Fix initialization stability      *
 * 19/04/28 v1.2 Add Co-Processor command          *
 * 19/06/05 v1.3 Add Jpeg Load command		       *
 * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "BT8XX.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"

#define RED		0xff0000UL
#define ORANGE	0xffa500UL
#define GREEN	0x00ff00UL
#define BLUE	0x0000ffUL
#define BLUE_1	0x5dade2L
#define YELLOW	0xffff00UL
#define PINK	0xff00ffUL
#define PURPLE	0x800080UL
#define WHITE	0xffffffUL
#define BLACK	0x000000UL

#define IMAGE_NUM       4
#define LINE_LIMIT      35     // max 33
#define COPY_SIZE 		64UL

typedef struct {
  char     filename[16]; // Filenames 8.3 ex. XYZ.bmp
  uint16_t sizeX;
  uint16_t sizeY;
  uint32_t format;
  uint32_t flashaddress;
}FileFormat;

FileFormat flashfiles[IMAGE_NUM];        //FlashFiles in attached flash


uint16_t eve_cmd_fifo;
uint32_t eve_buf_size;

#ifdef ENABLE_SD
	FATFS fs;
	FATFS *fsp = &fs;
	FIL fp;
	FRESULT fres;
	bool existsSDcard = false;
#endif

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


void EveWriteDataBufReset(){
	eve_buf_size=0;
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

	//EveCmdSetRotate(EVE_ROTATE_180deg);

	//Clear Dispaly
	EveWriteDataBufReset();
	EveWriteData32BufInc(RAM_DL,	CLEAR_COLOR_RGB(255,255,255));
	EveWriteData32BufInc(RAM_DL,	CLEAR(1,1,1));
	EveWriteData32BufInc(RAM_DL,	DISPLAY());
	EveWriteData32BufWrite();

	//Display swap
	EveWriteData8(REG_DLSWAP, DLSWAP_FRAME);




	//LCD visible
	EveWriteData8(REG_PCLK, USR_PCLK);
	HAL_Delay(100);
}

//Simple Demo
void EveDemo(){


	//Load Logo
	EveWriteData32BufInc(RAM_CMD,	CMD_LOGO);
	EveWriteData32BufWrite();
	HAL_Delay(1);
	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
	HAL_Delay(2000);

	//Display Hello
	EveWriteDataBufReset();
	EveWriteData32BufInc(RAM_DL, CLEAR_COLOR_RGB(255,255,255));
	EveWriteData32BufInc(RAM_DL, CLEAR(1, 1, 1));
	EveWriteData32BufInc(RAM_DL, COLOR_RGB(1, 1, 1));
	EveWriteData32BufInc(RAM_DL, BEGIN(BITMAPS)); // start drawing bitmaps
	EveWriteData32BufInc(RAM_DL, VERTEX2II(220, 110, 31, 'C')); // ascii C
	EveWriteData32BufInc(RAM_DL, VERTEX2II(248, 110, 31, 'r')); // ascii r
	EveWriteData32BufInc(RAM_DL, VERTEX2II(264, 110, 31, 'e')); // ascii e
	EveWriteData32BufInc(RAM_DL, VERTEX2II(290, 110, 31, 's')); // ascii s
	EveWriteData32BufInc(RAM_DL, VERTEX2II(315, 110, 31, 'c')); // ascii c
	EveWriteData32BufInc(RAM_DL, VERTEX2II(338, 110, 31, 'e')); // ascii e
	EveWriteData32BufInc(RAM_DL, VERTEX2II(360, 110, 31, 'n')); // ascii n
	EveWriteData32BufInc(RAM_DL, VERTEX2II(384, 110, 31, 't')); // ascii t
	EveWriteData32BufInc(RAM_DL, END());
	EveWriteData32BufInc(RAM_DL, COLOR_RGB(255, 255, 0));
	EveWriteData32BufInc(RAM_DL, POINT_SIZE(320));
	EveWriteData32BufInc(RAM_DL, BEGIN(POINTS));
	EveWriteData32BufInc(RAM_DL, VERTEX2II(192, 133, 0, 0));
	EveWriteData32BufInc(RAM_DL, COLOR_RGB(255, 255, 255));
	EveWriteData32BufInc(RAM_DL, POINT_SIZE(320));
	EveWriteData32BufInc(RAM_DL, BEGIN(POINTS));
	EveWriteData32BufInc(RAM_DL, VERTEX2II(200, 125, 0, 0));
	EveWriteData32BufInc(RAM_DL, END());
	EveWriteData32BufInc(RAM_DL, DISPLAY());
	EveWriteData8(REG_DLSWAP, DLSWAP_FRAME);
	HAL_Delay(3000);

}

// Co-Processor Demo
void EveDemo2(){

	//Load Logo
	EveWriteDataBufReset();
	EveWriteData32BufInc(RAM_CMD,	CMD_LOGO);
	EveWriteData32BufWrite();
	HAL_Delay(1);
	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
	HAL_Delay(200);

	//EveRecoverCoProcessor();
	EveWaitCmdFifoEmpty();
	EveSendCmd(CMD_DLSTART);
	EveSendCmd(CLEAR_COLOR_RGB(255,255,255));
	EveSendCmd( CLEAR(1, 1, 1));
	EveSendCmd(COLOR_RGB(220, 1, 240));
	//Number
	EveWriteNumberData(220,200,EVE_FONT_B_size6,0,120);
	EveSendCmd(COLOR_RGB(180, 180, 240));
	//Clock
	EveWriteClock(120,120,100,0,10,10,20,3);
	EveSendCmd(COLOR_RGB(20, 50, 140));
	//String
	EveWriteStringData(200,350,EVE_FONT_B_size6,0,"Digital Video Shield");
	EveSendCmd(COLOR_RGB(20, 100, 40));
	//String
	EveWriteStringData(200,400,EVE_FONT_B_size6,0,"by Crescent");
	EveSendCmd(COLOR_RGB(220, 200, 240));
	//Gauge
	EveWriteGauge(400,120,100,0,80,0,10,100);
	//Toggle
	EveWriteToggle(550,120,50,EVE_FONT_B_size4,0,1,"ON");
	//Button
	EveWriteButton(550,50,80,40,EVE_FONT_B_size4,0,"ON");
	//Progress
	EveWriteProgress(550,200,50,30,0,20,100);
	EveSendCmd(COLOR_RGB(1, 1, 1));
	//Spinner
	EveWriteSpinner(700, 150, 0, 0);
	//Display Data
	EveSendCmd(DISPLAY());
	EveWriteData8(REG_DLSWAP, DLSWAP_FRAME);
	EveWaitCmdFifoEmpty();
	while(1);
}


// Bitmap demo
void EveDemo3(){

	//Load Logo
	EveWriteDataBufReset();
	EveWriteData32BufInc(RAM_CMD,	CMD_LOGO);
	EveWriteData32BufWrite();
	HAL_Delay(1);
	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));

	printf("Demo Start \n\r");
	EveLoadJpgFile("logo.jpg", RAM_G, 800, 480);
	printf("Demo OK \n\r");

}

void EveWriteClock(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms)
{
	EveSendCmd(CMD_CLOCK);
	EveSendCmd((((uint32_t)y<<16)|(x & 0xffff)));
	EveSendCmd((((uint32_t)options<<16)|(r&0xffff)));
	EveSendCmd((((uint32_t)m<<16)|(h&0xffff)));
	EveSendCmd((((uint32_t)ms<<16)|(s&0xffff)));
	EveWaitCmdFifoEmpty();
}


void EveWriteButton(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* str){

	EveSendCmd(CMD_BUTTON);
	EveSendCmd((((uint32_t)y<<16)|(x & 0xffff)));
	EveSendCmd((((uint32_t)h<<16)|(w&0xffff)));
	EveSendCmd((((uint32_t)options<<16)|(font&0xffff)));
	EveAddStringData(str);

}

void EveWriteGauge(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range){

	EveSendCmd(CMD_GAUGE);
	EveSendCmd((((uint32_t)y<<16)|(x & 0xffff)));
	EveSendCmd((((uint32_t)options<<16)|(r&0xffff)));
	EveSendCmd((((uint32_t)minor<<16)|(major&0xffff)));
	EveSendCmd((((uint32_t)range<<16)|(val&0xffff)));
}


void EveWriteToggle(int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char *str){

	EveSendCmd(CMD_TOGGLE);
	EveSendCmd((((uint32_t)y<<16)|(x & 0xffff)));
	EveSendCmd((((uint32_t)font<<16)|(w&0xffff)));
	EveSendCmd((((uint32_t)state<<16)|options));
	EveAddStringData(str);
}

void EveAddStringData(const char *str){

	uint16_t dataptr, i, strptr;
	uint16_t length = strlen(str);

	if(!length)return;

	uint32_t* data = (uint32_t*) calloc((length / 4) + 1, sizeof(uint32_t));

	strptr = 0;
	for(dataptr=0; dataptr<(length/4); ++dataptr, strptr=strptr+4){
	  data[dataptr] = (uint32_t)str[strptr+3]<<24 | (uint32_t)str[strptr+2]<<16 | (uint32_t)str[strptr+1]<<8 | (uint32_t)str[strptr];
	}

	for(i=0; i<(length%4); ++i, ++strptr){
	  data[dataptr] |= (uint32_t)str[strptr] << (i*8);
	}

	for(i = 0; i <= length/4; i++){
	  EveSendCmd(data[i]);
	}
	free(data);
}



void EveWriteStringData(uint16_t x, uint16_t y, uint16_t font, uint16_t options, const char* str){

  uint16_t dataptr, i, strptr;
  uint16_t length = strlen(str);

  if(!length)return;

  uint32_t* data = (uint32_t*) calloc((length / 4) + 1, sizeof(uint32_t));

  strptr = 0;
  for(dataptr=0; dataptr<(length/4); ++dataptr, strptr=strptr+4){
	  data[dataptr] = (uint32_t)str[strptr+3]<<24 | (uint32_t)str[strptr+2]<<16 | (uint32_t)str[strptr+1]<<8 | (uint32_t)str[strptr];
  }

  for(i=0; i<(length%4); ++i, ++strptr){
	  data[dataptr] |= (uint32_t)str[strptr] << (i*8);
  }

  EveSendCmd(CMD_TEXT);
  EveSendCmd(((uint32_t)y << 16) | x );
  EveSendCmd(((uint32_t)options << 16) | font );

  for(i = 0; i <= length/4; i++){
	  EveSendCmd(data[i]);
  }

  free(data);
}

void EveWriteNumberData(int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n){

	EveSendCmd(CMD_NUMBER);
	EveSendCmd((((uint32_t)y<<16)|(x & 0xffff)));
	EveSendCmd((((uint32_t)options<<16)|(font&0xffff)));
	EveSendCmd(n);
	EveWaitCmdFifoEmpty();

}

void EveWriteSpinner(int16_t x, int16_t y, uint16_t style, uint16_t scale){

	EveSendCmd(CMD_SPINNER);
	EveSendCmd((((uint32_t)y<<16)|(x & 0xffff)));
	EveSendCmd((((uint32_t)scale<<16)|(style&0xffff)));

}

void EveWriteKeys(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char *str){

	EveSendCmd(CMD_KEYS);
	EveSendCmd(((uint32_t)y<<16)|(x & 0xffff));
	EveSendCmd(((uint32_t)h<<16)|(w&0xffff));
	EveSendCmd(((uint32_t)options<<16)|(font&0xffff));
	EveAddStringData(str);

}

void EveWriteDial(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t val){

	EveSendCmd(CMD_DIAL);
	EveSendCmd((((uint32_t)y<<16)|(x & 0xffff)));
	EveSendCmd((((uint32_t)options<<16)|(r&0xffff)));
	EveSendCmd(val);

}

void EveWriteProgress(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range){

	EveSendCmd(CMD_PROGRESS);
	EveSendCmd(((uint32_t)y<<16)|(x & 0xffff));
	EveSendCmd(((uint32_t)h<<16)|(w&0xffff));
	EveSendCmd(((uint32_t)val<<16)|(options&0xffff));
	EveSendCmd(range);

}

void EveWriteSync(){

	EveSendCmd(CMD_SYNC);
	EveWaitCmdFifoEmpty();
}

void EveCmdRomFont(uint32_t font, uint32_t slot){

  	EveSendCmd(CMD_ROMFONT);
	EveSendCmd(font);
	EveSendCmd(slot);
	EveWaitCmdFifoEmpty();
}

void EveResetRomFont(){

  	EveSendCmd(CMD_RESETFONTS);
	EveWaitCmdFifoEmpty();
}

void EveCmdSetRotate(uint32_t r){

  	EveSendCmd(CMD_SETROTATE);
	EveSendCmd(r);
	EveWaitCmdFifoEmpty();
}

void EveCmdSetBitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height){

	EveSendCmd(CMD_SETBITMAP);
	EveSendCmd(addr);
	EveSendCmd(((uint32_t)width << 16) | fmt);
	EveSendCmd((uint32_t)height);
}


void EveLoadJpgFile(char *filename, uint32_t addr, uint32_t width, uint32_t hight){

	uint32_t 	filesize=0;
	uint32_t 	filesectors=0;
	uint32_t 	sectorsspace=0;

#ifdef ENABLE_SD

	 if(f_mount(fsp, "", 0) != FR_OK)
		  printf("SD Mount ERR\n\r");

	fres = f_open(&fp, filename, FA_READ | FA_OPEN_EXISTING);
	if (fres != FR_OK){
		if(fres == FR_NO_FILE)
			printf("Could not find the file \"%s\"\r\n", filename);
		else
			printf("fres error, %d\r\n", fres);
		f_mount(NULL, "", 0);
		return;
	}
	else{

		filesize = f_size(&fp);
		filesectors = filesize/0x1000;

		if(filesize % 0x1000)   filesectors++;
		sectorsspace= ( 0x100000 - (addr - RAM_G)) / 0x1000;


		if ( filesectors > sectorsspace ){
			printf("Data size is over!\n\r");
			return;
		}

		printf("FileName:\"%s\" Size:%ld \r\n", filename,filesize);
	}


	printf("Read SD & RAM Write Start\n\r");

	uint16_t m=0;
	EveWaitCmdFifoEmpty();
	EveSendCmd(CMD_LOADIMAGE);
	EveSendCmd(addr);
	EveSendCmd(0);//Options

	while(f_eof(&fp) == 0){
		char str_readbuf[4] = {'\0'};
		uint32_t readbuf[4];

		uint32_t dat=0;
		uint16_t br;

		f_read(&fp,str_readbuf,sizeof(str_readbuf),(unsigned int*)&br);
		if(br==0)break;

		readbuf[0]=(uint32_t)str_readbuf[0];
		readbuf[1]=(uint32_t)str_readbuf[1];
		readbuf[2]=(uint32_t)str_readbuf[2];
		readbuf[3]=(uint32_t)str_readbuf[3];

		dat =	 (uint32_t)(readbuf[0]<<0)
				+(uint32_t)(readbuf[1]<<8)
				+(uint32_t)(readbuf[2]<<16)
				+(uint32_t)(readbuf[3]<<24);
		EveSendCmd(dat);

		m++;
	}

	printf("Read SD & RAM Write End\n\r");


	if(f_close(&fp) != FR_OK)
		  printf("SD File Close ERR\n\r");
	if(f_mount(NULL, "", 1) != FR_OK)
		  printf("SD Unmount ERR\n\r");

#endif

	EveWaitCmdFifoEmpty();
	EveSendCmd(CMD_DLSTART);
	EveSendCmd(CLEAR(1, 1, 1));
	EveCmdSetBitmap(RAM_G, RGB565, width, hight);
	EveSendCmd(BEGIN(BITMAPS));
	EveSendCmd(VERTEX2II(0, 0, 0, 0));//Position, Angle
	EveSendCmd(END());
	EveSendCmd(DISPLAY());
	EveWriteData8(REG_DLSWAP, DLSWAP_FRAME);
	EveWaitCmdFifoEmpty();



}


void EveRecoverCoProcessor(){

	uint32_t res;
	res=EveReadData32(REG_COPRO_PATCH_PTR);
	EveWriteData32(REG_CPURESET,1);
	EveWriteData32(REG_CMD_READ,0);
	EveWriteData32(REG_CMD_WRITE,0);
	EveWriteData32(REG_CMD_DL,0);
	EveWriteData32(REG_CPURESET,0);
	EveWriteData32(REG_COPRO_PATCH_PTR,res);
	EveWriteData8(REG_PCLK, USR_PCLK);
	HAL_Delay(10);
}


void EveFlashTest(){

	uint8_t rdat[256];
	uint8_t wdat[256];

	EveFlashEraseAll();
	EveFlashReadArray(0, 0, 256, rdat);
	printf("Flash Read Data0: %d %d %d\n\r",rdat[0],rdat[1],rdat[2]);
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
	printf("Flash All Erased!\n\r");

}

void EveFlashAttach(){

	EveSendCmd(CMD_FLASHATTACH);
	EveWaitCmdFifoEmpty();
	uint8_t status = EveFlashReadState();
	if (status != 2)printf("Flash is NOT attached\n\r");

}

void EveFlashDetach(){

	EveSendCmd(CMD_FLASHDETACH);
	EveWaitCmdFifoEmpty();

	uint8_t status = EveFlashReadState();
	if (status != 1)printf("Flash is NOT detached\n\r");

}

void EveFlashFastMode(){

	uint32_t res;
	char msg[60];
	//EveWaitCmdFifoEmpty();
	EveCheckCmdBuffer(sizeof(uint32_t));
	EveWriteData32(RAM_CMD + eve_cmd_fifo,CMD_FLASHFAST);
	res = EveReadData32(RAM_CMD + eve_cmd_fifo+4);
	EveWriteData16(REG_CMD_WRITE,RAM_CMD+eve_cmd_fifo+8);

	if(res!=0){
		if(res==0xE001)		strcpy(msg,"flash is not supported");
		else if(res==0xE002)strcpy(msg,"no header detected in sector 0 – is flash blank?");
		else if(res==0xE003)strcpy(msg,"sector 0 data failed integrity check");
		else if(res==0xE004)strcpy(msg,"device/blob mismatch – was correct blob loaded?");
		else if(res==0xE004)strcpy(msg,"failed full-speed test – check board wiring");
		else				strcpy(msg,"something wrong");
		printf("Flash Fast Mode Err: 0x%lX, %s \n\r",res,msg);
	}	//EveWaitCmdFifoEmpty();

	uint8_t status = EveFlashReadState();
	if (status != 3)printf("Flash is NOT Fast Mode (Not Full Mode)\n\r");
}



void EveUpdateCmdFifo(uint32_t count){

	eve_cmd_fifo  = (eve_cmd_fifo + count) & 4095;
	eve_cmd_fifo = (eve_cmd_fifo + 3) & 0xFFC;
	EveWriteData16(REG_CMD_WRITE,eve_cmd_fifo);
	//EveWriteData16(REG_CMDB_WRITE,eve_cmd_fifo);

}


void EveWaitCmdFifoEmpty(){

	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
	eve_cmd_fifo =EveReadData16(REG_CMD_WRITE);

}

uint16_t EveGetCmdFifoSpace(){

	uint16_t val;
	val=EveReadData16(REG_CMDB_SPACE) & 0xFFFF;
	//printf("Space:%d \n\r",val);
	return val;

}

void EveCheckCmdBuffer(uint32_t cnt){

	uint16_t space;
	do{
		space = EveGetCmdFifoSpace();
	}while(space < cnt);
}


void EveSendCmd (uint32_t cmd){

	EveCheckCmdBuffer(sizeof(cmd));
	EveWriteData32(RAM_CMD + eve_cmd_fifo,cmd);
	EveUpdateCmdFifo(sizeof(cmd));
}

void EveRecieveRes (uint32_t *res){

	EveCheckCmdBuffer(sizeof(uint32_t));
	res =(uint32_t*)EveReadData32(RAM_CMD + eve_cmd_fifo);
	EveUpdateCmdFifo(sizeof(uint32_t));
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

uint32_t EveWriteBlockRAM(uint32_t addr, const uint8_t *buff, uint32_t count){

  uint8_t index;
  uint32_t write_addr = addr;

  for (index = 0; index < count; index++){
    EveWriteData8(write_addr++, buff[index]);
  }
  return (write_addr);
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

void EveWriteData32BufInc(uint32_t addr, uint32_t data32)
{

	addr=addr+eve_buf_size;
	EveWriteData32( addr,  data32);
	eve_buf_size=eve_buf_size+4;

}

void EveWriteData16BufInc(uint32_t addr, uint32_t data32){

	addr=addr+eve_buf_size;
	EveWriteData32( addr,  data32);
	eve_buf_size=eve_buf_size+2;

}

void EveWriteData8BufInc(uint32_t addr, uint32_t data32){

	addr=addr+eve_buf_size;
	EveWriteData32( addr,  data32);
	eve_buf_size=eve_buf_size+1;

}

void EveWriteData32BufWrite()
{

	EveWriteData32( REG_CMD_WRITE,  eve_buf_size);

}

void EveWriteData16BufWrite(){

	EveWriteData16( REG_CMD_WRITE,  eve_buf_size);

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


