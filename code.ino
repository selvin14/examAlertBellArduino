/*
  16x2 LCD display

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 7
 * LCD D5 pin to digital pin 6
 * LCD D6 pin to digital pin 5
 * LCD D7 pin to digital pin 4
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

*/

/*
CONNECTIONS:
DS3231 SDA --> SDA (A4)
DS3231 SCL --> SCL (A5)
DS3231 VCC --> 3.3v or 5v
DS3231 GND --> GND
*/

// include the library code:
#include <LiquidCrystal.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
// #include <stdio.h>
RtcDS3231<TwoWire> Rtc(Wire);

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int mode = 1;   // counter for the number of button presses

const int buttonPin0 = 2,buttonPin1 = 3;     // the number of the pushbutton pin
const int relay =  14;      // the number of the LED pin

// variables will change:
volatile int buttonState = 0;         // variable for reading the pushbutton status

int buzzer = 8;//the pin of the active buzzer


void setup() {

  pinMode(buzzer,OUTPUT);//initialize the buzzer pin as an output

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("AutoBell - APSIT");
  lcd.setCursor(0,1);
  lcd.print("Design - SVF");
  delay(2000);
  lcd.clear();

//****************RTC*****************
    Serial.begin(9600);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    //--------RTC SETUP ------------
    // if you are using ESP-01 then uncomment the line below to reset the pins to
    // the available pins for SDA, SCL
    // Wire.begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL
    
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Cuases:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");

        // following line sets the RTC to the date & time this sketch was compiled
        // it will also reset the valid flag internally unless the Rtc device is
        // having an issue

        Rtc.SetDateTime(compiled);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
//**************RTC END*****************

  // initialize the LED pin as an output:
  pinMode(relay, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin0, INPUT_PULLUP);
    // initialize the pushbutton pin as an input:
  pinMode(buttonPin1, INPUT_PULLUP);
  // Attach an interrupt to the ISR vector
//   attachInterrupt(0, pin_ISR0, LOW);
  // Attach an interrupt to the ISR vector
//   attachInterrupt(1, pin_ISR1, LOW);
}

void loop() {
  // lcd.begin(16,2);
// lcd.setCursor(0,1);

  //****************RTC*****************
if (!Rtc.IsDateTimeValid()) 
    {
        // Common Cuases:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    Serial.println();

    // RtcTemperature temp = Rtc.GetTemperature();
    // Serial.print(temp.AsFloat());
    // Serial.println("C");

    delay(500); // ten seconds
  //************RTC END*****************
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.clear();
  lcdPrintTime(now);

    // read the state of the pushbutton value:
//   buttonState = ;

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
//   if (buttonState == LOW) {
    // turn LED on:
    digitalWrite(relay, digitalRead(buttonPin0));
//   }
//   else {
//     // turn LED off:
//     digitalWrite(relay, LOW);
//   }


  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (digitalRead(buttonPin1) == LOW) {
    lcd.setCursor(14,1);
    lcd.print("Hi");
  }

  if (digitalRead(buttonPin0) == LOW) {
    lcd.setCursor(12,1);
    lcd.print("Buzz");
    shbell();
  }

  int hh = now.Hour();
  int mm = now.Minute();
  int ss = now.Second();

 /*   //Mode Change Code for push button press
  
  buttonState = digitalRead(sw);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH)
    {
     if(mode!=4)
        {mode++;}
     else
     {mode=1;}; 
    }
    }
    
    delay(50);
    lastButtonState = buttonState;*/


mode=1;
//Mode Definition with preset times
  switch (mode) {
    case 1:
      {
        if (hh == 10 && mm == 50 && ss< 2)
          shbell();
        else if (hh == 11 && mm == 00 && ss< 2)
          longbell();
        else if (hh == 12 && mm == 00 && ss< 2)
          shbell();
        else if (hh == 13 && mm == 00 && ss< 2)
          shbell();
        else if (hh == 13 && mm == 50 && ss< 2)
          medbell();
        else if (hh == 14 && mm == 00 && ss< 2)
          longbell();
        else if (hh == 14 && mm == 50 && ss< 2)
          shbell();
        else if (hh == 15 && mm == 00 && ss< 2)
          longbell();
        else if (hh == 16 && mm == 00 && ss< 2)
          shbell();
        else if (hh == 17 && mm == 00 && ss< 2)
          shbell();
        else if (hh == 17 && mm == 50 && ss< 2)
          shbell();
        else if (hh == 18 && mm == 00 && ss< 2)
        longbell();
        else
        off();
      }
        
      break;

    case 2:
      {if (hh == 16 && mm == 30 && ss< 2)
          longbell();
        else if (hh == 16 && mm == 32 && ss< 2)
          medbell();
        else if (hh == 16 && mm == 34 && ss< 2)
          medbell();
        else if (hh == 16 && mm == 36 && ss< 2)
          medbell();
        else if (hh == 16 && mm == 38 && ss< 2)
          medbell();
        else if (hh == 16 && mm == 40 && ss< 2)
          medbell();
        else if (hh == 15 && mm == 30)
          medbell();
        else if (hh == 16 && mm == 300)
          medbell();
        else
        off();}
      break;

    case 3: 
      {
         if (hh == 9 && mm == 15)
          shbell();
        else if (hh == 9 && mm == 20)
          medbell();
        else if (hh == 9 && mm == 30)
          longbell();
        else if (hh == 10 && mm == 00)
          shbell();
        else if (hh == 12 && mm == 20)
          shbell();
        else if (hh == 12 && mm == 30)
         longbell();
        else if (hh == 13 && mm == 45)
          shbell();
        else if (hh == 13 && mm == 50)
          medbell();
        else if (hh == 14 && mm == 00)
          longbell();
        else if (hh == 14 && mm == 30)
          shbell();
        else if (hh == 16 && mm == 50)
          shbell();
       else if (hh == 17 && mm == 00)
        longbell();
        else
        off();
      }
       
      break;

    case 4: 
      {
        if (hh == 9 && mm == 00)
          longbell();
        else if (hh == 9 && mm == 30)
          medbell();
        else if (hh == 13 && mm == 00)
          longbell();
          else if (hh == 14 && mm == 00)
          medbell();
        else if (hh == 17 && mm == 00)
          longbell();
       else 
       off();
      }
       
      break;
  }//switch statement ends here
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}

void lcdPrintTime(const RtcDateTime& now){
    char tbs[16];
    char tbs1[16];

    sprintf(tbs, "%02u/%02u/%04u",now.Day(),now.Month(),now.Year());
    lcd.setCursor(0,0);
    lcd.print(tbs);
    sprintf(tbs1, "%02u:%02u:%02u",now.Hour(),now.Minute(),now.Second());
    lcd.setCursor(0,1);
    lcd.print(tbs1);   
}

void pin_ISR0() {
  buttonState = digitalRead(buttonPin0);
  digitalWrite(relay, buttonState);
}

void pin_ISR1() {
    lcd.setCursor(14,1);
    lcd.print("Hi");
}

void shbell() { 
    digitalWrite(relay, 0);           //Switches relay on
    buzz();
    delay(2000);                     //waits for 5 seconds 
    digitalWrite(relay, 1);           //switches relay off
    delay(10);
    lcd.begin(16, 2);
    delay(10);
}

void medbell() {
    digitalWrite(relay, 0);           //Switches relay on
    buzz();
    delay(4000);                     //waits for 7 seconds 
    digitalWrite(relay, 1);           //switches relay off
    delay(10);
    lcd.begin(16, 2);
    delay(10);
}

void longbell() {
  digitalWrite(relay, 0);           //Switches relay on
  buzz();
  delay(7000);                     //waits for 10 seconds 
  digitalWrite(relay, 1);           //switches relay off
  delay(10);
  lcd.begin(16, 2);
  delay(10);

}

void off() { 
    digitalWrite(relay, HIGH);
}

void buzz(){
  int i;
  Serial.println("Buzz");
  for(i=0;i<2000;i++){
  digitalWrite(buzzer,HIGH);
  delay(1);//wait for 1ms
  digitalWrite(buzzer,LOW);
  delay(1);//wait for 1ms
  }
}