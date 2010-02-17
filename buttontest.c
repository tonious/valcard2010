#include <stdint.h>
#include <stdio.h>

#include <util/delay.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "ledmatrix.h"

ISR( TIMER0_COMPA_vect ) {
  led_interrupt();
}

int main( void ) {
  TCCR0A = _BV( WGM01 ); // CTC mode.
  TCCR0B = _BV( CS01 ) | _BV( CS00 ); // /256 prescaler.
  OCR0A = 121; // F_CPU / (256 * 256) - 1
  TIMSK0 = _BV( OCIE0A ); // Enable interrupt on counter match.

  DDRC &= ~_BV( PC0 ); // Set pin PC0 of DDRC to 0
  PORTC |= _BV( PC0 ); // Enable pull up resistor.

  led_init();
  sei();

  FILE led_str = FDEV_SETUP_STREAM( led_putchar, NULL, _FDEV_SETUP_WRITE );
  stdout = &led_str;

  while( 1 ) {
    do {
      _delay_ms( 10 );
    } while( ( PINC & _BV( PC0 ) ) );

    led_purge();
    printf( " * " );

    do {
      _delay_ms( 10 );
    } while( !( PINC & _BV( PC0 ) ) );
  }
}

