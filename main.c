/*
 * ESE519_Lab4_Pong_Starter.c
 *
 * Created: 9/21/2021 21:21:21 AM
 * Author : J. Ye
 */ 

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <string.h>
#include <avr/io.h>
#include <stdio.h>
#include "lib\uart.h"
#include "lib\ST7735.h"
#include "lib\LCD_GFX.h"
#include <util/delay.h>

char String[25];
void Initialize()
{
	lcd_init();
	//set up joy stick
	PRR &= ~(1<<PRADC);
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);
	
	ADCSRA |= (1<<ADPS0);
	ADCSRA |= (1<<ADPS1);
	ADCSRA |= (1<<ADPS2);//128 scaler
	
	ADMUX &= ~(1<<MUX0);
	ADMUX &= ~(1<<MUX1);
	ADMUX &= ~(1<<MUX2);
	ADMUX &= ~(1<<MUX3);
	
	ADCSRA |= (1<<ADATE);
	ADCSRB &= ~(1<<ADTS0);
	ADCSRB &= ~(1<<ADTS1);
	ADCSRB &= ~(1<<ADTS2);
	
	DIDR0 |= (1<<ADC0D);
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);
	
	DDRD |= (1<<DDD3);
	DDRD |= (1<<DDD4);
	PORTD &= ~(1 << PORTD4); // for buzzer and led
	PORTD &= ~(1 << PORTD3);
}

int main(void)
{
	Initialize();
	UART_init(BAUD_PRESCALER);
	LCD_setScreen(WHITE); //erase clown vomit
	
	//ball
	int ballxv = (random() % 7)+2;
	int ballyv = (random() % 7)+2;//random speed/direction
	 ballxv = ballxv * (-1 * (random()%2));//makes a negative randomly to flip start direction
	 ballyv = ballyv * (-1 * (random()%2));
	int ballx = 80;//about center
	int bally = 64;
	LCD_drawCircle(ballx,bally,3,BLACK);
	//paddle com
	int comxv = (random() % 7)+2;
	int comx = 80;
	LCD_drawBlock(comx-20,0,comx+20,10,GREEN);
	//paddle player 
	int p1x = 80;
	LCD_drawBlock(p1x-20,117,p1x+20,127,RED);
	// scorebaord
	int comp = 0; //keep track of score and round
	int p1p = 0;
	int p1s = 0;
	int coms = 0;
	
    while (1) 
    {
		//ball
		LCD_drawCircle(ballx,bally,3,WHITE); //erase previous ball
		ballx += ballxv;
		bally += ballyv;
		if (ballx > LCD_WIDTH || ballx < 0){
			ballxv = -1 * ballxv;//check bounds and small beep
			PORTD |= (1 << PORTD3);
			Delay_ms(500);
			PORTD &= ~(1 << PORTD3);
		}
		if (bally > LCD_HEIGHT || bally < 0){
			if(bally < 0){
				p1p++;
				PORTD |= (1 << PORTD4); //p1 score and big beep/flash
				PORTD |= (1 << PORTD3);
				Delay_ms(2000);
				PORTD &= ~(1 << PORTD4);
				PORTD &= ~(1 << PORTD3);
			}
			else{
				comp++;
				PORTD |= (1 << PORTD4);//com score and big beep/flash
				PORTD |= (1 << PORTD3);
				Delay_ms(2000);
				PORTD &= ~(1 << PORTD4);
				PORTD &= ~(1 << PORTD3);
				
			}
			ballx = 80;//reset ball after point
			bally = 64;
			ballxv = (random() % 7)+2;
			ballyv = (random() % 7)+2;
			ballxv = ballxv * (1 + (-2 * (random()%2)));
			ballyv = ballyv * (1 + (-2 * (random()%2)));
			
		}
		if (abs(ballx - comx)<25 && bally <= 10){
			ballyv = -1 * ballyv;
			PORTD |= (1 << PORTD3); //check if touching paddle 25 to be slightly more forgiving
			Delay_ms(500);
			PORTD &= ~(1 << PORTD3);
		}
		if (abs(ballx - p1x)<25 && bally >= 117){
			ballyv = -1 * ballyv;
			PORTD |= (1 << PORTD3);
			Delay_ms(500);
			PORTD &= ~(1 << PORTD3);
		}
		LCD_drawCircle(ballx,bally,3,BLACK);
		
		//paddle com
		LCD_drawBlock(comx-20,0,comx+20,10,WHITE); //clear last position
		comx += comxv;
		if (comx > LCD_WIDTH || comx < 0){
			comxv = -1 * comxv; //bounce left and right
		}
		LCD_drawBlock(comx-20,0,comx+20,10,GREEN);
		//paddle player 
		
		int pos = ADC;//527 is still
		if(pos>527 && p1x < (135)){//set bounds and check joystick
			LCD_drawBlock(p1x-20,117,p1x+20,127,WHITE);//clear
			pos = pos - 500;
			pos = pos/100;
			p1x += pos * 2;//speed based on distance from center
			if(p1x >139){
				p1x=139;
			}
			LCD_drawBlock(p1x-20,117,p1x+20,127,RED);
		}
		else if (p1x>25) {//set bounds
			LCD_drawBlock(p1x-20,117,p1x+20,127,WHITE);
			pos = pos - 500;
			pos = pos/100;
			p1x += pos * 2;
			if(p1x >139){
				p1x=139;
			}
			LCD_drawBlock(p1x-20,117,p1x+20,127,RED);
		}
		//scoreboard
		if(p1p>=3){
			p1p=0; //3 points wins a round
			comp=0;
			p1s++;
			PORTD |= (1 << PORTD4);
			PORTD |= (1 << PORTD3);
			Delay_ms(5000);
			PORTD &= ~(1 << PORTD4);
			PORTD &= ~(1 << PORTD3);
		}
		else if(comp>=3){
			p1p=0;
			comp=0;
			coms++;
			PORTD |= (1 << PORTD4);
			PORTD |= (1 << PORTD3);
			Delay_ms(5000);
			PORTD &= ~(1 << PORTD4);
			PORTD &= ~(1 << PORTD3);
		}
		char score1 = (48+p1p);
		char score2 = (48+comp);
		char comscore = coms+48;
		char p1score = p1s+48;
		LCD_drawChar(80,30,comscore,WHITE,WHITE); //draw the scorebaord
		LCD_drawChar(80,100,p1score,WHITE,WHITE);
		LCD_drawChar(80,30,comscore,BLACK,WHITE);
		LCD_drawChar(80,100,p1score,BLACK,WHITE);
		LCD_drawChar(10,64,score1,WHITE,WHITE);
		LCD_drawChar(140,64,score2,WHITE,WHITE);
		LCD_drawChar(10,64,score1,RED,BLACK);
		LCD_drawString(60,64,": SCORE :",RED,BLACK);
		LCD_drawChar(140,64,score2,RED,BLACK);
		//Delay_ms(500);
		
    }
}