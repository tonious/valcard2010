#include <stdint.h>
#include <stdio.h>

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "ledmatrix.h"

FILE led_str = FDEV_SETUP_STREAM( led_putchar, NULL, _FDEV_SETUP_WRITE );

ISR( TIMER0_COMPA_vect ) {
  led_interrupt();
}

ISR( PCINT1_vect ) {
  ;
/*
  stdout = &led_str;
  if( !( PINC & _BV( PC0 ) ) ) {
    led_purge();
    printf( "Interrupt." );
  }   
 */
}

int main( void ) {
  TCCR0A = _BV( WGM01 ); // CTC mode.
  TCCR0B = _BV( CS01 ) | _BV( CS00 ); // /256 prescaler.
  OCR0A = 121; // F_CPU / (256 * 256) - 1
  TIMSK0 = _BV( OCIE0A ); // Enable interrupt on counter match.

  DDRC &= ~_BV( PC0 ); // Set pin PC0 of DDRC to 0
  PORTC |= _BV( PC0 ); // Enable pull up resistor.

  // Enable pin change interrupt.
  PCICR = _BV( PCIE1 );
  PCMSK1 = _BV( PCINT8 );
  sei();


  // Init LED matrix.
  led_init();
  stdout = &led_str;

  printf( "Boot. " );

  do { 
    _delay_ms( 10 );
  } while( led_working() );


  while( 1 ) {
	printf( "Sleep. " );

	do { 
	  _delay_ms( 10 );
        } while( led_working() );

  	// Set up sleep mode.
  	set_sleep_mode( SLEEP_MODE_PWR_DOWN );

	// Go to sleep.
        cli();
	PORTB = 0x00;
	PORTD = 0x00;
        sleep_enable();
//        sleep_bod_disable();
        sei();
        sleep_cpu();
        sleep_disable();
        sei();

	printf( "Awake. " );
	_delay_ms( 50);
  }
}

