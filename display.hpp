#ifndef LATENCY_TESTRUINO_DISPLAY_HPP
#define LATENCY_TESTRUINO_DISPLAY_HPP

extern LiquidCrystal lcd;

#define ARRAY_SIZE 512

class TestResult {
  private:
  uint16_t _measurements[ARRAY_SIZE];
  uint16_t _index = 0;
  
  public:
  inline TestResult() {
    reset();
  }
  
  uint16_t minimum = 0;
  uint16_t maximum = 0;
  uint16_t average = 0;
  uint16_t stddev = 0;
  uint16_t sent = 0;
  uint16_t received = 0;
  int8_t loss = 0;

  inline void reset() {
    minimum = 0;
    maximum = 0;
    average = 0;
    stddev = 0;
    sent = 0;
    received = 0;
    loss = 0;
    
    memset(_measurements, 0, ARRAY_SIZE * sizeof(uint16_t));
    _index = 0;
  }

  inline void add(uint16_t measurement) {
    _measurements[_index] = measurement;
    _index = (_index + 1) % ARRAY_SIZE;

    minimum = 65535;
    maximum = 0;
    uint32_t total = 0;
    uint16_t count = 0;
    
    for (uint16_t i = 0; i < ARRAY_SIZE; ++i) {
      if (_measurements[i] != 0) {
        total += _measurements[i];
        ++count;
        
        if (_measurements[i] < minimum) {
          minimum = _measurements[i];
        }
  
        if (_measurements[i] > maximum) {
          maximum = _measurements[i];
        }
      }
    }

    average = total / count;
    loss = ((sent - ++received) * 100) / sent;

    uint32_t variance = 0;
    for (uint16_t i = 0; i < count; ++i) {
      variance += sq(_measurements[i] - average);
    }

    stddev = sqrt(variance / count);
  }

  inline void increment_sent() {
    ++sent;
  }
};

class Display {
  private:
  enum class Page {
    minmax,
    avgstd,
    loss
  };

  Page _page = Page::minmax;
  uint32_t _last_draw = 0;
  uint8_t _draw_interval = 100;
  uint32_t _last_page = 0;
  uint16_t _page_interval = 2000;
  TestResult* _test_result = nullptr;

  inline void draw(bool running) {
    lcd.setCursor(0, 0);
    char buffer[24];

    if (running) {
      lcd.print("R");
    } else {
      lcd.print("P");
    }

    if (_page == Page::minmax) {
      lcd.setCursor(2, 0);
      lcd.print(F("  min    max"));
      sprintf(buffer, "   %4u   %4u", _test_result->minimum, _test_result->maximum);
    }

    else if (_page == Page::avgstd) {
      lcd.setCursor(2, 0);
      lcd.print(F("  avg   stddev"));
      sprintf(buffer, "   %4u   %4u", _test_result->average, _test_result->stddev);
    }

    else {
      lcd.setCursor(2, 0);
      lcd.print(F(" count  loss%"));
      sprintf(buffer, "  %02u/%02u  %3d%%", _test_result->received, _test_result->sent, _test_result->loss);
    }

    lcd.setCursor(0, 1);
    lcd.print(buffer);
    
    _last_draw = millis();
  }

  public:
  inline Display(TestResult* test_result) : _test_result(test_result) {}

  inline void refresh(bool running) {
    if ((millis() - _last_draw) > _draw_interval) {
      if ((millis() - _last_page) > _page_interval) {
        lcd.clear();
        _last_page = millis();
        
        if (_page == Page::minmax) {
          _page = Page::avgstd;
        } else if (_page == Page::avgstd) {
          _page = Page::loss;
        } else {
          _page = Page::minmax;
        }
      }
      
      draw(running);
    }
  }
  
};

#endif
