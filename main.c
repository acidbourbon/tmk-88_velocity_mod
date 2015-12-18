

#define F_CPU 12e6
 
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>




//#################### init - routines ####################

void init_USART(void)
{
	UCSRB |=  (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
	UCSRC |= (1<<UCSZ0)|(1<<UCSZ1); 
	UBRRH = 0;
	UBRRL = 23;
	// this initializes the UART and sets it to 31250 baud, the MIDI baudrate
	// note: the values change if you would use another quartz than 12 MHz
}

//----------------------------------------




//############################ main #####################################
 


int main (void) {          

uint8_t counter= 0,control_type =0;

uint8_t temp;


init_USART();



while(1) // begin the processing loop
	{




	//wait until MIDI data is received (single byte)

	while(!(UCSRA&(1<<RXC))){};

	//store received data to temp
	temp = UDR;
	if (temp >=128) // is it a control/command defining byte?
	{
		counter = 0; // reset the counter that counts how many bytes have arrived after a command byte
		if (temp == 0x90) // signature of Note-On command found
		{control_type = 1;} // 1 for Note-On command
		else
		{control_type = 0;}
	}

	if (control_type && counter ==2 && temp !=0) // note on event came and "temp" is the byte that holds velocity information
	{
		temp = (uint8_t)(temp*1.625); // here the magic happens: multiply velocity value by 1.625
		if(temp > 127) // if the result is higher than the maximum allowed value, crop result to allowed maximum
		{temp = 127;}
	}
	
	
	counter ++; // increase counter that counts how many bytes have arrived after a command byte
	if (counter ==3)
	{counter = 1;}
	
	while (!(UCSRA&(1<<UDRE))){}; // wait until USART is idle again
	UDR=temp; // send midi byte




}


	

   return 0;              
}






