// #include <stdio.h>
// #include <stdlib.h>
// #include <math.h>
// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>

// #include "usart.h"
// #include "timers.h"
// #include "colors.h"

// /* Configuration constants: */
// #define PM_BAUD 28800

// #define RGB_PULSE  1
// #define RGB_HSV    2

// #define LED_ANIMATION  RGB_HSV
// #define SPEAKER_ENABLED 0


// /**
//  * Initialize the GPIOs of LEDs / speaker etc.
//  */
// int GPIO_init() {
//     /* RGB led directions & default PORTs to 1 (OFF state) */
//     DDRB |= (1 << PB3);
//     DDRD |= (1 << PD5) | (1 << PD7);
//     PORTB |= (1 << PB3);
//     PORTD |= (1 << PD5) | (1 << PD7);
//     return 0;
// }

// int main() {
//     /* initialize peripheral modules: */
//     GPIO_init();
//     USART0_init(CALC_USART_UBRR(PM_BAUD));
//     USART0_use_stdio();

//     if (SPEAKER_ENABLED) {
//         Timer1_init_ctc();
//     } else {
//         Timer1_init_pwm();
//     }
//     Timer2_init_systicks();
    
//     Timer0_init_pwm();

//     DDRD |= (1 << PD7);

//     sei(); /* activate global interrupts */

//     printf("It's timer hammer!\n");

//     unsigned int last_ping_red = 0;

//     while (1) {
//         /* Print alive message */
//         if (SYSTICKS_PASSED(last_ping_red, 3000)) {
//             last_ping_red = systicks;
//             printf("Stayin' alive! [ticks=%i]\n", systicks);
//         }
//         if (LED_ANIMATION == RGB_PULSE) {
//             /*
//              * Pulse de LEDs based on a triangle wave:
//              * y = (P - abs(t % (2*P) - P) )
//              * Note: outputs values between [0, 3000]!
//              */
//             int wave_red = (3000 - abs(((int)systicks % 6000) - 3000));
//             /* rescale [0, 3000] to [0, 255] using ceil(3000 / 255) */
//             //OCR1A = (wave_red / 12);

//             int wave_blue = (3600 - abs(((int)systicks % 7200) - 3600));
//             //OCR0A = (wave_blue / 14);

//             int wave_green = (4200 - abs(((int)systicks % 8400) - 4200));
//             OCR2B = (wave_green / 45);

//         } else if (LED_ANIMATION == RGB_HSV) {
//             int color = (3600 - abs(((int)systicks % 7200) - 3600));

//             uint8_t out_rgb[3];
//             convert_HSV_to_RGB(color / 10, 1, 1, out_rgb);

//             OCR1A = out_rgb[0];
//             OCR0A = out_rgb[1];
//             OCR2B = out_rgb[2] / 3;
//         }
//         if (SPEAKER_ENABLED) {
//             /* TODO task 4 (bonus): you know what you have to do ;) */
//         }
//     }

//     return 0;
// }
