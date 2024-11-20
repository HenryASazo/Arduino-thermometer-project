#include "include.h"
#include "servo.h"

void servo_init(void)
{
    //Make the PB3 an output
	//For the Servo Motor
	DDRB |= (1 << 3);
}

//Function to initialize the timer2 counter
//For the servo motor
void timer2_init(void)
{
    TCCR2A |= (0b11 << WGM20);  // Fast PWM mode, modulus = 256
    TCCR2A |= (0b10 << COM2A0); // Turn D11 on at 0x00 and off at OCR2A
    OCR2A = 23;                 // Initial pulse duty cycle of 50%
    TCCR2B |= (0b111 << CS20);  // Prescaler = 1024 for 16ms period
}

//A Function for mapping the temperature
//value to the sevo motor
void servo_map(int temperature)
{
	//Keeping the OCR in range
	if(OCR2A > 35)
	{
		OCR2A = 35;
	}
	else if(OCR2A < 12)
	{
		OCR2A = 12;
	}

	//This will find the slope of the
	//equation to find the linear equation
	int top = 12 - 35;
	int bottom = 100 - 40;

	//Setting the OCR to its proper value
	int temp = (((top * temperature) / bottom) - ((top * 40) / 35) + 35);

	//Now we set the OCR2A
	OCR2A = temp;
}