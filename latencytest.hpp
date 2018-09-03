#ifndef LATENCY_TESTRUINO_LATENCYTEST_HPP
#define LATENCY_TESTRUINO_LATENCYTEST_HPP

#include "config.hpp"

// Demodulated (I, Q) amplitudes.
volatile int16_t signal_I, signal_Q;

void led_on(const Jack jack, const Direction direction);
void led_off();

class LatencyTest {
  private:
  bool _running = false;
  TestResult* _test_result = nullptr;

  uint32_t _tone_sent = 0;
  uint32_t _tone_start = 0;
  uint32_t _last_detection = 0;

  Jack _jack = Jack::A;

  inline uint16_t get_power_reading() {
    int16_t I, Q;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      I = signal_I;
      Q = signal_Q;
    }
    return sq((int8_t)(I >> LOG_TAU)) + sq((int8_t)(Q >> LOG_TAU));
  }

  inline void play_tone() {
    int8_t pin;
    if (_jack == Jack::A) {
      pin = PIN_JACK_A_OUT;
    }
    else {
      pin = PIN_JACK_B_OUT;
    }
    led_on(Jack::A, Direction::TX);
    tone(pin, TONE_FREQUENCY, TONE_DURATION);
    _tone_sent = millis();
    delay(TONE_DURATION);
    led_off();
    _test_result->increment_sent();
  }

  public:
  inline LatencyTest(TestResult* result) : _test_result(result) {}

  inline bool running() const {
    return _running;
  }
  
  inline void startstop() {
    if (_running) {
      _running = false;
      _last_detection = millis() + 1000; // clear the receiving line
      _tone_start = 0;
      _tone_sent = 0;
      led_off();
    } else {
      _running = true;
    }
  }

  inline void run() {
    if (_running) {
      // if we had a detection before, let's make sure the line is clear before listening again.
      if (_last_detection != 0) {
        if ((millis() - _last_detection) > TONE_DURATION) {
          _last_detection = 0;
        } else {
          return;
        }
      }
      
      if (_tone_sent != 0) { // we're waiting for a tone to arrive
        if ((millis() - _tone_sent) > TONE_RECEIVE_TIMEOUT) { // tone is lost
          _tone_sent = 0;
          return;
        }
  
        else { // tone is on its way, let's have a listen
          int32_t loop_start = millis();
          // block the main thread for 100ms while we listen continuously (increased accuracy)
          while ((millis() - loop_start) < LOOP_DURATION) {
            if (get_power_reading() > TONE_DURATION_THRESHOLD) { // a tone is currently being heard
              led_on(Jack::B, Direction::RX);
              if (_tone_start == 0) { // first time we're hearing it
                _tone_start = millis();
              }
              else if ((millis() - _tone_start) > TONE_DURATION_THRESHOLD) { // we've been hearing it for 30ms
                // log it
                _test_result->add(_tone_start - _tone_sent);
                _last_detection = millis();
                _tone_start = 0;
                _tone_sent = 0;
                led_off();
                break;
              }
            }
            else {
              led_off();
              if (_tone_start != 0) { // we're not hearing anything, but we started hearing a tone previously. Cancel it out.
                _tone_start = 0;
              }
            }
          }
        }
      }
      else {
        play_tone();
      }
    }
  }
};

#endif
