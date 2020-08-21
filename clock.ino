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
  byte month;
  int year;
} clock;

struct Stopwatch {
  Time t;
  boolean isRunning;
  unsigned long tick;
  byte centiSeconds;
} stopwatch;

struct Alarm{
  boolean isOn;
  boolean beepIsOn;
  boolean repeatOnDays[7];
  Time t;
};

struct Timer{
  boolean isCounting;
  boolean beepIsOn;
  unsigned long tick;
  Time t;
} timer;


//modes:
const byte clk = 0;
const byte stp = 1;
const byte tim = 2;
const byte alm1 = 3;
const byte alm2 = 4;
const byte tset = 5; 

unsigned long currentTick;
unsigned long debounce;
byte mode;
byte set;
byte blinkSwitch;
byte perpetualCal[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const int buzzer = 6;
int buttons;

void setup() {
  lcd.begin(16, 2);
  clock.t.hours = 12;
  clock.t.minutes = 0;
  clock.t.seconds = 0;
  clock.day = 1;
  clock.month = 1;
  clock.year = 2020;
  clock.dayOfWeek = 0;
  clock.tick = 0;
  resetStopwatch();
  mode = clk;
  pinMode(buzzer, OUTPUT);
  setDisplay();
}

void loop() {
  buttons = analogRead(A1);
  currentTick = millis();
  if ((mode == tset || mode == tim) && currentTick - clock.tick >= 1000) {
    blink();
  }
  if (currentTick - clock.tick >= 1000) {
    updateClock();
    setDisplay();
  }
  if (stopwatch.isRunning && currentTick - stopwatch.tick >= 10) {
    updateStopwatch();
  }
  if (timer.isCounting || timer.beepIsOn && currentTick - timer.tick >= 1000){
    if(timer.isCounting) updateTimer();
    else displayTim();
  }
  if ((mode == tset || mode == tim || timer.beepIsOn) && currentTick - clock.tick >= 500) {
    blink();
  }
  if (currentTick - debounce < 650) {
    buttons = 1023;
  }
  if (currentTick - debounce > 200) {
    noTone(buzzer);
  }
  if(buttons < 900){
    debounce = millis();
    tone(buzzer, 1000);
  }
  if (buttons > 600 && buttons < 700) {
      mode++;
      if (mode == (tset + 1)) {
        mode = 0;
      }
      set = 0;
      setDisplay();
  }
  else if (buttons > 500 && buttons < 600) {
    startSetBtn();
  }
  else if (buttons < 50) {
    resetSelectBtn();
  }
}

void updateTimer(){
  timer.tick = currentTick;
  if(timer.t.seconds != 0){
    timer.t.seconds--;
  }
  else if(timer.t.seconds == 0 && timer.t.minutes != 0){
    timer.t.minutes--;
    timer.t.seconds = 59;
  }
  else if(timer.t.minutes == 0 && timer.t.hours != 0){
    timer.t.hours--;
    timer.t.seconds = 59;
    timer.t.minutes = 59;
  }
  else{
    timer.beepIsOn = true;
    timer.isCounting = false;
    lcd.print("alarm");
  }
  if(mode == tim){
    setDisplay();
  }
}

void updateClock(){
  clock.tick = currentTick;
  updateTime(clock.t, true);
  if (mode == clk || mode == tset) setDisplay();
}

void updateStopwatch(){
  stopwatch.tick = currentTick;
  stopwatch.centiSeconds++;
  if (stopwatch.centiSeconds == 100) {
    stopwatch.centiSeconds = 0;
    updateTime(stopwatch.t, false);
  }
  if (mode == stp) {
    setDisplay();
  }
}

void resetTime(Time &t){
  t.hours = 0;
  t.minutes = 0;
  t.seconds = 0;
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
  resetTime(stopwatch.t);
  stopwatch.centiSeconds = 0;
}


void startSetBtn() {
  switch (mode) {
    case clk:

      break;
    case stp:
      startLapStopwatch();
      break;
    case tim:
      setTimer();
      break;
    case tset:
      setTime();
      break;
  }
}

void resetSelectBtn() {
  switch (mode) {
    case clk:

      break;
    case stp:
      resetStopwatch();
      break;
    case tim:
      set++;
      if(set == 5){
        set = 0;
      }
      break;
    case tset:
      set++;
      if(set == 7){
        set = 0;
      }
      break;
  }
  setDisplay();
}

void blink(){
  switch(mode){
    case tset:
      blinkClk();
      break;
    case tim:
      blinkTim();
      break;
  }
}

void blinkClk(){
  switch(set){
    case 3:
      lcd.setCursor(1, 0);
      lcd.print("   ");
      break;
    case 4:
      lcd.setCursor(5, 0);
      lcd.print("  ");
      break;
    case 5:
      lcd.setCursor(8, 0);
      lcd.print("  ");
      break;
    case 6:
      lcd.setCursor(11, 0);
      lcd.print("    ");
      break;
    default:
      blinkHrMinSec();
      break;
  }
}

void blinkTim(){
  if(timer.beepIsOn){
    lcd.setCursor(10, 0);
    lcd.print("      ");
  }
  else{
    switch(set){
      case 3:
        lcd.setCursor(11, 0);
        lcd.print("     ");
        break;
      case 4:
        lcd.setCursor(11, 1);
        lcd.print("     ");
        break;
      default:
        blinkHrMinSec();
        break;
    }
  }
}

void blinkHrMinSec(){
  switch(set){
    case 0:
      lcd.setCursor(0, 1);
      lcd.print("  ");
      break;
    case 1:
      lcd.setCursor(3, 1);
      lcd.print("  ");
      break;
    case 2:
      lcd.setCursor(6, 1);
      lcd.print("  ");
      break;
  }
}

void setHoursMinutes(Time &t){
  switch(set){
    case 0:
      t.hours++;
      if(t.hours == 24){
        t.hours = 0;
      }
      break;
    case 1:
      t.minutes++;
      if(t.minutes == 60){
        t.minutes = 0;
      }
      break;
  }
}

void setTime(){
  switch(set){
    case 2:
      clock.t.seconds = -1;
      break;
    case 3:
      clock.dayOfWeek++;
      if (clock.dayOfWeek == 7) {
        clock.dayOfWeek = 0;
      }
      break;
    case 4:
      clock.day++;
      if(clock.day == (perpetualCal[clock.month - 1] + 1)){
        clock.day = 1;
      }
      break;
    case 5:
      clock.month++;
      if (clock.month == 13) {
        clock.month = 1;
      }
      break;
    case 6:
      clock.year++;
      if (clock.year == 2031) {
        clock.year = 2020;
      }
      break;
    default:
      setHoursMinutes(clock.t);
      break;
  }
}

void setTimer(){
  switch(set){
    case 2:
      timer.t.seconds++;
      if(timer.t.seconds == 60) {
        timer.t.seconds = 0;
      }
      break;
    case 3:
      if(!timer.isCounting && (timer.t.hours != 0 || timer.t.minutes != 0 || timer.t.seconds != 0)){
        timer.isCounting = true;
        timer.tick = millis();
      }
      else{
        timer.isCounting = false;
      }
      break;
    case 4:
      resetTime(timer.t);
      timer.isCounting = false;
      break;
    default:
      setHoursMinutes(timer.t);
      break;
  }
}

void updateTime(Time &t, boolean withDate) {
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
      if(withDate){
        updateDate();
      }
    }
  }
}

void updateDate() {
  clock.dayOfWeek++;
  clock.day++;
  if (clock.dayOfWeek == 7) {
    clock.dayOfWeek = 0;
  }
  if (clock.day == 32) {
    clock.day = 1;
  }
}

void setDisplay() {
  lcd.clear();
  if(!timer.beepIsOn){
    switch (mode) {
      case clk:
        displayClk();
        break;
      case stp:
        displayStp();
        break;
      case tim:
        displayTim();
        break;
      case tset:
        displayClk();
        lcd.setCursor(11, 1);
        lcd.print("(SET)");
        break;
    }
  }
}

void displayClk(){
  displayTime(clock.t, 1);
  displayDate(clock);
}

void displayStp(){
  lcd.setCursor(0, 0);
  lcd.print("Stopwatch:      ");
  displayTime(stopwatch.t, 1);
  displayCentiSeconds(stopwatch.centiSeconds, 1);
}

void displayTim(){
  displayTime(timer.t, 1);
  lcd.setCursor(0, 0);
  lcd.print("Timer:     ");
  if(!timer.beepIsOn){
    lcd.setCursor(11, 0);
    if(timer.isCounting){
      lcd.print("Stop ");
    }
    else {
      lcd.print("Start");
    }
    lcd.setCursor(11, 1);
    lcd.print("Reset");
  }
  else{
    lcd.setCursor(10, 0);
    lcd.print("Cancel");
  }
}

void displayTset(){
  displayClk();
  lcd.setCursor(11, 1);
  lcd.print("(SET)");
}

void displayDate(Clock c) {
  lcd.setCursor(1, 0);
  switch (c.dayOfWeek) {
    case 0:
      lcd.print("Mon");
      break;
    case 1:
      lcd.print("Tue");
      break;
    case 2:
      lcd.print("Wed");
      break;
    case 3:
      lcd.print("Thu");
      break;
    case 4:
      lcd.print("Fri");
      break;
    case 5:
      lcd.print("Sat");
      break;
    case 6:
      lcd.print("Sun");
      break;
  }
  lcd.setCursor(5, 0);
  padLeft(c.day);
  lcd.print(".");
  padLeft(c.month);
  lcd.print(".");
  padLeft(c.year);
}

void padLeft(int x){
  if (x < 10) {
    lcd.print(String("0" + (String)x));
  }
  else {
    lcd.print(x);
  }
}

void isLeapYear(Clock c){
  if((c.year%4 == 0 && c.year%100) || c.year%400 == 0){
    perpetualCal[1] = 29;
  }
  else{
    perpetualCal[1] = 28;
    if(c.month == 2 && c.day == 29) c.day = 28;
  }
}

void displayTime(Time t, int row) {
  lcd.setCursor(0, row);
  padLeft(t.hours);
  lcd.setCursor(2, row);
  lcd.print(":");
  lcd.setCursor(3, row);
  padLeft(t.minutes);
  lcd.setCursor(5, row);
  lcd.print(":");
  lcd.setCursor(6, row);
  padLeft(t.seconds);
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
