#define LED_BUFFLENGTH 64
#define LED_DELAY 48

void led_init( void );
void led_interrupt( void );
int  led_putchar( unsigned char c, FILE *unused );
unsigned char led_unputchar();
void led_purge( void );
int  led_working( void );

