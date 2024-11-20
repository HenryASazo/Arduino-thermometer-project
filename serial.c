#include "include.h"
#include "serial.h"


#define FOSC 16000000 				// Clock frequency
#define BAUD 9600 					// Baud rate used
#define MYUBRR (FOSC/16/BAUD-1) 	// Value for UBRR0

volatile int checker = 0;
volatile int character;
volatile int convert_flag = 0;
volatile char arr[5];
int read_input = 1;
extern int wholeNumber;

//Serial Initialization
//From slides
void serial_init(void) {
    UBRR0 = MYUBRR;                         // Set baud rate
    UCSR0B |= (1 << TXEN0 | 1 << RXEN0);    // Enable RX and TX
    UCSR0B |= (1 << RXCIE0);                // Enable interrupts
    UCSR0C = (3 << UCSZ00);                 // Async., no parity, 1 stop bit, 8 data bits
}

//Function to convert string ASCII characters into
//a binary fixed point number
void serial_convert()
{
    convert_flag = 0;
    lcd_moveto(1,6);
    sscanf(arr,"%3d",&read_input);
    char out[5];
    snprintf(out,5,"%d",read_input);
    lcd_stringout(out);
}

//Transmit to the other board
void serial_transmit(void)
{
    char txx[7];
    snprintf(txx,7,"@+%d#",wholeNumber);
    tx_write(txx);
}

//From the slides
void tx_char(char ch) {
    // Wait for transmitter data register empty
    while ((UCSR0A & (1 << UDRE0)) == 0) {}
    UDR0 = ch;
}

//Helper function to transmit to the other board
void tx_write(char *string) 
{
    int i = 0;
    while (string[i] != '\0') 
	{
        tx_char(string[i]);    
        i++;
    }
}

//ISR for receiver
ISR(USART_RX_vect) {
    char receiver = UDR0;
    if (receiver == '@')
    {
        checker = 1;
        character = 0;
        convert_flag = 0;
    }
    else if (checker == 1 ) 
    {
        if ((receiver == '-' || receiver == '+') && character == 0) {
            arr[character] = receiver;
            character++;
        }
        else if ((receiver >= '0' && receiver <= '9')) 
        {
            arr[character] = receiver;
            character++;
        }
        else if (receiver == '#') 
        {
            if (character > 1) 
            {
                arr[character] = '\0';
                convert_flag = 1;
            }
            checker = 0;
        }
    }
}

