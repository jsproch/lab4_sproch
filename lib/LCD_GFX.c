/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: You
 */ 

#include "LCD_GFX.h"
#include "ST7735.h"
#include <string.h>

/******************************************************************************
* Local Functions
******************************************************************************/



/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
* @brief		Convert RGB888 value to RGB565 16-bit color data
* @note
*****************************************************************************/
uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
}

/**************************************************************************//**
* @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
* @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
* @note
*****************************************************************************/
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
	LCD_setAddr(x,y,x,y);
	SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
* @brief		Draw a character starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
	uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
	int i, j;
	if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
		for(i=0;i<5;i++){
			uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
			for(j=0;j<8;j++){
				if ((pixels>>j)&1==1){
					LCD_drawPixel(x+i,y+j,fColor);
				}
				else {
					LCD_drawPixel(x+i,y+j,bColor);
				}
			}
		}
	}
}


/******************************************************************************
* LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
* You are free to create and add any additional files, libraries, and/or
*  helper function. All code must be authentically yours.
******************************************************************************/

/**************************************************************************//**
* @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
* @brief		Draw a colored circle of set radius at coordinates
* @note
*****************************************************************************/
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
{
	//This is using bresenham's circle algorithm
	int x = 0;
	int y = radius;
	int d = 3 - 2 * radius;
	while(y>=x){
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else{
			d = d + 4 * x + 6;
		}
		// rather than an empty circle that the points in the algorith makes,
		//This draws blocks between them to fill it in.
		LCD_drawBlock(x0+x, y0-y,x0+x, y0+y, color);
		LCD_drawBlock(x0-x, y0-y,x0-x, y0+y, color);
		LCD_drawBlock(x0+y, y0-x,x0+y, y0+x, color);
		LCD_drawBlock(x0-y, y0-x,x0-y, y0+x, color);
		x++;
		
	}
}


/**************************************************************************//**
* @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
* @brief		Draw a line from and to a point with a color
* @note
*****************************************************************************/
void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
{
	//This is bresenham's line algorithm
	 int dx = x1 - x0;
	 int dy = y1 - y0;
	 int D = 2*dy - dx;
	 int y = y0;

	 for (int x = x0;x<=x1;x++){
	 LCD_drawPixel(x,y,c);
	 if (D > 0){
	 y = y + 1;
	 D = D - 2*dx;
	 }
	 D = D + 2*dy;
	 }
}



/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
{
	LCD_setAddr(x0,y0,x1,y1);
	//set off the entire block of addresses that need filled
	int x = x1-x0;
	int y = y1-y0;
	for(int count1 = 0;count1<=x;count1++){
		for(int count2 = 0; count2<=y;count2++){
			//iterate through all addresses and send color info
			SPI_ControllerTx_16bit(color);
		}
	}
}

/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color) 
{
	LCD_setAddr(0,0,LCD_WIDTH,LCD_HEIGHT);
	for(int x = 0;x<=LCD_WIDTH;x++){
		for(int y = 0; y<=LCD_HEIGHT;y++){
			//essentially just a really big draw block
			SPI_ControllerTx_16bit(color);
		}
	}
}

/**************************************************************************//**
* @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
* @brief		Draw a string starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
{
	int count = strlen(str);
	for(int s =0; s<count;s++){
	LCD_drawChar(x,y,str[s],fg,bg);
	x += 5; 
	//5 pixels seems to be the width of one char
	}
	}