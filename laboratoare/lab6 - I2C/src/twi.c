/* twi.c - TWI library for ATMega324P */

#include "twi.h"

void twi_init(void) {
	/* Enable I2C power (board-specific) */
	DDRA |= (1 << PA6);
	PORTA |= (1 << PA6);

	/* reset control register */
	TWCR = 0;
	
    // TODO 0: Set bitrate (TWBR & TWSR!)

    // Initialize I2C to a 100KHz clock
    // TWI Status Register: initialize prescaler to 1
    TWSR = (0b00 << TWPS0);
    TWBR = 52; // (12000000/100000 - 16) / (1 * 2)

}

void twi_start(void) {    
    // Send START condition (using TWCR)
	TWCR = 0; /* reset it again, just to be sure */
	TWCR = (1 << TWINT) | (1 << TWEN)| (1 << TWSTA);
	// obligatory: wait for START condition to be sent
	while (!(TWCR & (1 << TWINT)));
}

void twi_write(uint8_t data) {
    TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	// TODO 1: wait for transfer to complete (TWINT flag)
   	while (!(TWCR & (1 << TWINT)));
}

void twi_read_ack(uint8_t *data) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

    while (!(TWCR & (1 << TWINT)));

    *data = TWDR;
}

void twi_read_nack(uint8_t *data) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    TWCR &= ~(1 << TWEA);
    while (!(TWCR & (1 << TWINT)));

    *data = TWDR;
}

void twi_stop(void) {
    // Send STOP condition
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
}

void twi_discover(void) {
    // TODO 2: Search for I2C slaves.
    // HINT: An acknowledged SLA_R should enable a flag in TWSR. Check the datasheet!


    for (uint8_t i = 0x00; i < 0x7F; i++)  {
        twi_start();
		// TODO: write address & check TWSR (see util/twi.h documentation for
		// constants!)
        twi_write((i << 1) | 1);
        if (((TWSR & 0xF8) == TW_MR_SLA_ACK))
            printf("Device discovered on 0x%x\n", i);
    }
    twi_stop();
}
