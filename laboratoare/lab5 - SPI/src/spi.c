#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"

#define SPE 6
#define MSTR 4
#define SPR0 0
#define PIN_SS PB4

void SPI_init(){

	/* TODO1: set MOSI and SCK output, all others input */
	SPI_DDR = (1 << SPI_MOSI) | (1 << SPI_SCK);
	
	/* TODO1: set SS PIN as output and set it on HIGH */
	DDRB |= (1 << PIN_SS);

	/* TODO1: enable SPI, set it as Master, set clock rate at fosc/16 */
	SPCR0 |= (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

uint8_t SPI_exchange(uint8_t data){
 	SPDR0 = data;
	while(!(SPSR0 & (1<<SPIF0)))
		;
	return SPDR0;
}

