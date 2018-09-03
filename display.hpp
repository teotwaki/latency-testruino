#ifndef LATENCY_TESTRUINO_DISPLAY_HPP
#define LATENCY_TESTRUINO_DISPLAY_HPP

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

class TestResult {
  private:
  int32_t _total_duration;
  
  public:
  int16_t minimum = 0;
  int16_t maximum = 0;
  int16_t average = 0;
  int16_t stddev = 0;
  int16_t sent = 0;
  int16_t received = 0;

  inline void reset() {
    minimum = 0;
    maximum = 0;
    average = 0;
    stddev = 0;
    sent = 0;
    received = 0;
    _total_duration = 0;
  }

  inline void add(uint32_t measurement) {
    ++received;
    
    if (minimum == 0 || (measurement < minimum)) {
      minimum = measurement;
    }

    if (measurement > maximum) {
      maximum = measurement;
    }

    _total_duration += measurement;
    average = _total_duration / received;
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
  long int _last_draw = 0;
  long int _draw_interval = 100;
  long int _last_page = 0;
  long int _page_interval = 2000;
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
      lcd.print("  min    max");
      sprintf(buffer, "   %4d   %4d", _test_result->minimum, _test_result->maximum);
    }

    else if (_page == Page::avgstd) {
      lcd.setCursor(2, 0);
      lcd.print("  avg   stddev");
      sprintf(buffer, "   %4d   %4d", _test_result->average, _test_result->stddev);
    }

    else {
      lcd.setCursor(2, 0);
      lcd.print(" count  loss%");
      sprintf(buffer, "  %02d/%02d  %3d%%", _test_result->received, _test_result->sent, ((_test_result->sent - _test_result->received) * 100) / _test_result->sent);
    }

    lcd.setCursor(0, 1);
    lcd.print(buffer);
//    if (_state == State::paused) {
//      lcd.setCursor(0, 0);
//      lcd.print("P min  max  avg");
//      lcd.setCursor(0, 1);
//      lcd.print("  238  472  349");
//    }
    
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
