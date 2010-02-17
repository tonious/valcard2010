#include <avr/io.h>
#include <util/delay.h>

#include "font.h"

void putchar( unsigned char c ) {

  for( int y = 0; y < 8; y++ ) {
    unsigned char in = pgm_read_byte( font + (c - ' ')*8 + y);
    unsigned char out = 0x00;

    for( int i = 0; i < ( sizeof( in ) * 8 ); i++ ) {
      if( in & ( 0x01 << i ) )
        out |= 0x80 >> i;
    }
    
    PORTB = out;
    PORTD = ~( 0x01 << y);

    _delay_ms(1);
  }
}

void busyloop( void ) {
  char letter = 'a';
  int count = 0;

  while(1) {
    putchar( letter );

    count++;
    if( count > 100) {
      count = 0;
      letter++;
      if( letter > 'z' ) {
        letter = 'a';
      }
    }

  }

}

int main(void) {

  DDRB = 0xff;
  DDRD = 0xff;

  busyloop();
  return 1;
}

