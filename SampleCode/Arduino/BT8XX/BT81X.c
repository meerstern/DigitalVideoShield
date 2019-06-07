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

#include "BT81X.h"

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


extern void EveWriteCmd(unsigned char ftCommand);
extern unsigned char EveReadData8(unsigned long ftAddress);
extern unsigned int EveReadData16(unsigned long ftAddress);
extern unsigned long EveReadData32(unsigned long ftAddress);
extern void EveWriteData8(unsigned long ftAddress, unsigned char ftData8);
extern void EveWriteData16(unsigned long ftAddress, unsigned int ftData16);
extern void EveWriteData32(unsigned long ftAddress, unsigned long ftData32);
extern void EveWriteData32BufInc(unsigned long ftAddress, unsigned long ftData32);
extern void EveLoadJpgFile(char *filename, uint32_t addr, uint32_t width, uint32_t hight);
extern void EveReadMemory (uint32_t addr, uint8_t *buffer, uint32_t len);
extern void usrdelay(uint16_t t);

uint32_t buf_size;
uint16_t cmd_fifo;

/* Eve IO Port Def  */
const int8_t  sdcsPin = 5;   
const int8_t  irqPin  = 4;        // Interrupt 
const int8_t  pwrPin  = 3;        // PD_N
const int8_t  csPin   = 7;

void EveDemo(){
  
	printf("Demo Start\n\r");
	//Disp Logo
  EveWriteDataBufReset();
  EveWriteData32BufInc(RAM_CMD, CMD_LOGO);
  EveWriteData16BufWrite();
  usrdelay(1);
  while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
  usrdelay(500);
  
  //Display Crescent
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
  EveWaitCmdFifoEmpty();
	printf("Demo End\n\r");
	usrdelay(1000);
 
	
}

void WaitCmdFifoEmpty(){

	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
	cmd_fifo =EveReadData16(REG_CMD_WRITE);

}

void EveDemo2(){

  printf("Demo2 Start\n\r");
	//Load Logo
  EveWriteDataBufReset();
	EveWriteData32BufInc(RAM_CMD,	CMD_LOGO);
	EveWriteData16BufWrite();
	usrdelay(1);
	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
	usrdelay(500);

	//EveRecoverCoProcessor();
  EveWriteDataBufReset();
	//EveWaitCmdFifoEmpty();  
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
  printf("Demo2 End\n\r");
  usrdelay(2000);
	//while(1);
}

// Bitmap demo
void EveDemo3(){

  printf("Demo3 Start\n\r");
	//Load Logo
	EveWriteDataBufReset();
	EveWriteData32BufInc(RAM_CMD,	CMD_LOGO);
	EveWriteData32BufWrite();
	usrdelay(1);
	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
  usrdelay(500);
  
	EveLoadJpgFile("logo.jpg", RAM_G, 800, 480);
  printf("Demo3 End\n\r");
  usrdelay(2000);

}


void EveWriteClock(int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms)
{
	EveSendCmd(CMD_CLOCK);
	EveSendCmd((((uint32_t)y<<16)|(x & 0xffff)));
	EveSendCmd((((uint32_t)options<<16)|(r&0xffff)));
	EveSendCmd((((uint32_t)m<<16)|(h&0xffff)));
	EveSendCmd((((uint32_t)ms<<16)|(s&0xffff)));
	//EveWaitCmdFifoEmpty();
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
	//EveWaitCmdFifoEmpty();

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
	usrdelay(10);
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
  if(res==0)    printf("Flash [INIT] State\n\r");
  else if(res==1) printf("Flash [Detached] State\n\r");
  else if(res==2) printf("Flash [Basic] State\n\r");
  else if(res==3) printf("Flash [Full] State\n\r");
}


void EveFlashEraseAll(){

	EveSendCmd(CMD_FLASHERASE);
	//EveWaitCmdFifoEmpty();

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
	EveWriteData32(RAM_CMD + cmd_fifo,CMD_FLASHFAST);
	res = EveReadData32(RAM_CMD + cmd_fifo+4);
	EveWriteData16(REG_CMD_WRITE,RAM_CMD+cmd_fifo+8);

	if(res!=0){
		if(res==0xE001)		strcpy(msg,"flash is not supported");
		else if(res==0xE002)strcpy(msg,"no header detected in sector 0 – is flash blank?");
		else if(res==0xE003)strcpy(msg,"sector 0 data failed integrity check");
		else if(res==0xE004)strcpy(msg,"device/blob mismatch – was correct blob loaded?");
		else if(res==0xE004)strcpy(msg,"failed full-speed test – check board wiring");
		else				strcpy(msg,"something wrong");
		printf("Flash Fast Mode Err: 0x%lX, %s \n\r",res,msg);
	}
	//EveWaitCmdFifoEmpty();

	uint8_t status = EveFlashReadState();
	if (status != 3)printf("Flash is NOT Fast Mode (Not Full Mode)\n\r");
}




void EveUpdateCmdFifo(uint32_t count){

	cmd_fifo  = (cmd_fifo + count) & 4095;
	cmd_fifo = (cmd_fifo + 3) & 0xFFC;
	EveWriteData16(REG_CMD_WRITE,cmd_fifo);
}


void EveWaitCmdFifoEmpty(){

	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
	cmd_fifo =EveReadData16(REG_CMD_WRITE);

}

uint16_t EveGetCmdFifoSpace(){

	uint16_t fullness,val;

	fullness = (cmd_fifo-EveReadData16(REG_CMD_READ))&4095;
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
	EveWriteData32(RAM_CMD + cmd_fifo,cmd);
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
	  send_data32 |= (uint32_t)(*data++) << 8;
	  send_data32 |= (uint32_t)(*data++) << 16;
	  send_data32 |= (uint32_t)(*data++) << 24;
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


void EveWriteData16BufWrite(){
	EveWriteData16(REG_CMD_WRITE, buf_size);
	//buf_size=0;
}

void EveWriteData32BufWrite(){
  EveWriteData32(REG_CMD_WRITE, buf_size);
  //buf_size=0;
}
void EveWriteDataBufReset(){
	buf_size=0;
}
