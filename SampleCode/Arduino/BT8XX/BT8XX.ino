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

#ifdef ENABLE_SD
  #include <SD.h>
   File sdfile;
#endif

FILE serial_stdout;

void setup() 
{
    unsigned int cnt;
	
    Serial.begin(9600);
    fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &serial_stdout;
             
    SPI.begin();          					// Initialize SPI
	  Serial.println("Initialize Start!! \r\n");
    SPI.setBitOrder(MSBFIRST);      		// Send data most significant bit first
    SPI.setDataMode(SPI_MODE0);     		// Clock idle zero, clock data into BT81X on rising edge
    SPI.setClockDivider(SPI_CLOCK_DIV4);    // Set rate at 4MHz (16MHz OSC / 4)

    pinMode(irqPin, INPUT_PULLUP);   		// BT81X interrupt output (not used in this example)
    pinMode(pwrPin, OUTPUT);     			// BT81X Power Down (reset) input
    pinMode(csPin, OUTPUT);      			// BT81X SPI bus CS# input
    pinMode(sdcsPin, OUTPUT);      			// BT81X SPI bus CS# input

	
    digitalWrite(sdcsPin, HIGH);   			// Set CS# high to start - SPI inactive
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

#ifdef ENABLE_SD
	if (!SD.begin(sdcsPin)) 
	{
		Serial.println("SD initialization failed!\n");
	}    
#endif
	
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
      delay(1);
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

  
//  EveDemo3();
//  while(1);  
  EveDemo();
  //EveRecoverCoProcessor();
  EveDemo2();  
  //EveRecoverCoProcessor();
	

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
  unsigned long res;
  digitalWrite(csPin, LOW);
  SPI.transfer((char)(ftAddress >> 16) | MEM_READ);
  SPI.transfer((char)(ftAddress >> 8));
  SPI.transfer((char)(ftAddress));
  SPI.transfer(ZERO);

  res = SPI.transfer(ZERO);
  ftData32 = ( res << 0); 
  res = SPI.transfer(ZERO);
  ftData32 = ( res << 8) | ftData32;
  res = SPI.transfer(ZERO);
  ftData32 = ( res << 16) | ftData32;
  res = SPI.transfer(ZERO);
  ftData32 = ( res << 24) | ftData32;

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

void EveLoadJpgFile(char *filename, uint32_t addr, uint32_t width, uint32_t hight)
{

  uint32_t  filesize=0;
  uint32_t  filesectors=0;
  uint32_t  sectorsspace=0;

#ifdef ENABLE_SD
  printf("Read SD & RAM Write Start\n\r");
  
  sdfile = SD.open(filename, FILE_READ);
  if(sdfile){
    printf("SD File Open Success\n\r");
  }
  else{
    printf("SD File Open ERR\n\r");
    return;
  }
   
  
  filesize = sdfile.size();
  filesectors = filesize/0x1000;

  if(filesize % 0x1000)   filesectors++;
  sectorsspace= ( 0x100000 - (addr - RAM_G)) / 0x1000;


  if ( filesectors > sectorsspace ){
    printf("Data size is over!\n\r");
    return;
  }

  printf("FileName:\"%s\" Size:%ld \r\n", filename,filesize);


  uint16_t m=0;
  EveWaitCmdFifoEmpty();
  EveSendCmd(CMD_LOADIMAGE);
  EveSendCmd(addr);
  EveSendCmd(0);//Options

  for(uint32_t i=0; i < (filesize/4 + 1) ; i++){

    uint32_t readbuf[4];

    uint32_t dat=0;

    readbuf[0]=(uint32_t)sdfile.read();
    readbuf[1]=(uint32_t)sdfile.read();
    readbuf[2]=(uint32_t)sdfile.read();
    readbuf[3]=(uint32_t)sdfile.read();

    dat =  (uint32_t)(readbuf[0]<<0)
        +(uint32_t)(readbuf[1]<<8)
        +(uint32_t)(readbuf[2]<<16)
        +(uint32_t)(readbuf[3]<<24);
    EveSendCmd(dat);
    m++;
  }

  printf("Read SD & RAM Write End\n\r");

  sdfile.close();

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

void usrdelay(uint16_t t){
  
  uint32_t wait;
  wait = millis() + t;
  while(millis() < wait);
}

int serial_putchar(char c, FILE* f) {
    if (c == '\n') serial_putchar('\r', f);
    return Serial.write(c) == 1? 0 : 1;
}

