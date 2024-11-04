#include <Arduino.h>
#include <avr/interrupt.h>
#include <usart.h>

#define CLOCK_SPEED 12000000
#define BAUD 9600
#define MYUBRR CLOCK_SPEED/16/BAUD-1

#define CLOCK_SPEED 12000000L
#define BAUD 9600
#define MYUBRR CLOCK_SPEED / 16 / BAUD - 1
 
#define PRESCALE 256
#define NR_CC_ONE_SEC CLOCK_SPEED / PRESCALE
#define MAX_TIMER_VALUE (1UL << 16) - 1
#define NORMAL_MODE 0
#define CTC_MODE 4
#define TIMER_MODE CTC_MODE


/* ex 0.1 / 0.2
ISR(PCINT1_vect) {
   ex1
  if((PINB & (1 << PB2)) == 0) {
    PORTD ^= (1 << PD7);
  }
}*/

/*ex1
int buzzerSetting = 0;
int freq[] = {300, 700, 1000};


ISR(PCINT1_vect) {

  if((PINB & (1 << PB2)) == 0) {
    buzzerSetting = (buzzerSetting + 1) % 3;
    char freqstr[256];
    itoa(freq[buzzerSetting], freqstr, 10);
    USART0_print(freqstr);
  }
}

int main() {
  USART0_init(MYUBRR);
  DDRD |= (1 << PD7); /// GREEN OUTPUT
  DDRD |= (1 << PD5); /// RED
  DDRB |= (1 << PB3); /// BLUE

  PORTD &= ~(1 << PD7);
  PORTD |= (1 << PD5);
  PORTB |= (1 << PB3);


  DDRB &= ~(1 << PB2);
  PORTB |= (1 << PB2);

  DDRD &= ~(1 << PD6);
  PORTD |= (1 << PD6);

  DDRD |= (1 << PD4);

  PCICR |= (1 << PCIE1);
  PCMSK1 |= (1 << PCINT10);

  sei();
  while(true)
    ;
  return 0;
}*/

/// ex 2.1


uint32_t nr_milisec = 0;

uint32_t millis_homemade() {
  return nr_milisec;
}

ISR(TIMER1_COMPA_vect) {
  uint32_t blinkTimeMS = 2 * 1000;
  nr_milisec += 1000;
  if(nr_milisec >= blinkTimeMS) {
    PORTD ^= (1 << PD7);
    nr_milisec %= blinkTimeMS; 
  }
}

void set_prescale(int prescale) {
	if (NR_CC_ONE_SEC > MAX_TIMER_VALUE) {
		USART0_print("Invalid prescale value\n");
		return;
	}

	switch (prescale) {
		case 1:
			// TODO
			USART0_print("Prescaler set to 1\n");
			break;
		
		case 8:
			// TODO
			USART0_print("Prescaler set to 8\n");
			break;
		
		case 64:
			// TODO
			USART0_print("Prescaler set to 64\n");
			break;
		
		case 256:
			TCCR1B |= (1 << CS12);
			USART0_print("Prescaler set to 256\n");
			break;
		
		case 1024:
			// TODO
			USART0_print("Prescaler set to 1024\n");
			break;
		
		default:
			USART0_print("Invalid switch case\n");
			break;
	}
}

void set_mode(int mode) {
	switch (mode) {
		case NORMAL_MODE:
			// TODO

      // all already 0
			USART0_print("Timer mode set to NORMAL\n");
			break;

		case CTC_MODE:
			TCCR1B |= (1 << WGM12);
			USART0_print("Timer mode set to CTC\n");
			break;
		
		default:
			USART0_print("Invalid switch case\n");
			break;
	}
}

void set_max_value(int value) {
	ICR1 = value;
}

void timer_init() {

	set_mode(TIMER_MODE);

	set_prescale(PRESCALE);

	if (TIMER_MODE == CTC_MODE) {
		set_max_value(NR_CC_ONE_SEC);
	}

	sei();

	// enable OCIE1A interrupt
	TIMSK1 |= (1 << OCIE1A);
}

int main() {
  DDRD |= (1 << PD7);
  timer_init();
  while (true)
  {
    
  }
  
  return 0;
}

#include <string.h>
#include <util/delay.h>

/*
 * Functie de initializare a controllerului USART
 */
void USART0_init(unsigned int ubrr)
{
    /* seteaza baud rate-ul */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;

    /* porneste transmitatorul */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);

    /* seteaza formatul frame-ului: 8 biti de date, 2 biti de stop, fara paritate */
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}


/*
 * Functie ce transmite un caracter prin USART
 *
 * @param data - caracterul de transmis
 */
void USART0_transmit(char data)
{
    /* asteapta pana bufferul e gol */
    while(!(UCSR0A & (1<<UDRE0)));

    /* pune datele in buffer; transmisia va porni automat in urma scrierii */
    UDR0 = data;
}

/*
 * Functie ce primeste un caracter prin USART
 *
 * @return - caracterul primit
 */
char USART0_receive()
{
    /* asteapta cat timp bufferul e gol */
    while(!(UCSR0A & (1<<RXC0)));

    /* returneaza datele din buffer */
    return UDR0;
}

/*
 * Functie ce transmite un sir de caractere prin USART
 *
 * @param data - sirul (terminat cu '\0') de transmis
 */
void USART0_print(const char *data)
{
    while(*data != '\0')
        USART0_transmit(*data++);
}

void USART0_exec(const char command[]) {
    DDRB |= (1 << PB3);
    DDRD |= (1 << PD5) | (1 << PD7);
    if(strcmp(command, "on") == 0) {
        PORTB &= ~(1 << PB3);
        PORTD &= ~(1 << PD5);
        PORTD &= ~(1 << PD7);

        return;
    }

    if(strcmp(command, "off") == 0) {
        PORTB |= (1 << PB3);
        PORTD |= (1 << PD5);
        PORTD |= (1 << PD7);
                
        return;
    }

    if(strcmp(command, "red") == 0) {
        PORTB |= (1 << PB3);
        PORTD |= (1 << PD7);
        PORTD &= ~(1 << PD5);
                
        return;
    }

    if(strcmp(command, "green") == 0) {
        PORTB |= (1 << PB3);
        PORTD &= ~(1 << PD7);
        PORTD |= (1 << PD5);
                
        return;
    }

    if(strcmp(command, "blue") == 0) {
        PORTB &= ~(1 << PB3);
        PORTD |= (1 << PD7);
        PORTD |= (1 << PD5);
                
        return;
    }

    USART0_print("ATMega324P: Comanda invalida!\n");
}

void USART0_semafor(const char command[]) {
    if(strcmp(command, "pieton") == 0) {
        PORTD &= ~(1 << PD7);
        _delay_ms(2000);
        PORTD |= (1 << PD5);
        _delay_ms(5000);
        PORTD &= ~(1 << PD5);
        PORTD |= (1 << PD7);
        return;
    }
    USART0_print("ATMega324P: Comanda invalida!\n");
}