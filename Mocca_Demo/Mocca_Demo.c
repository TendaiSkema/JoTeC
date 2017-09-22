/***********************************************************************
 *                                                                     
 *  DATE        : 11.11.2014										   
 *  DESCRIPTION : Testsoftware für Mocca			                                                                       
 *  Author:		:                 
 *                                                                     
 ***********************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Mocca_Treiber.h"

#define clock_high (PORTD &= 0xBF);
#define clock_low (PORTD |= 0x40);


void send_data(u8 number,u8 send_to)
{
	//set send and clock to output
	DDRD = 0xFF;
	
	//clock pin set high
	clock_high;
	wait_5ms(2);
	clock_low;
	wait_5ms(1);
	
	//send address
	for (u8 i=0;i<8;i++)
	{
		//take msb from number and set data to that bit & clock port high
		PORTD = (send_to&0x80);
		//shift number for sending the next bit
		send_to<<=1;
		//wait some time
		wait_5ms(2);
		
		//clock low
		clock_low;
	}
	wait_5ms(5);

	for (u8 i=0;i<8;i++)
	{
		//take msb from number and set data to that bit & clock port high
		PORTD = (number&0x80);
		//shift number for sending the next bit
		number<<=1;
		//wait some time
		wait_5ms(2);
		
		//clock low
		clock_low;
	}
}

u8 recieve_data(u8 address)
{
	u8 rec = 0; 
	
	//wait for end of wakeup signal
	while(!(PIND & 0x40));
	
	for (u8 i=0;i<8;i++)
	{
		while((PIND & 0x40));
		
		rec<<=1;
		rec += ((PIND & 0x80)>>7);
		
		
		while(!(PIND & 0x40));
	}

	if (rec == address)
	{
		rec = 0;
		for (u8 i=0;i<8;i++)
		{
			while((PIND & 0x40));
			
			rec<<=1;
			rec += ((PIND & 0x80)>>7);
			
			
			while(!(PIND & 0x40));
		}
		return rec;
	}

	return 0;
}

// main programm
void main(void)
{
	// Initialisierung: Board und Peripherie
	init_mocca();
	init_lcd();		
	init_ADC();
    wait_5ms(1);
	clear_lcd_f();
	

	//Set Ports to default
	DDRD = 0x3F;
	DDRC = 0xF0;
	DDRA = 0xFF;
	DDRB = 0xFF;

	//set clock pull up
	PORTD |=0x80;

	//stop leds
	PORTA = 0xFF;
	PORTB = 0xFF;

	u8 address = PORTC;
	write_text(0,3,"Address Wählen:");
	while(!Taste_1)
	{
		address = PORTC;
		write_zahl(1,10,address,3,0,0);
	}
	clear_lcd_f();
	wait_5ms(400);

	u8 data = read_ADC(9)%255;
	u8 send_to = PORTC;
	
	while (1)
	{
		//define and show data and target address
		data = read_ADC(9)%255;
		send_to = PORTC;

		write_text(1,2,"Data:");
		write_zahl(1,10,data,3,0,0);
		write_text(2,2,"Target:");
		write_zahl(2,10,send_to,3,0,0);
		write_text(3,2,"Taste 2 add. aendern");

		//first check for recieving befor send something
		if (!(PIND &= 0x40))
		{
			data = recieve_data(address);

			clear_lcd_f();
			
			//show data if no error receved
			if (data > 0)
			{
				write_text(0,2,"Empfangen:");
				write_zahl(1,7,data,4,0,0);
				wait_5ms(1000);
				clear_lcd_f();
			}
		}
		else if (Taste_1)
		{
			send_data(data,send_to);

			//show sending information
			clear_lcd_f();
			write_text(0,2,"Gesendet:");
			write_zahl(1,7,data,4,0,0);

			//Set Ports to default
			DDRD = 0x3F;

			//set clock and data as pull up
			PORTD |=0xC0;
			
			wait_5ms(1000);
			clear_lcd_f();
		}
		else if (Taste_2)
		{
			//change address
			write_text(0,3,"Address Wählen:");
			while(!Taste_1)
			{
				address = PORTC;
				write_zahl(1,10,address,3,0,0);
			}
			clear_lcd_f();
			wait_5ms(400);
		}
	}
}