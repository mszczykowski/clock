#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned long tick;
unsigned long currentTime;
byte hours = 0;
byte minutes = 0;
byte seconds = 0;
byte day = 1;
byte dayOfWeek = 2;

byte mode = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print(String(5, 6));
}

void loop() {
  int buttons = analogRead(A1);
  currentTime = millis();
  if(currentTime - tick >= 1000){
    updateTime();
  }
  if (buttons > 600 && buttons < 700){
    mode++;
    if(mode == 4){
      mode = 0;
    }
  }
  else if (buttons > 500 && buttons < 600){
    
  }
  else if (buttons < 50){
    
  }
  else{
    
  }
}

void updateTime(){
  seconds++;
  tick = currentTime;
  if(seconds == 60){
    seconds = 0;
    minutes++;
  }
  if(minutes == 60){
    minutes = 0;
    hours++;
  }
  if(hours == 24){
    hours = 0;
    dayOfWeek++;
    day++;
    displayDate();
  }
  if(dayOfWeek == 8){
    dayOfWeek = 0;
  }
  if(day == 32){
    day = 0;
  }
  if(mode == 0){
    displayTime(1);
  }
}

void displayDate(){
  lcd.setCursor(0, 0);
  switch(dayOfWeek){
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
      lcd.print("Thursday" );
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
  if(day < 10){
    lcd.print(String(" " + (String)day));
  }
  else{
    lcd.print(day);
  }
}

void displayTime(int row){
  lcd.setCursor(0, row);
  if(hours < 10){
      lcd.print(String("0" + (String)hours));
    }
  else{
    lcd.print(hours);
  }
  lcd.setCursor(2, row);
  lcd.print(":");
  lcd.setCursor(3, row);
  if(minutes < 10){
    lcd.print(String("0" + (String)minutes));
  }
  else{
    lcd.print(minutes);
  }
  lcd.setCursor(5, row);
  lcd.print(":");
  lcd.setCursor(6, row);
  if(seconds < 10){
    lcd.print(String("0" + (String)seconds));
  }
  else{
    lcd.print(seconds);
  }
}
