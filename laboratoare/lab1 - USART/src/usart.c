#include "usart.h"
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