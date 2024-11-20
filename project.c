/********************************************
 *
 *  Name: Henry Sazo
 *  Email: hsazo@usc.edu
 *  Section: Friday 11 am
 *  Assignment: Project: Thermometer
 *
 ********************************************/

#include "include.h"
#include "lcd.h"
#include "ds18b20.h"
#include "encoder.h"
#include "led.h"
#include "servo.h"
#include "serial.h"
#include "buttons.h"
#include "buzzer.h"

void splashScreen(void);
int initialize(void);

// Set to volatile to allow changes of these variables outside the main function
extern volatile unsigned char threshold; // Holds the threshold
int wholeNumber = 0;
extern volatile int convert_flag;
int check = 0;

int main(void)
{
	// Initialize EVERYTHING
	int x = initialize();

	// Check to see if temperature sensor initialization
	// failed
	if (x == 0)
	{
		lcd_writecommand(1);
		lcd_stringout("FAILED");

		// If failed just end the program
		return 0;
	}

	// This will display the splash screen
	splashScreen();

	// Read the A and B inputs to determine the intial state.
	// In the state number, B is the MSB and A is the LSB.
	// Warning: Do NOT read A and B separately.  You should read BOTH inputs
	// at the same time, then determine the A and B values from that value.
	encoder_precheck();

	// Char array to hold the two temperature bytes
	unsigned char t[2];

	lcd_moveto(0, 1);
	lcd_stringout("Loc:");
	lcd_moveto(1, 1);
	lcd_stringout("Rmt:");

	// Beginning the first conversion
	ds_convert();

	while (1)
	{
		// True when first conversion is over
		if (ds_temp(t))
		{
			// Move both value into a single int
			int temperature = ((t[1] << 8) + t[0]);

			// Creating the conversion from Celsius to Fahrenheit
			int fahrenheitValue = (9 * temperature) / 8 + 320;

			// The ones, and tens place
			wholeNumber = fahrenheitValue / 10;

			// Decimals place
			int decimal = fahrenheitValue % 10;

			// Call the function to map the servo motor
			// to its corresponding point
			if (check == 0)
			{
				servo_map(wholeNumber);
				check = 1;
			}

			// Outputting the temperature onto the LCD
			char buf[15];
			snprintf(buf, 5, "%d.%d", wholeNumber, decimal);
			lcd_moveto(0, 6);
			lcd_stringout(buf);

			// Outputting the Threshold on the LCD
			snprintf(buf, 15, "%d", threshold);
			lcd_moveto(0, 14);
			lcd_stringout(buf);

			// Transmit
			serial_transmit();

			// Function to check what led and
			// behavior of led to perform
			led_output();

			if (convert_flag == 1)
			{
				// Conversion of ASCII
				// Characters
				serial_convert();
			}

			// Function to change on lcd
			// which button to select
			// to displah temperature
			button_select();

			// Beginning of a new conversion
			ds_convert();
		}
	}
}

// Function to splash screen
void splashScreen(void)
{
	// Clear the LCD
	lcd_writecommand(1);

	/* Call lcd_stringout to print out your name */
	lcd_moveto(0, 3); // Centers it on the LCD
	lcd_stringout("Henry Sazo");
	lcd_moveto(1, 1);
	lcd_stringout("EE 109 Project");

	/* Delay 1 second */
	_delay_ms(1000);

	// CLear the LCD
	lcd_writecommand(1);
}

// Function to initialize EVERYTHING
int initialize(void)
{
	// Initialize DDR and PORT registers and LCD
	// This will initialize the lcd to the default settings
	lcd_init();

	// Initialize the temperature reader
	int x = 1;
	if (ds_init() == 0)
	{ // Initialize the DS18B20
		x = 0;
	}

	// Initialize the TIMER2
	timer2_init();

	// Initialize the TIMER1
	timer1_init();

	// Initialize TIMER0
	timer0_init();

	// Initializing the buttons
	button_init();

	// Initializing the serial
	serial_init();

	// Initializing the servo motor
	servo_init();

	// Making pin C5 an output for the buzzer
	DDRC |= (1 << 5);

	// Initializing the encoder
	encoder_init();

	// Initialize the LEDs
	led_init();

	// making buffer I/O an output
	DDRC |= (1 << 4);

	// enabling buffer pullup
	PORTC &= ~(1 << 4);

	sei(); // global

	// Getting the threshold before even starting the program
	unsigned char getThresh = eeprom_read_byte((void *)100);
	if (getThresh >= 40 && getThresh <= 90)
	{
		threshold = getThresh;
	}
	else
	{
		threshold = 80;
	}

	return x;
}
