#include <LiquidCrystal.h>
#include <util/atomic.h>
#include "pin_config.hpp"
#include "config.hpp"
#include "typedefs.hpp"
#include "pressbutton.hpp"
#include "display.hpp"
#include "latencytest.hpp"

// Variables
TestResult test_result;
Display display(&test_result);
LatencyTest latency_test(&test_result);
PressButton start_stop_button(PIN_BUTTON_SS), reset_button(PIN_BUTTON_RS);

void banner() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Back to the");
  lcd.setCursor(5, 1);
  lcd.print("Future");
    
  led_on(Jack::A, Direction::TX);
  delay(250);
  led_on(Jack::B, Direction::RX);
  delay(250);
  led_on(Jack::A, Direction::RX);
  delay(250);
  led_on(Jack::B, Direction::TX);
  delay(250);

  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("with");
  lcd.setCursor(6, 1);
  lcd.print("Dixa");

    
  led_on(Jack::A, Direction::TX);
  delay(250);
  led_on(Jack::B, Direction::RX);
  delay(250);
  led_on(Jack::A, Direction::RX);
  delay(250);
  led_on(Jack::B, Direction::TX);
  delay(250);
  
  led_off();
  lcd.clear();
}

void setup() {
  // We are using 2 analog pins as outputs, to drive the microphone channels of the jacks.
  pinMode(PIN_JACK_A_OUT, OUTPUT);
  pinMode(PIN_JACK_B_OUT, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // display starting banner
  banner();
  
  adc_set_channel(Jack::B, Channel::LEFT);
}

void adc_set_channel(const Jack jack, const Channel channel) {
  int8_t pin = 0;

  if (jack == Jack::A) {
    if (channel == Channel::LEFT) {
      pin = PIN_JACK_A_IN_LEFT;
    }
    else {
      pin = PIN_JACK_A_IN_RIGHT;
    }
  }
  else {
    if (channel == Channel::LEFT) {
      pin = PIN_JACK_B_IN_LEFT;
    }
    else {
      pin = PIN_JACK_B_IN_RIGHT;
    }    
  }

  // set pin
  ADMUX  = _BV(REFS0)   // ref = AVCC
         | _BV(ADLAR)   // left adjust result
         | pin;         // input channel

  // enable ADC
  ADCSRB = 0;           // free running mode
  ADCSRA = _BV(ADEN)    // enable
         | _BV(ADSC)    // start conversion
         | _BV(ADATE)   // auto trigger enable
         | _BV(ADIF)    // clear interrupt flag
         | _BV(ADIE)    // interrupt enable
         | 7;           // prescaler = 128
}

// Interrupt handler called each time an ADC reading is ready.
ISR(ADC_vect) {
  // Read the ADC and convert to signed number.
  int8_t sample = ADCH - 128;

  // Update the phase of the local oscillator.
  static uint16_t phase;
  phase += PHASE_INC;

  // Multiply the sample by square waves in quadrature.
  int8_t x = sample;
  if (((phase>>8) + 0x00) & 0x80) {
    x = -1 - x;
  }
  
  int8_t y = sample;
  if (((phase>>8) + 0x40) & 0x80) {
    y = -1 - y;
  }

  // First order low-pass filter.
  signal_I += x - (signal_I >> LOG_TAU);
  signal_Q += y - (signal_Q >> LOG_TAU);
}

void led_on(const Jack jack, const Direction direction) {
  int8_t pin = 0;
  
  if (jack == Jack::A) {
    if (direction == Direction::RX) {
      pin = PIN_LED_A_RX;
    }
    else {
      pin = PIN_LED_A_TX;
    }
  }
  else {
    if (direction == Direction::RX) {
      pin = PIN_LED_B_RX;
    }
    else {
      pin = PIN_LED_B_TX;
    }
  }

  led_off();
  digitalWrite(pin, HIGH);
}

void led_off() {
  digitalWrite(PIN_LED_A_RX, LOW);
  digitalWrite(PIN_LED_A_TX, LOW);
  digitalWrite(PIN_LED_B_RX, LOW);
  digitalWrite(PIN_LED_B_TX, LOW);
}

void loop() {
  if (start_stop_button.pressed()) {
    latency_test.startstop();
  }

  if (reset_button.pressed()) {
    test_result.reset();
  }
  
  if (latency_test.running()) {
    latency_test.run();
    display.refresh(true);
  }
  else {
   display.refresh(false); 
  }
}
