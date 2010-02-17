#include <avr/io.h>
#include <util/delay.h>

void busyloop( void ) {
  while(1) {
    for( int y = 0; y < 8; y++ ) {
      for( int x = 0; x < 8; x++ ) {

        PORTB = 0x01 << x;
        PORTD = ~( 0x01 << y);
        _delay_ms(25);
      }
    }
  }
}

int main(void) {
  char i;

  DDRB = 0xff;
  DDRD = 0xff;

  busyloop();
  return 1;
}

