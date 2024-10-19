// #include <avr/io.h>
// #include <util/delay.h>
 
// int main() {
// 	bool blinking = false;
// 	int led = 0;

// 	DDRD |= (1 << PD7) | (1 << PD5);
// 	DDRB |= (1 << PB3);
// 	DDRB &= ~(1 << PB2);
// 	DDRD &= ~(1 << PD6); 

// 	PORTB |= (1 << PB2); /// rezistenta de pullup
// 	PORTD |= (1 << PD6);

// 	PORTD &= ~(1 << PD7);
// 	PORTD |= (1 << PD5);
// 	PORTB |= (1 << PB3);

// 	while(1) {
// 		if(!(PINB & (1 << PB2))) {
// 			if(led == 0) {
// 				PORTD |= (1 << PD7);
// 				PORTD &= ~(1 << PD5);
// 				PORTB |= (1 << PB3);
// 			}
// 			else {
// 				if(led == 1) {
// 					PORTD |= (1 << PD5);
// 					PORTB &= ~(1 << PB3);
// 					PORTD |= (1 << PD7);
// 				}
// 				else {
// 					PORTB |= (1 << PB3);
// 					PORTD &= ~(1 << PD7);
// 					PORTD |= (1 << PD5);
// 				}
// 			}
// 			++led;
// 			led %= 3;
// 		}

// 		if(!(PIND & (1 << PD6))) {
// 			blinking = !blinking;
// 			if(!blinking)
// 				switch(led) {
// 					case 0:
// 						PORTD &= ~(1 << PD7);
// 						break;
// 					case 1:
// 						PORTD &= ~(1 << PD5);
// 						break;
// 					case 2:
// 						PORTB &= ~(1 << PB3);
// 						break;
// 				}
// 		}

// 		if(blinking && (PINB & (1 << PB2))) {
// 			switch(led) {
// 				case 0:
// 					PORTD ^= (1 << PD7);
// 					break;
// 				case 1:
// 					PORTD ^= (1 << PD5);
// 					break;
// 				case 2:
// 					PORTB ^= (1 << PB3);
// 					break;
// 			}
// 		}
//                 _delay_ms(150);
// 	}
 
// 	return 0;
// }

#include <Arduino.h>
#define RED_LED 13
#define GREEN_LED 15
#define BLUE_LED 3
#define BTN1 2
#define BTN2 14

bool blinking = false;
int led = 0;

void setup() {
	pinMode(BTN1, INPUT_PULLUP);
	pinMode(BTN2, INPUT_PULLUP);
	pinMode(RED_LED, OUTPUT);
	pinMode(GREEN_LED, OUTPUT);
	pinMode(BLUE_LED, OUTPUT);
	digitalWrite(GREEN_LED, 0);
	digitalWrite(RED_LED, 1);
	digitalWrite(BLUE_LED, 1);
}

void loop() {
	if(digitalRead(BTN1) == 0) {
		blinking = !blinking;
		if(!blinking)
			switch(led) {
				case 0:
					digitalWrite(GREEN_LED, 0);
					digitalWrite(RED_LED, 1);
					digitalWrite(BLUE_LED, 1);
					break;
				case 1:
					digitalWrite(GREEN_LED, 1);
					digitalWrite(RED_LED, 0);
					digitalWrite(BLUE_LED, 1);
					break;
				case 2:
					digitalWrite(GREEN_LED, 1);
					digitalWrite(RED_LED, 1);
					digitalWrite(BLUE_LED, 0);
					break;
			}
	}

	if(digitalRead(BTN2) == 0) {
		led = (led+1)%3;
		switch(led) {
			case 0:
				digitalWrite(GREEN_LED, 0);
				digitalWrite(RED_LED, 1);
				digitalWrite(BLUE_LED, 1);
				break;
			case 1:
				digitalWrite(GREEN_LED, 1);
				digitalWrite(RED_LED, 0);
				digitalWrite(BLUE_LED, 1);
				break;
			case 2:
				digitalWrite(GREEN_LED, 1);
				digitalWrite(RED_LED, 1);
				digitalWrite(BLUE_LED, 0);
				break;
		}
	}

	if(blinking) {
		switch(led) {
			case 0:
				digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
				break;
			case 1:
				digitalWrite(RED_LED, !digitalRead(RED_LED));
				break;
			case 2:
				digitalWrite(BLUE_LED, !digitalRead(BLUE_LED));
				break;
		}
	}

	delay(150);
}