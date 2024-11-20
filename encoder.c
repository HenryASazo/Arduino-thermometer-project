#include "encoder.h"
#include "include.h"

volatile unsigned char new_state, old_state;
volatile unsigned char threshold;
volatile unsigned char input;
volatile unsigned char a, b;

//Initialization of the encoder
void encoder_init(){
    //Enabling the pullup resistors for the 
	//rotary encoder
	DDRD &= ~((1<<2)|(1<<3));
	PORTD |= (1 << 2);
	PORTD |= (1 << 3);
	
	//Enabling the interrupts locally 
	//For PORTD 
	PCICR |= (1 << PCIE2); //local
	//Make it only listen to D3 and D2
	PCMSK2 |= (1 << PCINT19); 
	PCMSK2 |= (1 << PCINT18);
}

//Variable to check the state
//of the encoder before while loop
//in main project.c
void encoder_precheck()
{
    unsigned char input2 = PIND;
	a = input2 & (1<<2);
    b = input2 & (1<<3);

    if (!b && !a)
	old_state = 0;
    else if (!b && a)
	old_state = 1;
    else if (b && !a)
	old_state = 2;
    else
	old_state = 3;

    new_state = old_state;
}

//ISR for the Encoder
ISR(PCINT2_vect)
{
	// Read the input bits and determine A and B.
	input = PIND;
	a = input & (1 << 2);
	b = input & (1 << 3);
	
	//Old state where both a and b are 00
	if (old_state == 0) 
	{
	    //This will throw into the second
		//stage of the state machine and update the new state
		//for the next iteration of the loop
		// a = 1     b = 0
		if (a && !b) 
		{
			new_state = 1;
			threshold += 1;
		} 
		//This will thoruhg into the third
		//stage of the state machine and update the new state
		//for the next iteration of the loop
		// a = 0       b = 1
		else if (!a && b) 
		{
			new_state = 2;
			threshold -= 1;
		}
	}
	//Old state where a is 1 and b is 0
	else if (old_state == 1)
	 {
	    //This will throw into the first
		//stage of the state machine and update the new state
		//for the next iteration of the loop
		// a = 0     b = 0
		if (!a && !b) 
		{
			new_state = 0;
			threshold-=1;
		} 
		//This will throw into the third
		//stage of the state machine and update the new state
		//for the next iteration of the loop
		// a = 1     b = 1
		else if (a && b) 
		{
			new_state = 3;
			threshold+=1;
		}
	}
	//Old state where a and b are 11
	else if (old_state == 2) 
	{
	    //This will throw into the first
		//stage of the state machine and update the new state
		//for the next iteration of the loop
		// a = 1     b = 0
		if (a && !b) 
		{
			new_state = 0;
			threshold+=1;
		} 
		//This will throw into the fourth and final
		//stage of the state machine and update the new state
		//for the next iteration of the loop
		// a = 1     b = 1
		else if (a && b) 
		{
			new_state = 3;
			threshold-=1;
		}
	}
	//Old state where a is 0 and b is 1
	else if (old_state == 3)
	{ 
	    //This will throw into the second
		//stage of the state machine and update the new state
		//for the next iteration of the loop
		// a = 1     b = 0
		if (a && !b) 
		{
			new_state = 1;
			threshold-=1;
		} 
		//This will throw into the third
		//stage of the state machine and update the new state
		//for the next iteration of the loop
		// a = 0    b = 1
		else if (!a && b) 
		{
			new_state = 2;
			threshold+=1;
		}
	}

	//Keeping the threshold in the range
	//of 50 to 90 degrees
	if(threshold > 90)
	{
		threshold = 90;
	}
	else if(threshold < 50)
	{
		threshold = 50;
	}

	//Updating the EEPROM of the threshold
	unsigned char promThreshold = threshold; 
	eeprom_update_byte((void *) 100, promThreshold);

	//This is like a boolean. If the new state and old state
	//are different then there was a change and we need to 
	//mark the change and update the state so the 
	//circuit knows where it is and what to do
	if (new_state != old_state) 
	{
	    old_state = new_state;
	}
}