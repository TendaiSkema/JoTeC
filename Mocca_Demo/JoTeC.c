/*
 * JoTeC.c
 *
 * Created: 22.09.2017 20:39:40
 *  Author: Tendai Rondof
 */

 void use_JoTeC()
 {
	DDRD = 0x3F;
	PORTD |=0x80;
 }

 void set_ports()
 {
	DDRD = 0x3F;
	PORTD |=0x80;
 }

 void send_data(u8 number,u8 send_to)
 {
	 //set send and clock to output
	 DDRD = 0xFF;
	 
	 //clock pin set high
	 (PORTD &= 0xBF);
	 wait_5ms(2);
	 (PORTD |= 0x40);
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
		 (PORTD |= 0x40);
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
		 (PORTD |= 0x40);
	 }
	 set_ports();
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