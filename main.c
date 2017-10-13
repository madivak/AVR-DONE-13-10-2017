
/*
 * GPS-UART-BUF.c
 *
 * Created: 10/13/2017 5:37:46 PM
 * Author : madiv
 */ 
#define F_CPU 1000000UL 
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "AT-SMS.h"

#define FOSC 2000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (((FOSC / (BAUD * 16UL))) - 1)

static FILE uart0_output = FDEV_SETUP_STREAM(USART0_Transmit, NULL, _FDEV_SETUP_WRITE);
static FILE uart1_output = FDEV_SETUP_STREAM(USART1_Transmit, NULL, _FDEV_SETUP_WRITE);
static FILE uart1_input = FDEV_SETUP_STREAM(NULL, USART1_Receive, _FDEV_SETUP_READ);
static FILE uart0_input = FDEV_SETUP_STREAM(NULL, USART0_Receive, _FDEV_SETUP_READ);

int CheckSMS();
void checkOKstatus();
char sample_GPS_data (void);
int CarStatus(int p);
int sender();
int CompareNumber();

#define CAR_ON	PORTB |= (1<<PORTB1)
#define CAR_OFF	PORTB &= ~(1<<PORTB1)

int a; int i; char w; int y;
char input;
char buff[20];
char company[]	= "NUMBER";
char owner[]	= "NUMBER";

int main( void )
{
	CAR_ON;
	
	USART1_Init(MYUBRR);
	USART0_Init(MYUBRR);
	DDRB |= (1<<DDB1); //set PORTB1 as output
	
 	stdin = &uart0_input;
 	stdout = &uart0_output;
//	sei();
	
	_delay_ms(1000);
/*	printf("\r\n STARTING SYSTEM \r\n");*/

	while(1) 
	{
		//check availability of SMS
		int n = 0;
		CheckSMS(); //check if available unread SMS and its content
		int f = buff[13]; // get car status value from buff[13]

		//check SMS source
		if (f < 3) // if SMS content is "1" or "0"
		{ CompareNumber(); }  //check whether text no# is authorized + insert values to buff[14] & buff[15]
		else
		{ buff[14] = buff[15] = 0; } // if SMS content is neither "1" or "2"

		//Alter status of car
		int z = buff[14] + buff[15]; //sum values of the 2 buffer values
		if (z < 2) //A scenario of receiving text from an authorized no# with '1' or '0'
		{ CarStatus(f); }
		else //A scenario of receiving text from Unauthorized no#
		{}		
/////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		fdev_close();
		stdout = &uart0_output;
		stdin = &uart1_input;
		sample_GPS_data ();
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		i=0;		
		fdev_close(); // monitor proceedings
		stdout = &uart1_output;
		printf("\r\nPrinting buffer");
		printf("\r\nCar status is = %d\r\nText no# is :", f);
		while (n < 13)
		{
			printf("%c", buff[n]);
			n++;
		}
		printf("\r\nbuff[14] = %d", buff[14]);
		printf("\r\nbuff[15] = %d", buff[15]);
		printf("\r\nbuff[14] + buff[15] = %d", z);
		printf("\r\nbuffer end\r\n");
/////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		fdev_close();
		stdout = &uart0_output;
		stdin = &uart0_input;
		_delay_ms(4000);
	}
	return 0;
}

int CheckSMS()
{
	//char w;
	y=0;
	a=0;
	printf("AT\r\n");
	checkOKstatus();
	
	printf("AT+CMGF=1\r\n");
	checkOKstatus();
	
	printf("AT+CMGL=\"REC UNREAD\"\r\n");
	while (a < 2) //skip the <LF>
	{
		w = getchar();
		if (w==0x0A)
		{ a++; }
		else
		{}
	}
	w = getchar();
	
	if (w==0x02B) // if w = +
	{
		sender();
		
		w = getchar();
		while (w !=  0x0A) //w is not <LF>
		{ w = getchar();}
		
		w = getchar();
		if (w == 0x030)//w is '0'
		{ buff[13] = 1;  }
		else if(w == 0x031)//w is '1'
		{ buff[13] = 2;  }
		else{buff[13] = 6;}
	}
	else if(w==0x04F) // if w = 'O'
	{
		w = getchar();
		if (w==0x04B) // if w = 'K'
		{ buff[13] = 3; }
		else
		{ buff[13] = 4; }
	}
	else
	{buff[13] = 5;}
	
		if (buff[0]<3) // clear sms storage area if 0/1 is received
		{
			printf("AT+CMGF=1\r\n");
			printf("AT+CMGD=1,4\r\n"); //clearing all SMS in storage AREA
		}
		else
		{}

	return *buff;
}

void checkOKstatus()
{
		w = getchar();
		while (w!=0x04F) // if w = O
		{	w = getchar();	}
			
		while (w!=0x04B) // if w = K
		{	w = getchar();	}
}

int CarStatus(int p)
{
	int c;
	c = p;
	if (c == 1)
	{ CAR_OFF; }
	else if (c == 2)
	{ CAR_ON; }
	else
	{}
	return 0;
}

int sender()
{
	int n;
	w = getchar();
	while (w != 0x02B) // while w is not +
	{ w = getchar(); }
	
	for (n=0; n<13; n++) //capture 13 digit phone number
	{	buff[n] = w;
	w = getchar();}
	
	return *buff;
}

int CompareNumber()
{
	int j;
	buff[14]=buff[15]=0;
	
	for (j=0; j<13; j++)
	{
		if (buff[j]!=company[j])
		{ buff[14] = 1;}
		else if (buff[j]!=owner[j])
		{ buff[15] = 1;}
		else{}
	}
	return *buff;
}

char sample_GPS_data (void)
{
	int i=0;
	printf("AT\r\n");
	_delay_ms(1000);
	printf("AT+CGATT=1\r\n");
	_delay_ms(2000);
	printf("AT+CIPMUX=0\r\n");
	_delay_ms(1000);
	printf("AT+CSTT=\"APN\",\"\",\"\"\r\n");
	_delay_ms(2000);
	printf("AT+CIICR\r\n");
	_delay_ms(3000);
	printf("AT+CIFSR\r\n");
	_delay_ms(2000);
	printf("AT+CIPSTART=\"TCP\",\"IP\",\"PORT\"\r\n");
	_delay_ms(1000);
	printf("AT+CIPSEND\r\n");
	_delay_ms(2000);
	printf("\r\n$GPGG");
	while(i == 0)
	{
		input = getchar();
		if (input == 0x024) //if the character is "$"
		{
			input = getchar();
			if (input == 0x047) //if the character is "G"
			{
				input = getchar();
				if (input == 0x050) //if the character is "P"
				{
					input = getchar();
					if (input != 0x047) //if the character is not "G"
					{}
					else
					{
						input = getchar();
						if (input != 0x047) //if the character is not "G"
						{}
						else
						{
							input = getchar();
							while (input != 0x024)  //if the character is not "$"
							{
								putchar(input);
								input = getchar();
								i=1;
							}
						}
					}
				}
				else{}
			}
			else{}
		}
		else{}
	}
//	printf("\r\nsStatus = %d", x);
	putchar(0x1A); //putting AT-MSG termination CTRL+Z in USART0
	_delay_ms(2000);
	printf("AT+CIPCLOSE\r\n");
	_delay_ms(2000);
	return 0;
}
