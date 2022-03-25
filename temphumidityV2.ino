//libraries used ---------------------------------------------------
#include <dht.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include "RTClib.h"

//constants --------------------------------------------------------

RTC_DS3231 rtc;
dht DHT;

//For UNO
/*
LiquidCrystal lcd(1,2,4,5,6,7); //pins for LCD 
int backLight = 13; //pin for lcd backlight on off 
const int pingPin = 10; // Trigger Pin of Ultrasonic Sensor 
const int echoPin = 11; // Echo Pin of Ultrasonic Sensor 
#define DHT11_PIN 8 //pin for hdt sensor 
*/

//For Nano
LiquidCrystal lcd(2,3,4,5,6,7); //pins for LCD 
int backLight = 8; //pin for lcd backlight on off 
#define DHT11_PIN 9 //pin for hdt sensor 
const int pingPin = 10; // Trigger Pin of Ultrasonic Sensor 
const int echoPin = 11; // Echo Pin of Ultrasonic Sensor 


void setup(){
  lcd.begin(16, 2);
  Serial.begin(9600);
  delay(3000); // wait for console opening
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    setTime();
  }
  
}

void loop(){
  
  
checkTempHumidity();

printDate();

 for(int i=0;i<20000;i++){

  printTime();
   
    long duration, inches, cm;
    pinMode(pingPin, OUTPUT);
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPin, LOW);
    pinMode(echoPin, INPUT);
    duration = pulseIn(echoPin, HIGH);
    cm = microsecondsToCentimeters(duration);
    
    if(cm<10){
      digitalWrite(backLight, HIGH);
      delay(10000);
    }else{
      digitalWrite(backLight, LOW);
    }
  }
}

/*
 * converts microseconds to cm
 */
long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}

/*
 * prints the time on the lcd screen
 */
void printTime(){
  DateTime now = rtc.now();  
  lcd.setCursor(11,1);
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  if(now.minute()<9){
    lcd.print('0');
  }
  lcd.print(now.minute(), DEC);
}

/*
 * Prints the date on the lcd screen
 */
void printDate(){
  DateTime now = rtc.now();
  lcd.setCursor(0,1);
  lcd.print(now.year()-2000, DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
}
/*
 * Method that checks the humidity and displays it on the screen
 */
void checkTempHumidity(){
  int chk = DHT.read11(DHT11_PIN);
  int temp = (int)DHT.temperature;
  int humidity = (int)DHT.humidity;
   
  lcd.setCursor(0,0); 
  //lcd.print("Temp:");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(12,0);
  //lcd.print("Humd:");
  lcd.print(humidity);
  lcd.print("%H");
}

void serialPrintTime(){
    char daysOfTheWeek[7][3] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    DateTime now = rtc.now();
    Serial.println("Current Date & Time: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}
/*
 * Method to set the time over serial connection
 * 
 */
void setTime(){
  
  //variables
  byte year;
  byte month;
  byte date;
  byte dow;
  byte hour;
  byte minute;
  byte second;
  Serial.println("Format YYMMDDwhhmmssx");
  Serial.println("Where YY = Year (ex. 20 for 2020)");
  Serial.println("      MM = Month (ex. 04 for April)");
  Serial.println("      DD = Day of month (ex. 09 for 9th)");
  Serial.println("      w  = Day of week from 1 to 7, 1 = Sunday (ex. 5 for Thursday)");
  Serial.println("      hh = hours in 24h format (ex. 09 for 9AM or 21 for 9PM)");
  Serial.println("      mm = minutes (ex. 02)");
  Serial.println("      ss = seconds (ex. 42)");
  Serial.println("Example for input : 2004095090242x");
  Serial.println("-----------------------------------------------------------------------------");
  Serial.println("Please enter the current time to set on DS3231 ended by 'x':");
  while(!Serial.available()){
  }
    inputDateFromSerial();
//    rtc.setClockMode(false);  // set to 24h
    DateTime timeToSet = new DateTime (year, month, date, hour, minute, second);
    rtc.adjust(
    rtc.setYear(year);
    rtc.setMonth(month);
    rtc.setDate(date);
    rtc.setDoW(dow);
    rtc.setHour(hour);
    rtc.setMinute(minute);
    rtc.setSecond(second);
  
}

 void inputDateFromSerial() {
 // Call this if you notice something coming in on
  // the serial port. The stuff coming in should be in
  // the order YYMMDDwHHMMSS, with an 'x' at the end.
  boolean isStrComplete = false;
  char inputChar;
  byte temp1, temp2;
  char inputStr[20];

  uint8_t currentPos = 0;
  while (!isStrComplete) {
    if (Serial.available()) {
      inputChar = Serial.read();
      inputStr[currentPos] = inputChar;
      currentPos += 1;

      // Check if string complete (end with "x")
      if (inputChar == 'x') {
        isStrComplete = true;
      }
    }
  }
  Serial.println(inputStr);

  // Find the end of char "x"
  int posX = -1;
  for(uint8_t i = 0; i < 20; i++) {
    if(inputStr[i] == 'x') {
      posX = i;
      break;
    }
  }

  // Consider 0 character in ASCII
  uint8_t zeroAscii = '0';

  // Read Year first
  temp1 = (byte)inputStr[posX - 13] - zeroAscii;
  temp2 = (byte)inputStr[posX - 12] - zeroAscii;
  year = temp1 * 10 + temp2;

  // now month
  temp1 = (byte)inputStr[posX - 11] - zeroAscii;
  temp2 = (byte)inputStr[posX - 10] - zeroAscii;
  month = temp1 * 10 + temp2;

  // now date
  temp1 = (byte)inputStr[posX - 9] - zeroAscii;
  temp2 = (byte)inputStr[posX - 8] - zeroAscii;
  date = temp1 * 10 + temp2;

  // now Day of Week
  dow = (byte)inputStr[posX - 7] - zeroAscii;

  // now Hour
  temp1 = (byte)inputStr[posX - 6] - zeroAscii;
  temp2 = (byte)inputStr[posX - 5] - zeroAscii;
  hour = temp1 * 10 + temp2;

  // now Minute
  temp1 = (byte)inputStr[posX - 4] - zeroAscii;
  temp2 = (byte)inputStr[posX - 3] - zeroAscii;
  minute = temp1 * 10 + temp2;

  // now Second
  temp1 = (byte)inputStr[posX - 2] - zeroAscii;
  temp2 = (byte)inputStr[posX - 1] - zeroAscii;
  second = temp1 * 10 + temp2;
}
