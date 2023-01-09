// using the atmega8l microcontroller
// Datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/atmel-2486-8-bit-avr-microcontroller-atmega8_l_datasheet.pdf
// dual brushed dc motor/stepper driver chip is TC78H621FNG
// Datasheet: https://toshiba.semicon-storage.com/info/TC78H621FNG_datasheet_en_20170410.pdf?did=57992&prodName=TC78H621FNG

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h> // i want to use bool
#define byte uint8_t // i like using byte, i can remeber what it is

// ------------ PINS --------------------------
// all pins for the motor H-brigde driver is connected to port C
// this is using
// PC0 - EN_B, PC3 - EN_A
// PC1 - PHA_B, PC4 - PHA_A
// PC2 - STBY, PC5 - LED
// motor B is not used
#define MPORT PORTC
#define MPORTD DDRC
#define EN_B 0b00000001
#define EN_A 0b00001000
#define PHA_B 0b00000010
#define PHA_A 0b00010000
#define STBY 0b00000100
#define LED 0b00100000

// Inputs
// PD6 - BUTTON (need pullup res)
// PD5 - HOME_SWITCH (need pullup res)
// PD0 - RXD, PD1 - TXD
#define BUTTON 0b01000000
#define HOME_SWITCH 0b00100000

// motor speed
// degs is calc with the motor speed of 40rpm
#define DELAY_START 100
#define DELAY_360D 1500
#define DELAY_180D 750
#define DELAY_120D 500
#define DELAY_EX1 125 // adjust the delay to account for the insersha of the turntable and gearbox ans stop speed
#define DELAY_EX2 150 // adjust the delay to account for the insersha
#define DELAY_EX3 100 // adjust the delay to account for the insersha
#define DELAY_240D 1000
#define DELAY_STOP 0 // not driving motor backware to stop it

// Set up serial
#define FOSC F_CPU // Clock Speed, 8MHz extranal
#define BAUD 9600
#define MYUBRR FOSC / 16 / BAUD - 1

// DDRx   - Used to configure the respective PORT as output/input
// PORTx  - Used to write the data to the Port pins
// PINx   - Used to Read the data from the port pins

// init serial func, right from the datasheet page 135
void USART_Init(unsigned int ubrr)
{
  /* Set baud rate */
  UBRRH = (unsigned char)(ubrr >> 8);
  UBRRL = (unsigned char)ubrr;
  /* Enable receiver and transmitter */
  UCSRB = (1 << RXEN) | (1 << TXEN);
  /* Set frame format: 8data, 1stop bit */
  UCSRC = (1 << URSEL) | /* (1 << USBS) |*/ (3 << UCSZ0);
}

// serial receive func, right from the datasheet page 139
unsigned char USART_Receive(void) // may change to return byte instead of char, makes things ez later
{
  /* Wait for data to be received */
  while (!(UCSRA & (1 << RXC)))
    ;
  /* Get and return received data from buffer */
  return UDR;
}

// serial printing, just for testing
void USART_Transmit(unsigned char data)
{
  /* Wait for empty transmit buffer */
  while (!(UCSRA & (1 << UDRE)))
    ;
  /* Put data into buffer, sends the data */
  UDR = data;
}

// this function rotoates the motor
// int delay - runs the motor for the number of ms, negitive numbers rotate backware
// bool home - when true the motor stops when it gets a home trigger, false will ignor the home trigger
// bool slow - when true the motor runs at 50% duly at 500 hz, false the motor runs normal.
void rotate(int delay, bool home, bool slow)
{
  // turn off motor
  if (delay == 0)
  {
    PORTC = 0;
    return;
  }

  if (delay > 0)
  {
    PORTC = (EN_A | STBY | LED);
    for (int i = 0; i < delay; i++)
    {
      _delay_ms(1); // delay to let motor run
      if (slow)
      {
        PORTC = PORTC ^ EN_A;
      }

      if (home)
      {
        if (!(PIND & (HOME_SWITCH))) // check to see if it is homes
        {
          PORTC = 0; // turn off motor
          return;    // when the HOME_SWITCH goes low (when it is pressed). it will break out of for loop
        }
      }
    }
  }
  else
  {
    PORTC = (EN_A | STBY | LED | PHA_A);
    for (int i = 0; i > delay; i--)
    {
      _delay_ms(1); // delay to let motor run
      if (slow)
      {
        PORTC = PORTC ^ EN_A;
      }
      if (home)
      {
        if (!(PIND & (HOME_SWITCH))) // check to see if it is homes
        {
          PORTC = 0;
          return; // when the HOME_SWITCH goes low (when it is pressed). it will break out of for loop
        }
      }
    }
  }
  PORTC = 0; // turn off the motor
  return;
}

int main()
{
  //  set motor pins as outputs
  DDRC = 0b00111111;
  // turn off all motor pins
  PORTC = 0b00000000;

  // enable inputs as inputs
  DDRD = 0b00000000;
  // enable pullup resistors
  PORTD = (BUTTON | HOME_SWITCH);

  // init serial
  USART_Init(MYUBRR);
  // rotate(-1,1,1);

  while (1)
  {

    switch (USART_Receive())
    {
    case '\n': // ingor termiator chars
    case '\r':
      break;
    case '1': // rotate to side 1
      USART_Transmit('1');
      if (!(PIND & (HOME_SWITCH))) // when home switch is low, that means it is homed
      {
        rotate(DELAY_180D, false, false); // rotate out of the way of the
      }
      rotate(DELAY_START + DELAY_360D, true, false); // find where home is
      rotate(DELAY_180D, false, false);              // move more to home a 2nd time
      rotate(DELAY_360D, true, false);               // find where home is
      rotate(DELAY_EX1, false, false);               // rotate to the right side
      // rotate(DELAY_STOP, false, false); // stop motor
      PORTC = 0; // turn off motor
      break;
    case '2': // rotate to side 2
      USART_Transmit('2');
      if (!(PIND & (HOME_SWITCH))) // when home switch is low, that means it is homed
      {
        rotate(DELAY_180D, false, false); // go backwares to get more persice position of the home
      }
      rotate(DELAY_START + DELAY_360D, true, false); // find where home is
      rotate(DELAY_180D, false, false);              // move more to home a 2nd time
      rotate(DELAY_360D, true, false);               // find where home is
      rotate(DELAY_120D + DELAY_EX2, false, false);  // move to the right side of the display
      // rotate(DELAY_STOP, false, false); // stop motor
      PORTC = 0; // turn off motor
      break;
    case '3': // rotate to side 3
      USART_Transmit('3');
      if (!(PIND & (HOME_SWITCH))) // when home switch is low, that means it is homed
      {
        rotate(DELAY_180D, false, false); // go backwares to get more persice position of the home
      }
      rotate(DELAY_START + DELAY_360D, true, false); // find where home is
      rotate(DELAY_180D, false, false);              // move more to home a 2nd time
      rotate(DELAY_360D, true, false);               // find where home is
      rotate(DELAY_240D + DELAY_EX3, false, false);  // move to the right side of the display
      // rotate(DELAY_STOP, false, false); // stop motor
      PORTC = 0; // turn off motor
      break;
    case '4': // rotate forward for 500ms
      USART_Transmit('4');
      rotate(500, false, false);
      PORTC = 0; // turn off motor
      break;
    case '5': // rotate backwares for 500ms
      USART_Transmit('5');
      rotate(-500, false, false);
      PORTC = 0; // turn off motor
      break;
    case '6': // rotate forward for 250ms
      USART_Transmit('6');
      rotate(250, false, false);
      PORTC = 0; // turn off motor
      break;
    case '7': // rotate backwares for 250ms
      USART_Transmit('7');
      rotate(-250, false, false);
      PORTC = 0; // turn off motor
      break;
    case '8':
      USART_Transmit('8');
      PORTC = 0; // turn off motor
      break;
    default:
      break;
    }
  }
}
