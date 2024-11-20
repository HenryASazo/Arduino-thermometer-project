#include "buttons.h"
#include "include.h"

volatile int button_flag = 0;
extern int wholeNumber;
extern int read_input;

//Button initialization
void button_init(void) {
    // Initializing pull-up resistors 
    //for the buttons
	DDRC &= ~((1 << PC1) | (1 << PC2));
	PORTC |= ((1 << PC1) | (1 << PC2));

	// Turn on interrupts for input buttons
	PCICR |= (1 << PCIE1);

    //Only listen to these specific ports
	PCMSK1 |= ((1 << PCINT9) | (1 << PCINT10));
}

//Selects what button to display on lcd
void button_select(void)
{
    //Local button
    if (button_flag == 0) 
    {
        lcd_moveto(0,0);
        lcd_stringout(">");
        lcd_moveto(1,0);
        lcd_stringout(" ");
        servo_map(wholeNumber);
    }
    //romote button
    else if (button_flag == 1) 
    {
        lcd_moveto(0,0);
        lcd_stringout(" ");
        lcd_moveto(1,0);
        lcd_stringout(">");
        servo_map(read_input);
    }
}

// ISR for buttons
ISR(PCINT1_vect) {
    char INPUT = PINC;
    if ((INPUT & (1 << 1)) == 0) {
        button_flag = 0;
    }
    else if ((INPUT & (1 << 2)) == 0) {
        button_flag = 1;
    }
}