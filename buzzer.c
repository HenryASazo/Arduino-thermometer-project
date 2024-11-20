#include "buzzer.h"
#include "include.h"

volatile int count = 0;
volatile int buzz = 0;

// Frequencies for natural notes from middle C (C4)
// up one octave to C5.
unsigned short frequency[8] =
    { 262, 294, 330, 349, 392, 440, 494, 523 };

//Function to initialize the TIMER2
//For the buzzer1
void timer0_init(void){
	TCCR0A |= (1 << WGM01);   // set to CTC mode
	TIMSK0 |= (1 << OCIE0A); // enable timer interupt
	OCR0A = 80; // setting max counter w/ prescalar 256 
}

//ISR for the buzzer timer
ISR(TIMER0_COMPA_vect)
{
	if(buzz == 0)
	{ 
		PORTC |= (1 << PC5); 
		buzz = 1; 
	}
	else
	{
		PORTC &= ~(1 << PC5); 
		buzz = 0; 
	}
	count++; 
	if(count > 500)
	{
		TCCR0B &= ~(1 << CS02);
		count = 0; 
	}
}