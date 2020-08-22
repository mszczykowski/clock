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
  boolean repeat;
  boolean repeatOnDays[7];
  Time t;
};

struct Timer{
  boolean isCounting;
  unsigned long tick;
  Time t;
  Time lastTimer;
} timer;

const byte bellIcon[8] = {
  0b00100,
  0b01110,
  0b01110,
  0b01110,
  0b11111,
  0b00000,
  0b00100,
  0b00000
};

const byte yesIcon[8] = {
  0b00000,
  0b00001,
  0b00011,
  0b10110,
  0b11100,
  0b01000,
  0b00000,
  0b00000
};

const byte noIcon[8] = {
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
  0b00000
};

char weekAlarm[7] = {109, 116, 119, 116, 102, 115, 115};

const byte no = 0;
const byte yes = 1;
const byte bell = 7;
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
byte perpetualCal[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const int buzzer = 6;
int buttons;
boolean screenNeedRefresh;
boolean screenNeedCleaning;
boolean beepIsOn;
Alarm alarm1, alarm2;

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
  isLeapYear();
  resetStopwatch();
  mode = clk;
  lcd.createChar(no, noIcon);
  lcd.createChar(yes, yesIcon);
  lcd.createChar(bell, bellIcon);
}

void loop() {
  buttons = analogRead(A1);
  screenNeedRefresh = screenNeedCleaning = false;
  currentTick = millis();
  if (currentTick - clock.tick >= 1000) {
    updateClock();
    screenNeedRefresh = true;
  }
  if (stopwatch.isRunning && currentTick - stopwatch.tick >= 10) {
    updateStopwatch();
    if(mode == stp) screenNeedRefresh = true;
  }
  if (timer.isCounting && currentTick - timer.tick >= 1000){
    updateTimer();
    if(mode == tim) screenNeedRefresh = true;
  }
  if ((mode == tset || mode == tim || beepIsOn) && currentTick - clock.tick >= 500) {
    blink();
    if(beepIsOn)tone(buzzer, 1000, 100);
  }
  if (currentTick - debounce < 650) {
    buttons = 1023;
  }
  if(buttons < 900){
    debounce = millis();
    tone(buzzer, 1000, 200);
    screenNeedRefresh = true;
  }
  if (buttons > 600 && buttons < 900) {
    modeBtn();
  }
  else if (buttons > 50 && buttons < 600) {
    startSetBtn();
  }
  else if (buttons < 50) {
    resetSelectBtn();
  }
  if(screenNeedRefresh) setDisplay(screenNeedCleaning);
}

void modeBtn(){
  if(!beepIsOn){
    mode++;
    if (mode == (tset + 1)) {
     mode = 0;
    }
    if(mode == tim && (timer.isCounting || !isTimeEmpty(timer.t))) set = 3;
    else
    set = 0;
    screenNeedCleaning = true;
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
    beepIsOn = true;
    timer.isCounting = false;
    set = 3;
    mode = tim;
    screenNeedRefresh = screenNeedCleaning = true;
  }
}

void updateClock(){
  clock.tick = currentTick;
  updateTime(clock.t, true);
}

void updateStopwatch(){
  stopwatch.tick = currentTick;
  stopwatch.centiSeconds++;
  if (stopwatch.centiSeconds == 100) {
    stopwatch.centiSeconds = 0;
    updateTime(stopwatch.t, false);
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
      if (!beepIsOn) setTimer();
      else {
        beepIsOn = false;
        timer.t = timer.lastTimer;
      }
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
      if(!beepIsOn){
        set++;
        if(set == 5){
          set = 0;
        }
      }
      break;
    case tset:
      set++;
      if(set == 7){
        set = 0;
      }
      break;
  }
  setDisplay(false);
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
  if(beepIsOn){
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
     if(clock.day > (perpetualCal[clock.month - 1] + 1)){
        clock.day = perpetualCal[clock.month - 1];
      }
      break;
    case 6:
      clock.year++;
      if (clock.year == 2031) {
        clock.year = 2020;
      }
      isLeapYear();
      break;
    default:
      setHoursMinutes(clock.t);
      break;
  }
}

boolean isTimeEmpty(Time t){
  if (t.hours != 0 || t.minutes != 0 || t.seconds != 0) return false;
  else return true;
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
      if(!timer.isCounting && !isTimeEmpty(timer.t)){
        timer.isCounting = true;
        timer.tick = millis();
        timer.lastTimer = timer.t;
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
  if (clock.day == (perpetualCal[clock.month - 1] + 1)) {
    clock.day = 1;
    clock.month ++;
    if(clock.month == 13){
      clock.month = 1;
      clock.year++;
      isLeapYear();
    }
  }
}

void setDisplay(boolean clear) {
  if(clear) lcd.clear();
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
      displayTset();
      break;
    case alm1:
      displayAlm(alarm1, 1);
      break;
    case alm2:
      displayAlm(alarm2, 2);
      break;
  }
}

void displayClk(){
  displayTime(clock.t, 1, true);
  displayDate(clock);
  if(alarm1.isOn){
    lcd.setCursor(10, 1);
    lcd.write(byte(bell));
    lcd.print("1");
  }
  if(alarm2.isOn){
    lcd.setCursor(13, 1);
    lcd.write(byte(bell));
    lcd.print("2");
  }
}

void displayStp(){
  lcd.setCursor(0, 0);
  lcd.print("Stopwatch:      ");
  displayTime(stopwatch.t, 1, true);
  displayCentiSeconds(stopwatch.centiSeconds, 1);
}

void displayTim(){
  displayTime(timer.t, 1, true);
  lcd.setCursor(0, 0);
  lcd.print("Timer:     ");
  if(!beepIsOn){
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

void displayAlm(Alarm a, byte number){
  lcd.setCursor(1, 0);
  for(int i = 0; i < 7; i++){
    if(a.repeatOnDays[i]){
      lcd.print((String)(weekAlarm[i] + (65 - 97)) + " ");
    }
    else{
      lcd.print((String)weekAlarm[i] + " ");
    }
  }
  displayTime(a.t, 1, false);
  lcd.setCursor(6, 1);
  lcd.print("rpt:");
  if(a.repeat){
    lcd.write(byte(yes));
  }
  else{
    lcd.write(byte(no));
  }
  lcd.setCursor(12, 1);
  lcd.write(byte(bell));
  lcd.print((String)number + ":");
  if(a.isOn){
    lcd.write(byte(yes));
  }
  else{
    lcd.write(byte(no));
  }
}

void displayTset(){
  displayTime(clock.t, 1, true);
  displayDate(clock);
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

void isLeapYear(){
  if((clock.year%4 == 0 && clock.year%100) || clock.year%400 == 0){
    perpetualCal[1] = 29;
  }
  else{
    perpetualCal[1] = 28;
    if(clock.month == 2 && clock.day == 29) clock.day = 28;
  }
}

void displayTime(Time t, int row, boolean showSeconds) {
  lcd.setCursor(0, row);
  padLeft(t.hours);
  lcd.setCursor(2, row);
  lcd.print(":");
  lcd.setCursor(3, row);
  padLeft(t.minutes);
  if(showSeconds){
    lcd.setCursor(5, row);
    lcd.print(":");
    lcd.setCursor(6, row);
    padLeft(t.seconds);
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
