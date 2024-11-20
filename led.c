#include "include.h"
#include "led.h"

extern int wholeNumber;
extern volatile unsigned char threshold;	
int threshcounter = 0;

void led_init()
{
    // Making PB4 and PB5 as an output bit 
	//   red LED     green LED
	DDRB |= (1 << 4); 
	DDRB |= (1 << 5);
}

//Function to Inilialize the timer1 counter
//For the blinking
void timer1_init(void)
{
    TCCR1B |= (1 << WGM12);  // Set to CTC mode
    TIMSK1 |= (1 << OCIE1A); // Enable Timer Interrupt 
    OCR1A = 31250;	// Setting max counter w/ prescalar 256 countering to 31,250 = 0.5s w/ 16MHz clock
}

void led_output(void)
{
    //Comparing values of the temperature
	//and the threshold for the LED
	//OK turn on green light
	if(wholeNumber <= threshold)
	{
		//Turning off the timer for the red LED
		TCCR1B &= ~(1 << CS12);
		PORTB &= ~(1 << 4); //turn off red
		PORTB |= (1 << 5); //turn on green
		lcd_moveto(1,12);
		lcd_stringout("COOL");
		threshcounter = 0;
	}
	//HOT turn red light on and buzz
	else if(wholeNumber >= threshold && wholeNumber - 3 >= threshold)
	{
		//Turning off the timer for the red LED
		TCCR1B &= ~(1 << CS12);
		PORTB &= ~(1 << 4); //turn off red
		PORTB |= (1 << 4); //turn on red
		PORTB &= ~(1 << 5); //turn off green
		lcd_moveto(1,12);
		lcd_stringout("HOT ");

		//Turning on the counter for the buzzer
		if(threshcounter < 1)
		{
			//Turn on the timer
			TCCR0B |= ((1 << CS02));
		}
		threshcounter = 1;
	}
	//Warm make the light blink
	else if( wholeNumber >= threshold)
	{
		TCCR1B |= (1 << CS12);
		PORTB &= ~(1<<5); //turn off green
		lcd_moveto(1,12);
		lcd_stringout("WARM");
		threshcounter = 0;
	}
}

//ISR for the LED timer
ISR(TIMER1_COMPA_vect){
	PORTB ^= (1 << PB4); 
}