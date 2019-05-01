/* * * * * * * * * * * * * * * * * * * * * * * * * *
 * FTDI EVE Firmware for BT81X                     *
 * Copyright (c) 2019                              *
 * K.Watanabe,Crescent                             *
 * Released under the MIT license                  *
 * http://opensource.org/licenses/mit-license.php  *
 * 19/02/16 v1.0 Initial Release                   *
 * 19/02/27 v1.1 Fix initialization stability      *
 * 19/04/28 v1.2 Add Co-Processor command          *
 * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <Arduino.h>    
#include <SPI.h>       
#include "BT81X.h"

uint32_t buf_size;
uint16_t cmd_fifo;

void setup() 
{
    unsigned int cnt;
	
    Serial.begin(9600);         
    SPI.begin();          					// Initialize SPI
	  Serial.println("Initialize Start!! \r\n");
    SPI.setBitOrder(MSBFIRST);      		// Send data most significant bit first
    SPI.setDataMode(SPI_MODE0);     		// Clock idle zero, clock data into BT81X on rising edge
    SPI.setClockDivider(SPI_CLOCK_DIV4);    // Set rate at 4MHz (16MHz OSC / 4)

	
    pinMode(irqPin, INPUT_PULLUP);   		// BT81X interrupt output (not used in this example)
    pinMode(pwrPin, OUTPUT);     			// BT81X Power Down (reset) input
    pinMode(csPin, OUTPUT);      			// BT81X SPI bus CS# input


    digitalWrite(csPin, HIGH);   			// Set CS# high to start - SPI inactive
    digitalWrite(pwrPin, HIGH);    			// Set PD# high to start
    delay(80);          					// Wait a few MS before waking the BT81X
    digitalWrite(pwrPin, LOW);    			// Set PD# high to start
    delay(80);  
    digitalWrite(pwrPin, HIGH);    			// Set PD# high to start
	  delay(80); 

	
  	EveWriteCmd(CLKEXT);    
  	EveWriteCmd(CLK36M);      
   	EveWriteCmd(ACTIVE);     
  	delay(300);     

	
  	while(CHIP_ID_RESET!=EveReadData8(REG_ID)){
  		cnt++;
  		if(cnt>100){
        Serial.print("EVE NOT Found!! :");
        Serial.println(EveReadData8(REG_ID));
  			delay(100);
  			break;
  		}
  	} 
    if(cnt<100)Serial.println("EVE Found!!");

    
   
  
  	cnt=0;
  	while(0x0!=EveReadData16(REG_CPURESET)){
  		cnt++;
  		if(cnt>100){
  			Serial.print("EVE NOT Active!! :");
  			Serial.println(EveReadData16(REG_CPURESET));
  			delay(200);
  		}
  	}
	  if(cnt<100)Serial.println("EVE Active!!");

    EveWriteData32(REG_FREQUENCY,0x3938700);   // Set CPU Frequency
    
  	// Set Resolution
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
  	EveWriteData16(REG_HSIZE,		USR_HSIZE);
  	EveWriteData16(REG_VSIZE,		USR_VSIZE);
  	EveWriteData8(REG_CSPREAD,	USR_CSPREAD);
  	EveWriteData8(REG_DITHER,		USR_DITHER);

    EveWriteData16(REG_GPIOX_DIR, 0xFFFF);
    EveWriteData16(REG_GPIOX, 0xFFFF);
  	
  	//Clear Dispaly
    EveWriteData32BufInc(RAM_DL,  CLEAR_COLOR_RGB(255,255,255));
    EveWriteData32BufInc(RAM_DL,  CLEAR(1,1,1));
    EveWriteData32BufInc(RAM_DL,  DISPLAY());
    EveWriteData32BufWrite();

    //Display swap
    EveWriteData8(REG_DLSWAP, DLSWAP_FRAME);


    //LCD visible
    EveWriteData8(REG_PCLK, USR_PCLK);
    delay(300);
  
    Serial.println("Hello");
}


void loop() 
{

  
	demo();
	while(1);

}

void demo(){
	
	//Disp Logo
  EveWriteData32BufInc(RAM_CMD, CMD_LOGO);
  EveWriteData16BufWrite();
  delay(1);
  while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
  delay(2000);
  
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
	delay(3000);
	
}

void WaitCmdFifoEmpty(){

	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
	cmd_fifo =EveReadData16(REG_CMD_WRITE);

}

void demo2(){

	//Load Logo
	EveWriteData32BufInc(RAM_CMD,	CMD_LOGO);
	EveWriteData16BufWrite();
	delay(1);
	while(EveReadData16(REG_CMD_WRITE) != EveReadData16(REG_CMD_READ));
	delay(2000);

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
	delay(10);
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

  digitalWrite(csPin, LOW);  
  for(int i=0; i<3;i++){
     SPI.transfer(wdata[i]);
  }
  for(int i=0; i<len;i++){
     *(buffer+i)=SPI.transfer(ZERO);
  }
  digitalWrite(csPin, HIGH);
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

void EveWriteCmd(unsigned char ftCommand)
{
  digitalWrite(csPin, LOW);   
  SPI.transfer(ftCommand);     
  SPI.transfer(0x00);     
  SPI.transfer(0x00); 
  digitalWrite(csPin, HIGH);
}

unsigned char EveReadData8(unsigned long ftAddress)
{
  unsigned char ftData8 = ZERO;
  digitalWrite(csPin, LOW); 
  SPI.transfer((char)(ftAddress >> 16) | MEM_READ); 
  SPI.transfer((char)(ftAddress >> 8)); 
  SPI.transfer((char)(ftAddress)); 
  SPI.transfer(ZERO);      
  ftData8 = SPI.transfer(ZERO); 
  digitalWrite(csPin, HIGH); 
  return ftData8;     
}

unsigned int EveReadData16(unsigned long ftAddress)
{
  unsigned int ftData16;
  digitalWrite(csPin, LOW);
  SPI.transfer((char)(ftAddress >> 16) | MEM_READ);
  SPI.transfer((char)(ftAddress >> 8)); 
  SPI.transfer((char)(ftAddress));
  SPI.transfer(ZERO);      
    ftData16 = (SPI.transfer(ZERO));  
    ftData16 = (SPI.transfer(ZERO) << 8) | ftData16; 
  digitalWrite(csPin, HIGH);
  return ftData16;
}

unsigned long EveReadData32(unsigned long ftAddress)
{
  unsigned long ftData32;
  digitalWrite(csPin, LOW);
  SPI.transfer((char)(ftAddress >> 16) | MEM_READ);
  SPI.transfer((char)(ftAddress >> 8));
  SPI.transfer((char)(ftAddress));
  SPI.transfer(ZERO);
    ftData32 = (SPI.transfer(ZERO)); 
    ftData32 = (SPI.transfer(ZERO) << 8) | ftData32;
    ftData32 = (SPI.transfer(ZERO) << 16) | ftData32;
    ftData32 = (SPI.transfer(ZERO) << 24) | ftData32;
  digitalWrite(csPin, HIGH);
  return ftData32;
}

void EveWriteData8(unsigned long ftAddress, unsigned char ftData8)
{
  digitalWrite(csPin, LOW);
  SPI.transfer((char)(ftAddress >> 16) | MEM_WRITE);
  SPI.transfer((char)(ftAddress >> 8));
  SPI.transfer((char)(ftAddress));
  SPI.transfer(ftData8);  
  digitalWrite(csPin, HIGH);
}

void EveWriteData16(unsigned long ftAddress, unsigned int ftData16)
{

  digitalWrite(csPin, LOW);
  SPI.transfer((char)(ftAddress >> 16) | MEM_WRITE);
  SPI.transfer((char)(ftAddress >> 8));
  SPI.transfer((char)(ftAddress));
  SPI.transfer((char)(ftData16));
  SPI.transfer((char)(ftData16 >> 8));
  digitalWrite(csPin, HIGH);
}

void EveWriteData32(unsigned long ftAddress, unsigned long ftData32)
{
  digitalWrite(csPin, LOW);
  SPI.transfer((char)(ftAddress >> 16) | MEM_WRITE);
  SPI.transfer((char)(ftAddress >> 8));
  SPI.transfer((char)(ftAddress));
  SPI.transfer((char)(ftData32)); 
  SPI.transfer((char)(ftData32 >> 8));
  SPI.transfer((char)(ftData32 >> 16));
  SPI.transfer((char)(ftData32 >> 24));
  digitalWrite(csPin, HIGH); 
}

void EveWriteData32BufInc(unsigned long ftAddress, unsigned long ftData32)
{
  ftAddress=ftAddress+buf_size;
  digitalWrite(csPin, LOW);    
  SPI.transfer((char)(ftAddress >> 16) | MEM_WRITE);
  SPI.transfer((char)(ftAddress >> 8));
  SPI.transfer((char)(ftAddress));    
  SPI.transfer((char)(ftData32));  
  SPI.transfer((char)(ftData32 >> 8));
  SPI.transfer((char)(ftData32 >> 16));
  SPI.transfer((char)(ftData32 >> 24));  
  digitalWrite(csPin, HIGH);   
  buf_size=buf_size+4;
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
