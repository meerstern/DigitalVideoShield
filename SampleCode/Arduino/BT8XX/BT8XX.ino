/* * * * * * * * * * * * * * * * * * * * * * * * * *
 * FTDI EVE Firmware for BT81X                     *
 * Copyright (c) 2019                              *
 * K.Watanabe,Crescent                             *
 * Released under the MIT license                  *
 * http://opensource.org/licenses/mit-license.php  *
 * 19/02/16 v1.0 Initial Release                   *
 * 19/02/27 v1.1 Fix initialization stability      *
 * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <Arduino.h>    
#include <SPI.h>       
#include "BT81X.h"

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

	
  	cmdWrite(CLKEXT);    
  	cmdWrite(CLK36M);      
   	cmdWrite(ACTIVE);     
  	delay(300);     

	
  	while(CHIP_ID_RESET!=rd8(REG_ID)){
  		cnt++;
  		if(cnt>100){
        Serial.print("EVE NOT Found!! :");
        Serial.println(rd8(REG_ID));
  			delay(100);
  			break;
  		}
  	} 
    if(cnt<100)Serial.println("EVE Found!!");

    
   
    wr32(RAM_CMD+0, CMD_FLASHDETACH);
    wr32(REG_CMD_WRITE,4) ;
  
  	cnt=0;
  	while(0x0!=rd16(REG_CPURESET)){
  		cnt++;
  		if(cnt>100){
  			Serial.print("EVE NOT Active!! :");
  			Serial.println(rd16(REG_CPURESET));
  			delay(200);
  		}
  	}
	  if(cnt<100)Serial.println("EVE Active!!");

    wr32(REG_FREQUENCY,0x3938700);   // Set CPU Frequency
    
  	// Set Resolution
  	wr16(REG_HCYCLE,	USR_HCYCLE);
  	wr16(REG_HOFFSET,	USR_HOFFSET);
  	wr16(REG_HSYNC0,	USR_HSYNC0);
  	wr16(REG_HSYNC1,	USR_HSYNC1);
  	wr16(REG_VCYCLE,	USR_VCYCLE);
  	wr16(REG_VOFFSET,	USR_VOFFSET);
  	wr16(REG_VSYNC0,	USR_VSYNC0);
  	wr16(REG_VSYNC1,	USR_VSYNC1);
  	wr8(REG_SWIZZLE,	USR_SWIZZLE);
  	wr8(REG_PCLK_POL,	USR_PCLK_POL);
  	wr16(REG_HSIZE,		USR_HSIZE);
  	wr16(REG_VSIZE,		USR_VSIZE);
  	wr8(REG_CSPREAD,	USR_CSPREAD);
  	wr8(REG_DITHER,		USR_DITHER);

    wr16(REG_GPIOX_DIR, 0xFFFF);
    wr16(REG_GPIOX, 0xFFFF);
  	
  	//Clear Dispaly
    wr32(RAM_DL+0,  CLEAR_COLOR_RGB(255,255,255));
    wr32(RAM_DL+4,  CLEAR(1,1,1));
    wr32(RAM_DL+8,  DISPLAY());
    wr32(REG_CMD_WRITE,12);

    //Display swap
    wr8(REG_DLSWAP, DLSWAP_FRAME);


    //LCD visible
    wr8(REG_PCLK, 2);
    delay(300);
  
    Serial.println("Hello");
}


void loop() 
{

  	//Disp Logo
  	wr32(RAM_CMD+0,	CMD_LOGO);
  	wr32(REG_CMD_WRITE,4) ;
  	delay(1);
  	while(rd16(REG_CMD_WRITE) != rd16(REG_CMD_READ));
  	delay(2000);
	
	
    //Display Hello
    wr32(RAM_DL+0,   CLEAR_COLOR_RGB(255,255,255));
    wr32(RAM_DL + 4, CLEAR(1, 1, 1)); // clear screen
    wr32(RAM_DL + 8, COLOR_RGB(1, 1, 1)); // change colour to red
    wr32(RAM_DL + 12, BEGIN(BITMAPS)); // start drawing bitmaps
    wr32(RAM_DL + 16, VERTEX2II(220, 110, 31, 'H')); // ascii H
    wr32(RAM_DL + 20, VERTEX2II(248, 110, 31, 'E')); // ascii E
    wr32(RAM_DL + 24, VERTEX2II(272, 110, 31, 'L')); // ascii L
    wr32(RAM_DL + 28, VERTEX2II(299, 110, 31, 'L')); // ascii L
    wr32(RAM_DL + 32, VERTEX2II(326, 110, 31, 'O')); // ascii O
    wr32(RAM_DL + 36, END());
    wr32(RAM_DL + 40, COLOR_RGB(0, 0, 255)); // change colour to red
    wr32(RAM_DL + 44, POINT_SIZE(320)); // set point size to 20 pixels in radius
    wr32(RAM_DL + 48, BEGIN(POINTS)); // start drawing points
    wr32(RAM_DL + 52, VERTEX2II(192, 133, 0, 0)); // red point
    wr32(RAM_DL + 56, END());
    wr32(RAM_DL + 60, DISPLAY()); // display the image
    wr8(REG_DLSWAP, DLSWAP_FRAME);
    HAL_Delay(3000);


}



void cmdWrite(unsigned char ftCommand)
{
  digitalWrite(csPin, LOW);    // Set CS# low
  SPI.transfer(ftCommand);      // Send command
  SPI.transfer(0x00);       // Commands consist of two more zero bytes
  SPI.transfer(0x00);       // Send last zero byte
  digitalWrite(csPin, HIGH);   // Set CS# high
}

unsigned char rd8(unsigned long ftAddress)
{
  unsigned char ftData8 = ZERO;
  digitalWrite(csPin, LOW);    // Set CS# low
  SPI.transfer((char)(ftAddress >> 16) | MEM_READ); // Send Memory Write plus high address byte
  SPI.transfer((char)(ftAddress >> 8));   // Send middle address byte
  SPI.transfer((char)(ftAddress));    // Send low address byte
  SPI.transfer(ZERO);       // Send dummy byte
    ftData8 = SPI.transfer(ZERO);   // Read data byte
  digitalWrite(csPin, HIGH);   // Set CS# high
  return ftData8;       // Return byte read
}

unsigned int rd16(unsigned long ftAddress)
{
  unsigned int ftData16;
  digitalWrite(csPin, LOW);    // Set CS# low
  SPI.transfer((char)(ftAddress >> 16) | MEM_READ); // Send Memory Write plus high address byte
  SPI.transfer((char)(ftAddress >> 8));   // Send middle address byte
  SPI.transfer((char)(ftAddress));    // Send low address byte
  SPI.transfer(ZERO);       // Send dummy byte
    ftData16 = (SPI.transfer(ZERO));    // Read low byte
    ftData16 = (SPI.transfer(ZERO) << 8) | ftData16; // Read high byte
  digitalWrite(csPin, HIGH);   // Set CS#hHigh
  return ftData16;        // Return integer read
}

unsigned long rd32(unsigned long ftAddress)
{
  unsigned long ftData32;
  digitalWrite(csPin, LOW);    // Set CS# low
  SPI.transfer((char)(ftAddress >> 16) | MEM_READ); // Send Memory Write plus high address byte
  SPI.transfer((char)(ftAddress >> 8));   // Send middle address byte
  SPI.transfer((char)(ftAddress));    // Send low address byte
  SPI.transfer(ZERO);       // Send dummy byte
    ftData32 = (SPI.transfer(ZERO));    // Read low byte
    ftData32 = (SPI.transfer(ZERO) << 8) | ftData32;
    ftData32 = (SPI.transfer(ZERO) << 16) | ftData32;
    ftData32 = (SPI.transfer(ZERO) << 24) | ftData32; // Read high byte
  digitalWrite(csPin, HIGH);   // Set CS# high
  return ftData32;        // Return long read
}

void wr8(unsigned long ftAddress, unsigned char ftData8)
{
  digitalWrite(csPin, LOW);    // Set CS# low
  SPI.transfer((char)(ftAddress >> 16) | MEM_WRITE); // Send Memory Write plus high address byte
  SPI.transfer((char)(ftAddress >> 8));   // Send middle address byte
  SPI.transfer((char)(ftAddress));    // Send low address byte
  SPI.transfer(ftData8);      // Send data byte
  digitalWrite(csPin, HIGH);   // Set CS# high
}

void wr16(unsigned long ftAddress, unsigned int ftData16)
{

  digitalWrite(csPin, LOW);    // Set CS# low
  SPI.transfer((char)(ftAddress >> 16) | MEM_WRITE); // Send Memory Write plus high address byte
  SPI.transfer((char)(ftAddress >> 8));   // Send middle address byte
  SPI.transfer((char)(ftAddress));    // Send low address byte
  SPI.transfer((char)(ftData16));   // Send data low byte
  SPI.transfer((char)(ftData16 >> 8));    // Send data high byte
  digitalWrite(csPin, HIGH);   // Set CS# high
}

void wr32(unsigned long ftAddress, unsigned long ftData32)
{
  digitalWrite(csPin, LOW);    // Set CS# low
  SPI.transfer((char)(ftAddress >> 16) | MEM_WRITE); // Send Memory Write plus high address byte
  SPI.transfer((char)(ftAddress >> 8));   // Send middle address byte
  SPI.transfer((char)(ftAddress));    // Send low address byte
  SPI.transfer((char)(ftData32));   // Send data low byte
  SPI.transfer((char)(ftData32 >> 8));
  SPI.transfer((char)(ftData32 >> 16));
  SPI.transfer((char)(ftData32 >> 24));   // Send data high byte
  digitalWrite(csPin, HIGH);   // Set CS# high
}


