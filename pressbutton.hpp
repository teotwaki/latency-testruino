#ifndef LATENCY_TESTRUINO_PRESSBUTTON_HPP
#define LATENCY_TESTRUINO_PRESSBUTTON_HPP

class PressButton {
  private:
  int8_t _pin;
  uint32_t _last_press = 0;
  uint16_t _debounce_delay;

  bool debouncing() {
    return ((millis() - _last_press) < _debounce_delay);
  }
  
  public:
  PressButton(int8_t pin, unsigned long delay = 300)
    : _pin(pin)
    , _debounce_delay(delay) {};

  inline bool pressed() {
    if (debouncing()) {
      return false;
    }
    _last_press = millis();
    return digitalRead(_pin) != 0;
  }
};

#endif
