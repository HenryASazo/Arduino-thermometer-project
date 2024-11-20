/*
  lcd.c - Routines for sending data and commands to the LCD shield
*/

#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"                // Declarations of the LCD functions

/* This function not declared in lcd.h since
   should only be used by the routines in this file. */
void lcd_writenibble(unsigned char);

/* Define a couple of masks for the bits in Port B and Port D */
#define DATA_BITS ((1 << PD7)|(1 << PD6)|(1 << PD5)|(1 << PD4))
#define CTRL_BITS ((1 << PB1)|(1 << PB0))
#define MASK (0xf0) // mask of our bits to send
#define ENABLE (1 << PB1) //enable port
#define REGISTER (1 << PB0) //registor port

/*
  lcd_init - Do various things to initialize the LCD display
*/
void lcd_init(void)
{
    DDRB |= CTRL_BITS;  //Set to Output   // Set the DDR register 
                                          //bits for ports B and D
    DDRD |= DATA_BITS;  //Set to Output   // Take care not to affect any unnecessary bits

    _delay_ms(15);              // Delay at least 15ms

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011
    _delay_ms(5);               // Delay at least 4msec

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011
    _delay_us(120);             // Delay at least 100usec

    lcd_writenibble(0x30);      // Use lcd_writenibble to send 0b0011, no delay needed

    lcd_writenibble(0x20);      // Use lcd_writenibble to send 0b0010
    _delay_ms(2);               // Delay at least 2ms
    
    lcd_writecommand(0x28);     // Function Set: 4-bit interface, 2 lines

    lcd_writecommand(0x0f);     // Display and cursor on
}

/*
  lcd_moveto - Move the cursor to the row and column given by the arguments.
  Row is 0 or 1, column is 0 - 15.
*/
void lcd_moveto(unsigned char row, unsigned char col)
{
    unsigned char pos;
    if(row == 0) {
        pos = 0x80 + col;       // 1st row locations start at 0x80
    }
    else {
        pos = 0xc0 + col;       // 2nd row locations start at 0xc0
    }
    lcd_writecommand(pos);      // Send command
}

/*
  lcd_stringout - Print the contents of the character string "str"
  at the current cursor position.
*/
void lcd_stringout(char *str)
{
    int i = 0;
    while (str[i] != '\0') {    // Loop until next charater is NULL byte
        lcd_writedata(str[i]);  // Send the character
        i++;
    }
}

/*
  lcd_writecommand - Output a byte to the LCD command register.
*/
void lcd_writecommand(unsigned char cmd)
{
    /* Clear PB0 to 0 for a command transfer */
    PORTB &= ~REGISTER; //Clear the registor

    /* Call lcd_writenibble to send UPPER four bits of "cmd" argument */
    lcd_writenibble(cmd); //Send the first 4 bits for nibble

    /* Call lcd_writenibble to send LOWER four bits of "cmd" argument */
    lcd_writenibble(cmd << 4); //Shift the cmd by 4 so lower 4 bits are sent for
                                //nibble

    /* Delay 2ms */
    _delay_ms(2); 

}

/*
  lcd_writedata - Output a byte to the LCD data register
*/
void lcd_writedata(unsigned char dat)
{
    /* Set PB0 to 1 for a data transfer */
    PORTB |= REGISTER; //Set the register to 1

    /* Call lcd_writenibble to send UPPER four bits of "dat" argument */
    lcd_writenibble(dat); //Send the first 4 bits of dat to nibble

    /* Call lcd_writenibble to send LOWER four bits of "dat" argument */
    lcd_writenibble(dat << 4); //Shift dat by 4 spaces to the left and send
                                //and send the lower 4 bits to nibble

    /* Delay 2ms */
    _delay_ms(2);

}

/*
  lcd_writenibble - Output the UPPER four bits of "lcdbits" to the LCD
*/
void lcd_writenibble(unsigned char lcdbits)
{
    /* Load PORTD, bits 7-4 with bits 7-4 of "lcdbits" */
    PORTD &= ~MASK; //Clearing the bits we are going to use 7-4

    PORTD |= (lcdbits & MASK); //Clear the bits in variable
                                //that we dont want to keep and the 
                                //Copy over the bits we do want and send



    /* Make E signal (PB1) go to 1 and back to 0 */
    PORTB |= ENABLE; //Make enable a 1
    PORTB |= ENABLE; //Make the enable last longer
    PORTB &= ~ENABLE; //Make enable a 0
                      //Enable = (1<<PB1)

}
