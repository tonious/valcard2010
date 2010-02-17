#include <stdint.h>
#include <stdio.h>

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "ledmatrix.h"

FILE led_str = FDEV_SETUP_STREAM( led_putchar, NULL, _FDEV_SETUP_WRITE );

#define NUM_NOTHINGS 28

// In GCC, PROGMEM won't carry over to individual strings if declared
// as *char[].  So declare the strings first, then stuff them into
// an array.
unsigned char mesg00[] PROGMEM = "To my Sheena:";
unsigned char mesg01[] PROGMEM = "Happy Valentines Day";
unsigned char mesg02[] PROGMEM = "Love, Tony.";
unsigned char mesg03[] PROGMEM = "I \x7f my Sheena";
unsigned char mesg04[] PROGMEM = "How do I \x7f my Sheena?";
unsigned char mesg05[] PROGMEM = "Let me count the ways...";
unsigned char mesg06[] PROGMEM = "1. 2. 3.";
unsigned char mesg07[] PROGMEM = "45. 46. 47.";
unsigned char mesg08[] PROGMEM = "13451. 13452. 13453.";
unsigned char mesg09[] PROGMEM = "I \x7f hugging my Sheena";
unsigned char mesg10[] PROGMEM = "I \x7f kissing my Sheena";
unsigned char mesg11[] PROGMEM = "I \x7f\x7f\x7f my Sheena";
unsigned char mesg12[] PROGMEM = "\"\x7f\"";
unsigned char mesg13[] PROGMEM = "My Sheena is my favourite Sheena";
unsigned char mesg14[] PROGMEM = "I still \x7f my Sheena";
unsigned char mesg15[] PROGMEM = "My Sheena is the bestest Sheena";
unsigned char mesg16[] PROGMEM = "--,--'-{@   @}-,-`--";
unsigned char mesg17[] PROGMEM = "Roses are red";
unsigned char mesg18[] PROGMEM = "Violets are blue";
unsigned char mesg19[] PROGMEM = "I \x7f my Sheena";
unsigned char mesg20[] PROGMEM = "And she's pretty, too";
unsigned char mesg21[] PROGMEM = "\x7f\x7f\x7f\x7f\x7f";
unsigned char mesg22[] PROGMEM = "Roses are 0xff0000";
unsigned char mesg23[] PROGMEM = "Violets are 0x0000ff";
unsigned char mesg24[] PROGMEM = "All my base...";
unsigned char mesg25[] PROGMEM = "Are belong to you";
unsigned char mesg26[] PROGMEM = "\x7f \x7f \x7f \x7f \x7f";


unsigned char *sweet_nothings[] PROGMEM = {
	mesg00, mesg01, mesg02, mesg03, mesg21,
	mesg04, mesg05, mesg06, mesg07,
	mesg08, mesg09, mesg10, mesg11,
	mesg12, mesg13, mesg14, mesg15,
	mesg16, mesg17, mesg18, mesg19,
	mesg20, mesg21, mesg22, mesg23,
	mesg24, mesg25, mesg26
};

ISR( TIMER0_COMPA_vect ) {
  led_interrupt();
}

ISR( PCINT1_vect ) {
  ;
}


int main( void ) {
  // Set up refresh interrupt.
  TCCR0A = _BV( WGM01 ); // CTC mode.
  TCCR0B = _BV( CS01 ) | _BV( CS00 ); // /256 prescaler.
  OCR0A = 121; // F_CPU / (256 * 256) - 1
  TIMSK0 = _BV( OCIE0A ); // Enable interrupt on counter match.

  // Set up button.
  DDRC &= ~_BV( PC0 ); // Set pin PC0 of DDRC to 0
  PORTC |= _BV( PC0 ); // Enable pull up resistor.

  // Set sleep mode.
  set_sleep_mode( SLEEP_MODE_PWR_DOWN );

  // Init LED matrix.
  led_init();
  stdout = &led_str;

  int index = 0;
  int just_woke = 0;
  _delay_ms( 1 );

  while( 1 ) {
    // Prepare for sleep.
    cli();
    PCICR = _BV( PCIE1 );
    PCMSK1 = _BV( PCINT8 );
    PORTB = 0x00; PORTD = 0x00;
    sei();
    sleep_enable();

    // Sleep here.
    sleep_cpu();

    // Aaaand we just woke up.
    sleep_disable();
    PCMSK1 = 0;
    sei();
    just_woke = 1;

    // Display the current message.
    printf( " " );
    printf_P( (PGM_P)pgm_read_word(&sweet_nothings[ index ]) );
    printf( " " );

    // Advance to the next message.
    if( ++index >= NUM_NOTHINGS ) index = 0;

    // Wait until the display is finished.
    do { 
      if( ( PINC & _BV( PC0 ) ) ) {
        just_woke = 0;
      }

      if( !( PINC & _BV( PC0 ) ) && just_woke == 0 ) {
        led_purge();
	break;
      }
     
      _delay_ms( 50 );
    } while( led_working() );
  }
}

