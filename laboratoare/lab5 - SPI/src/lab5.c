#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "usart.h"
#include "lcd.h"
#include "pff.h"

#define PM_BAUD 9600



FATFS fs;					// sistemul de fisiere

/*---------------------------------------------------------------------------*/
/* Player audio                                                              */
/*---------------------------------------------------------------------------*/

/*
 * Four-Character Code - folosit pentru a indentifica formate de date
 */
#define FCC(c1, c2, c3, c4) \
	(((DWORD)(c4) << 24) + \
	 ((DWORD)(c3) << 16) + \
	 (( WORD)(c2) <<  8) + \
	 (( BYTE)(c1) <<  0))


uint8_t	buf[2][256];	// wave output buffers (double buffering)
const	 uint16_t	buf_size = 256;	// front and back buffer sizes
volatile uint8_t	buf_front = 0;	// front buffer index (current buffer used)
volatile uint8_t	buf_pos = 0;	// current buffer position
volatile uint8_t	buf_sync = 0;

#define BUF_FRONT	(buf[buf_front])
#define BUF_BACK	(buf[1 - buf_front])


ISR(TIMER0_COMPA_vect)
{
	OCR1B = BUF_FRONT[buf_pos++];

	// swap buffers when end is reached (end is 256 <=> overflow to 0)
	if(buf_pos == 0)
		buf_front = 1 - buf_front;
}

void timer0_start(void)
{
	// interrupt on compare A
	TIMSK0 |= (1 << OCIE0A);
	// CTC, top OCRA
	TCCR0B |= (0 << WGM02);
	TCCR0A |= (1 << WGM01) | (0 << WGM00);
	// prescaler 8
	TCCR0B |= (2 << CS00);
}

void timer0_stop(void)
{
	TCCR0B = 0;
	TCCR0A = 0;
	TIMSK0 = 0;
	OCR0A = 0;
	TCNT0 = 0;
}

void timer1_start(void)
{
	// 8-bit FastPWM
	TCCR1B |= (1 << WGM12);
	TCCR1A |= (1 << WGM10);
	// channel B inverted
	TCCR1A |= (1 << COM1B0) | (1 << COM1B1);
	// prescaler 1
	TCCR1B |= (1 << CS10);
}

void timer1_stop(void)
{
	TCCR1B = 0;
	TCCR1A = 0;
	OCR1B = 0;
	TCNT1 = 0;
}

bool continue_play()
{
	if((PINB & (1 << PB2)) == 0 && (PIND & (1 << PD6)) == 0)
		return false;

	return true;
}

/*
 * Incarca header-ul unui fisier WAVE
 *
 * @return DWORD
 * 	0 => format invalid
 * 	1 => eroare I/O
 * 	>1 => numarul de sample-uri
 */
DWORD load_header(void)
{
	DWORD size;
	WORD ret;

	// citeste header-ul (12 octeti)
	if(pf_read(BUF_FRONT, 12, &ret))
		return 1;

	if(ret != 12 || LD_DWORD(BUF_FRONT + 8) != FCC('W','A','V','E'))
		return 0;

	for(;;)
	{
		// citeste chunk ID si size
		pf_read(BUF_FRONT, 8, &ret);
		if(ret != 8)
			return 0;

		size = LD_DWORD(&BUF_FRONT[4]);

		// verifica FCC
		switch(LD_DWORD(&BUF_FRONT[0]))
		{
			// 'fmt ' chunk
			case FCC('f','m','t',' '):
				// verifica size
				if(size > 100 || size < 16) return 0;

				// citeste continutul
				pf_read(BUF_FRONT, size, &ret);
				// verifica codificarea
				if(ret != size || BUF_FRONT[0] != 1) return 0;
				// verifica numarul de canale
				if(BUF_FRONT[2] != 1 && BUF_FRONT[2] != 2) return 0;
				// verifica rezolutia
				if(BUF_FRONT[14] != 8 && BUF_FRONT[14] != 16) return 0;

				// seteaza sampling rate-ul
				OCR0A = (BYTE)(F_CPU / 8 / LD_WORD(&BUF_FRONT[4])) - 1;
				break;

			// 'data' chunk => incepe redarea
			case FCC('d','a','t','a'):
				return size;

			// 'LIST' chunk => skip
			case FCC('L','I','S','T'):
			// 'fact' chunk => skip
			case FCC('f','a','c','t'):
				pf_lseek(fs.fptr + size);
				break;

			// chunk necunoscut => eroare
			default:
				return 0;
		}
	}

	return 0;
}

/*
 * Functie care reda un fisier audio
 *
 * path - calea absoluta a fisierului
 *
 * @return UINT
 *	FR_OK daca a rulat cu succes fisierul
 */
UINT play(const char *path)
{
	FRESULT ret;

	if((ret = pf_open(path)) == FR_OK)
	{
		WORD bytes_read;

		// incarca header-ul fisierului
		DWORD current_size = load_header();
		if(current_size < buf_size)
			return FR_NO_FILE;

		// align to sector boundary
		ret = pf_lseek((fs.fptr + 511) & ~511);
		if(ret != FR_OK)
			return ret;

		// fill front buffer
		ret = pf_read(BUF_FRONT, buf_size, &bytes_read);
		if(ret != FR_OK)
			return ret;
		if(bytes_read < buf_size)
			return ret;

		// reset front buffer index
		buf_pos = 0;

		// start output
		timer0_start();
		timer1_start();
		DDRD |= (1 << PD4);

		while(continue_play())
		{
			uint8_t old_buf_front = buf_front;
			
			// fill back buffer
			ret = pf_read(BUF_BACK, buf_size, &bytes_read);
			if(ret != FR_OK)
				break;
			if(bytes_read < buf_size)
				break;

			// wait for buffer swap
			while(old_buf_front == buf_front) ;
		}

		// stop output
		DDRD &= ~(1 << PD4);
		timer1_stop();
		timer0_stop();
	}

	return ret;
}


/*---------------------------------------------------------------------------*/
/* Ceas                                                                      */
/*---------------------------------------------------------------------------*/

volatile uint8_t hours = 0;
volatile uint8_t minutes = 0;
volatile uint8_t seconds = 0;


ISR(TIMER2_COMPA_vect)
{
	static uint16_t miliseconds = 0;

	if(++miliseconds != 1000)
		return;
	miliseconds = 0;

	if(++seconds == 60)
	{
		seconds = 0;

		if(++minutes == 60)
		{
			minutes = 0;

			if(++hours == 24)
				hours = 0;
		}
	}

	char buf[9];
	snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hours, minutes, seconds);

	LCD_clear_bottom_line();
	LCD_printAt(0x40, buf);
}

void timer2_init(void)
{
	// set compare at each milisecond
	OCR2A = 124;
	// interrupt on compare A
	TIMSK2 |= (1 << OCIE2A);
	// CTC, top OCRA
	TCCR2A |= (1 << WGM21);
	// prescaler 128
	TCCR2B |= (1 << CS20) | (1 << CS22);
}


DIR directory;
FILINFO file;

void init_directory() {
	/* TODO3: open `/music`, using the `directory` structure declared above */
	pf_opendir(&directory, "/music");
	/* TODO3: find the first file whose name doesn't start with a `_`; use the `directory` and `file` structures declared above */
	do
		pf_readdir(&directory, &file);
	while(file.fname[0] == '_');

	/* TODO3: display the name of the first file on the LCD */
	LCD_clear_top_line();
	LCD_printAt(0x00, file.fname);
}

void next_file() {
	/* TODO4: go to the next file whose name doesn't start with a `_`; after the last file, go back to the first file*/
	do {
		if(file.fname[0] == '\0')
			pf_opendir(&directory, "/music");
		pf_readdir(&directory, &file);
	}
	while(file.fname[0] == '\0' || file.fname[0] == '_');

	LCD_clear_top_line();
	LCD_printAt(0x00, file.fname);
}

void buttons_init() {
	/* TODO4: configure button PB2 */
	DDRB &= ~(1 << PB2);
	PORTB |= (1 << PB2);
	/* TODO5: configure button PD6 */
	DDRD &= ~(1 << PD6);
	PORTD |= (1 << PD6);
}

char music[32] = "";

int main (void)
{
	char path[200];

	buttons_init();
	LCD_init();
	timer2_init();
	USART0_init(CALC_USART_UBRR(PM_BAUD));
  	USART0_use_stdio();



	sei();

	for(;;)
	{
		// mount filesystem
		LCD_printAt(0x00, "mounting...");

		int mount = pf_mount(&fs);
		if(mount != FR_OK)
		{
			// wait a while and retry
			_delay_ms(1000);
			continue;
		}

		LCD_clear_top_line();
		LCD_printAt(0x00, "mounted!");
		_delay_ms(2000);
		LCD_clear_top_line();

		init_directory();

		for(;;)
		{
			/* TODO4: go to next file when button PB2 is pressed */
			if(!(PINB & (1 << PB2)))
				next_file();
			/* TODO5: play current file on the speaker when button PD6 is pressed */
			if (!(PIND & (1 << PD6))) {
				strcpy(music, "/music/");
				play(strcat(music, file.fname));
			}
			_delay_ms(250);
		}
	}

	return 0;
}
