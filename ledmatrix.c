#include <stdint.h>
#include <stdio.h>

#include <util/delay.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "font.h"
#include "ledmatrix.h"

volatile unsigned char led_buffer[ LED_BUFFLENGTH ];
volatile unsigned char *led_read;
volatile unsigned char *led_write;
volatile unsigned char led_curr_char;
volatile unsigned char led_next_char;
volatile int led_buffsize;
volatile int led_delay;
volatile int led_atwork;
volatile int led_scroll;
volatile int led_line;

void led_init(void) {
  DDRB = 0xff;
  DDRD = 0xff;

  led_read = led_buffer;
  led_write = led_buffer;
  led_buffsize = 0;
  led_atwork = 0;
  led_line = 0;
  led_scroll = 0;

  led_curr_char = '\0';
  led_next_char = '\0';
}

int led_working( void ) {
  return( led_atwork == 1 );
}

void led_purge( void ) {
  cli();

  led_read = led_buffer;
  led_write = led_buffer;
  led_buffsize = 0;
  led_scroll = 0;
  led_line = 0;
  led_delay = 0;

  led_curr_char = '\0';
  led_next_char = '\0';

  sei();
}

int reverse( int in ) {
  unsigned char out = 0x00;
  for( int i = 0; i < 8; i++ ) {
    if( in & ( 0x01 << i ) ) out |= 0x80 >> i;
  }
  return( out );
}

void led_interrupt( void ) {
  unsigned char curr_line, next_line;

  curr_line = pgm_read_byte( font + (led_curr_char - ' ')*8 + led_line);
  next_line = pgm_read_byte( font + (led_next_char - ' ')*8 + led_line);
  
  curr_line = led_curr_char == '\0' ? 0x00 : curr_line; 
  next_line = led_next_char == '\0' ? 0x00 : next_line; 

  PORTB = reverse( curr_line << led_scroll | next_line >> ( 8 - led_scroll ) );
  PORTD = ~( 0x01 << led_line );
 
  if( ++led_line > 7 ) led_line = 0;

  if( ++led_delay > LED_DELAY || led_curr_char == '\0' ) {
    led_delay = 0;

    if( ++led_scroll > 7 || led_curr_char == '\0' ) {
      led_scroll = 0;
      led_curr_char = led_next_char;
      led_next_char = led_unputchar();
    }
  } 
}

unsigned char led_unputchar() {
  if( led_buffsize == 0 )
    return( '\0' );

  unsigned char nextchar = *led_read;
  led_buffsize--;

  // Loop the ring if needed.
  if( ++led_read >= led_buffer + LED_BUFFLENGTH  )
    led_read = led_buffer;

  if( nextchar == '\0' && led_curr_char == '\0' && led_next_char == '\0' ) {
    led_atwork = 0;
  } else {
    led_atwork = 1;
  }

  return( nextchar );
}

int led_putchar( unsigned char c, FILE *unused ) {
  while( led_buffsize > (LED_BUFFLENGTH - 1) ) {
    _delay_ms( 10 );
  }

  cli();
  led_atwork = 1;

  *led_write = c;
  led_buffsize++;

  // Loop the ring if needed.
  if( ++led_write > led_buffer + LED_BUFFLENGTH )
    led_write = led_buffer;

  sei();
  return( 0 );
}

