#ifndef LATENCY_TESTRUINO_PIN_CONFIG_HPP
#define LATENCY_TESTRUINO_PIN_CONFIG_HPP

// There are two pairs of LEDs. One one the left side, and one on the right
// side of the board. The TX LEDs are blue, and the RX pins are green. A is on
// the left hand side of the board, and B is on the other side.
#define PIN_LED_A_TX 9
#define PIN_LED_A_RX 6
#define PIN_LED_B_TX 5
#define PIN_LED_B_RX 3

// LCD pins
#define PIN_LCD_RS 11
#define PIN_LCD_EN 10
#define PIN_LCD_D4 8
#define PIN_LCD_D5 7
#define PIN_LCD_D6 4
#define PIN_LCD_D7 2

// buttons
// Note to self: put buttons on pins 2 & 3 of the Arduino Uno. It allows for
// the use of interrupts instead of having to poll them constantly.
#define PIN_BUTTON_SS 13
#define PIN_BUTTON_RS 12

// Input/Output jacks
#define PIN_JACK_A_OUT      A4
#define PIN_JACK_B_OUT      A5
#define PIN_JACK_A_IN_LEFT  0
#define PIN_JACK_A_IN_RIGHT 1
#define PIN_JACK_B_IN_LEFT  2
#define PIN_JACK_B_IN_RIGHT 3

#endif // LATENCY_TESTRUINO_PIN_CONFIG_HPP
