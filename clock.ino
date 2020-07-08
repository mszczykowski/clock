#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

struct Time {
  byte hours;
  byte minutes;
  byte seconds;
};

struct Clock {
  Time t;
  unsigned long tick;
  byte day;
  byte dayOfWeek;
} clock;

struct Stopwatch {
  Time t;
  boolean isRunning;
  unsigned long tick;
  byte centiSeconds;
} stopwatch;

boolean wasButtonRelased;
unsigned long currentTime;
byte mode;
byte set;

void setup() {
  lcd.begin(16, 2);
  clock.t.hours = 12;
  clock.t.minutes = 0;
  clock.t.seconds = 0;
  clock.day = 1;
  clock.dayOfWeek = 0;
  clock.tick = 0;
  resetStopwatch();
  mode = 0;
  setDisplay();
}

void loop() {
  int buttons = analogRead(A1);
  currentTime = millis();
  if (currentTime - clock.tick >= 1000) {
    clock.tick = currentTime;
    updateTime(clock.t);
    updateDate(clock);
    if (mode == 0 || mode == 3) {
      displayTime(clock.t, 1);
      displayDate(clock);
    }
  }
  if (stopwatch.isRunning && currentTime - stopwatch.tick >= 10) {
    stopwatch.tick = currentTime;
    if (mode == 1) displayCentiSeconds(stopwatch.centiSeconds, 1);
    stopwatch.centiSeconds++;
    if (stopwatch.centiSeconds == 100) {
      stopwatch.centiSeconds = 0;
      updateTime(stopwatch.t);
      if (mode == 1) {
        displayTime(stopwatch.t, 1);
        displayCentiSeconds(stopwatch.centiSeconds, 1);
      }
    }
  }
  if (buttons > 600 && buttons < 700) {
    if(wasButtonRelased){
      mode++;
      if (mode == 4) {
        mode = 0;
      }
      setDisplay();
      wasButtonRelased = false;
    }
  }
  else if (buttons > 500 && buttons < 600) {
    wasButtonRelased = false;
    startSetBtn(mode);
  }
  else if (buttons < 50) {
    wasButtonRelased = false;
    resetSelectBtn(mode);
  }
  else {
    wasButtonRelased = true;
  }
}

void startLapStopwatch() {
  if (!stopwatch.isRunning) {
    stopwatch.isRunning = true;
    stopwatch.tick = millis();
  }
  else {
    stopwatch.isRunning = false;
  }
}

void resetStopwatch() {
  stopwatch.t.hours = 0;
  stopwatch.t.minutes = 0;
  stopwatch.t.seconds = 0;
  stopwatch.centiSeconds = 0;
  displayTime(stopwatch.t, 1);
  displayCentiSeconds(stopwatch.centiSeconds, 1);
}

void startSetBtn(byte m) {
  switch (m) {
    case 0:

      break;
    case 1:
      startLapStopwatch();
      break;
    case 2:

      break;
    case 3:

      break;
  }
}

void resetSelectBtn(byte m) {
  switch (m) {
    case 0:

      break;
    case 1:
      resetStopwatch();
      break;
    case 2:

      break;
    case 3:
      set++;
      if(set == 5){
        set = 0;
      }
      higlight();
      break;
  }
}

void higlight(){
  switch(set){
    case 0:
      lcd.setCursor(1, 0);
      lcd.blink();
      lcd.setCursor(1, 1);
      lcd.blink();
      break;
  }
}

void updateTime(Time &t) {
  t.seconds++;
  if (t.seconds == 60) {
    t.seconds = 0;
    t.minutes++;
    if (t.minutes == 60) {
      t.minutes = 0;
      t.hours++;
    }
    if (t.hours == 24) {
      t.hours = 0;
    }
  }
}

void updateDate(Clock &c) {
  if (c.t.hours == 24) {
    c.dayOfWeek++;
    c.day++;
    if (c.dayOfWeek == 8) {
      c.dayOfWeek = 0;
    }
    if (c.day == 32) {
      c.day = 1;
    }
  }
}

void setDisplay() {
  lcd.clear();
  switch (mode) {
    case 0:
      displayTime(clock.t, 1);
      displayDate(clock);
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Stopwatch:      ");
      displayTime(stopwatch.t, 1);
      displayCentiSeconds(stopwatch.centiSeconds, 1);
      break;
    case 2:

      break;
    case 3:
      lcd.setCursor(11, 1);
      lcd.print("(SET)");
      break;
  }
}

void displayDate(Clock c) {
  lcd.setCursor(0, 0);
  switch (c.dayOfWeek) {
    case 0:
      lcd.print("Monday   ");
      break;
    case 1:
      lcd.print("Tuesday  ");
      break;
    case 2:
      lcd.print("Wednesday");
      break;
    case 3:
      lcd.print("Thursday ");
      break;
    case 4:
      lcd.print("Friday   ");
      break;
    case 5:
      lcd.print("Saturday ");
      break;
    case 6:
      lcd.print("Sunday   ");
      break;
  }
  lcd.setCursor(10, 0);
  if (c.day < 10) {
    lcd.print(String(" " + (String)c.day));
  }
  else {
    lcd.print(c.day);
  }
}

void displayTime(Time t, int row) {
  lcd.setCursor(0, row);
  if (t.hours < 10) {
    lcd.print(String("0" + (String)t.hours));
  }
  else {
    lcd.print(t.hours);
  }
  lcd.setCursor(2, row);
  lcd.print(":");
  lcd.setCursor(3, row);
  if (t.minutes < 10) {
    lcd.print(String("0" + (String)t.minutes));
  }
  else {
    lcd.print(t.minutes);
  }
  lcd.setCursor(5, row);
  lcd.print(":");
  lcd.setCursor(6, row);
  if (t.seconds < 10) {
    lcd.print(String("0" + (String)t.seconds));
  }
  else {
    lcd.print(t.seconds);
  }
}

void displayCentiSeconds(byte centiSeconds, int row) {
  lcd.setCursor(8, row);
  lcd.print(".");
  lcd.setCursor(9, row);
  if (centiSeconds < 10) {
    lcd.print(String("0" + (String)centiSeconds));
  }
  else {
    lcd.print(centiSeconds);
  }
}
