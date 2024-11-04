#include <util/delay.h>
#include <usart.c>
#include <Arduino.h>

#define CLOCK_SPEED 12000000
#define BAUD 9600
#define MYUBRR CLOCK_SPEED/16/BAUD-1

#define MORSE_LUNG  3
#define MORSE_SCURT 1

/* Literele alfabetului in cod Morse */
char* morse_alphabet[] = {
    (char*)"13",    // a
    (char*)"3111",  // b
    (char*)"3131",  // c
    (char*)"311",   // d
    (char*)"1",     // e
    (char*)"1131",  // f
    (char*)"331",	// g
    (char*)"1111",	// h
    (char*)"11",	// i
    (char*)"1333",	// j
    (char*)"313",	// k
    (char*)"1311",	// l
    (char*)"33",	// m
    (char*)"31",	// n
    (char*)"333",	// o
    (char*)"1331",	// p
    (char*)"3313",	// q
    (char*)"131",	// r
    (char*)"111",	// s
    (char*)"3",	    // t
    (char*)"113",	// u
    (char*)"1113",	// v
    (char*)"133",	// w
    (char*)"3113",	// x
    (char*)"3133",	// y
    (char*)"3311"	// z
};

/* apelati functia pentru a obtine un ton lung/scurt,
 * folosind macro-urile MORSE_LUNG si MORSE_SCURT
 */
void speaker_morse(int tip)
{
    int delay = 1;
    int i;
    DDRD |= (1 << PD4);     // PD4 trebuie sa fie output

    for (i = 0; i < tip * 100 / delay; i++)
    {
        PIND = (1 << PD4);  // alternam starea iesirii
        _delay_ms(1);
    }
}

void port_init() {
    DDRB = 0xFFu;
    DDRC = 0xFFu;
    DDRD = 0xFFu;
    DDRB &= ~(1 << PB2); /// buton PB2

    PORTB = 0xFFu;
    PORTC = 0xFFu;
    PORTD = 0xFFu;
}

void morseCode(char buffer[], uint8_t bufferLength) {
    for(int i=0; buffer[i]; ++i) {
        if(buffer[i] - 'a' >= 0 && buffer[i] - 'z' < 26)
            for(int j=0; morse_alphabet[buffer[i] - 'a'][j]; ++j)
                speaker_morse(morse_alphabet[buffer[i] - 'a'][j] - '0'), _delay_ms(100);
        else 
            USART0_print("ATMega324P: Invalid character detected, skipping\n");
        _delay_ms(200);
    }
}

/*
int main() {
    port_init();
    USART0_init(MYUBRR);

    // ex 1
    char val = 255;
    while(1) {
        char val_ant = val;
        val = PINB & (1 << PB2);

        if(!val && val_ant) { /// buton apasat
            USART0_print("Butonul 1 a fost apasat !\n"); 
        }
        _delay_ms(100);
    }

   // ex2
   char commandBuffer[256] = "";
   uint8_t bufferLength = 0;

   while (1) {
        char caracter = USART0_receive();
        if(caracter != '\n') 
            commandBuffer[bufferLength++] = caracter, commandBuffer[bufferLength] = '\0';
        else 
            USART0_exec(commandBuffer), bufferLength = 0, commandBuffer[0] = '\0';
   }
    
 
   //  ex3 
    char buffer[256] = "";
    uint8_t bufferLength = 0;

    while(1) {
        char caracter = USART0_receive();
        if(caracter != '\n')
            buffer[bufferLength++] = caracter, buffer[bufferLength] = '\0';
        else 
            morseCode(buffer, bufferLength), bufferLength = 0, buffer[0] = '\0';
    }

    */
    /// bonus
    /*
    char commandBuffer[256] = "";
    uint8_t bufferLength = 0;

    DDRB |= (1 << PB3);
    DDRD |= (1 << PD5) | (1 << PD7);

    PORTB |= (1 << PB3);
    PORTD |= (1 << PD7);
    PORTD &= ~(1 << PD5);

    while (1) {
        char caracter = USART0_receive();
        if(caracter != '\n') 
            commandBuffer[bufferLength++] = caracter, commandBuffer[bufferLength] = '\0';
        else 
            USART0_semafor(commandBuffer), bufferLength = 0, commandBuffer[0] = '\0';
    }
    
    return 0;
} 
*/


/*
/// ex1 arduino
int32_t val_ant = 0xff, val = 0xff;
   void setup() {
        Serial.begin(9600);
        pinMode(2, INPUT_PULLUP);
   }

   void loop() {
        val_ant = val;
        val = digitalRead(2);
        if(!val && val_ant) {
            USART0_print("Butonul 1 a fost apasat !\n"); 
        }
        delay(100);
   }
*/


/// ex2 arduino
/*
char commandBuffer[256] = "";
uint8_t bufferLength = 0;

void setup() {
    Serial.begin(9600);
}

void loop() {
    while(Serial.available() == 0)
        ; /// wait for chr
    
    char caracter = (char)Serial.read();
    if(caracter != '\n') 
        commandBuffer[bufferLength++] = caracter, commandBuffer[bufferLength] = '\0';
    else 
        USART0_exec(commandBuffer), bufferLength = 0, commandBuffer[0] = '\0';

}*/

/// ex3 arduino

char buffer[256] = "";
uint8_t bufferLength = 0;

void setup() {
    Serial.begin(9600);
}

void loop() {
    while(Serial.available() == 0)
        ; /// wait for chr

    char caracter = (char)Serial.read();

    if(caracter != '\n')
        buffer[bufferLength++] = caracter, buffer[bufferLength] = '\0';
    else 
        morseCode(buffer, bufferLength), bufferLength = 0, buffer[0] = '\0';
}