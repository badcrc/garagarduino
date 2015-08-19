




/*
  Based on SD Data logger

 */

//#include <LiquidCrystal.h>
#include <LiquidCrystal595.h>


// initialize the library with the numbers of the interface pins
LiquidCrystal595 lcd(2,3,4);     // just 3 pins! datapin, latchpin, clockpin

//previous LCD with 6 pins
//LiquidCrystal lcd(7, 6, 5, 4, 3, 2);




//for SD Dataloger shield
#include <SPI.h>
#include <SD.h>

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;



//for OneWire sensor (ds18b20)
#include <OneWire.h>
#include <DallasTemperature.h>


// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 10;


///Temperature monitors
const int tempPin = A2;

#define ONE_WIRE_BUS 9


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


//relays
const int cold_relay = 6;
const int hot_relay = 7;

int Cold_on = 0;
int Hot_on = 0;

//max-min and optimal temps
const float MaxAllowedTemp = 28.00;
const float MinAllowedTemp = 18.00;
const float TargetTemp = 25.00;

float PrevTemp = 0.00;

int cnt=1;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  analogReference(INTERNAL);
  /*
    SD Card
  */

  //Serial.print("Initializing SD card...");

  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    //Serial.println("Card failed, or not present");
    // don't do anything more:
    return; //todo: cambiar por un exit o similar
  }
  //Serial.println("card initialized.");

  /*
     RTC Clock
  */

  Wire.begin();
  RTC.begin();
  //Serial.print("Setting up RTC clock...");

  if (! RTC.isrunning()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // uncomment it & upload to set the time, date and start run the RTC!
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  // Serial.println("RTC initialized.");
  //Serial.println();

  /*
    Relay PINs
  */
  
  pinMode(cold_relay,OUTPUT);
  pinMode(hot_relay,OUTPUT);


  /*
  LCD
  */
  lcd.begin(16, 2);
  
  /*
  OneWire
  */
  
  sensors.begin();


}

void loop() {


  //Actual time
  DateTime now = RTC.now();

  //LFormating it for the log
  String date_now = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day());
  String time_now = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  String dataString = date_now + " " + time_now + ",";

  float val_temp = analogRead(tempPin);
  sensors.requestTemperatures();
  
  float wort_temp = sensors.getTempCByIndex(0);
    
  float current_temp = (1.1 * val_temp * 100.0) / 1024;
  
  
  

  String dir = "";

  if (current_temp > PrevTemp) {
    dir = "^";
  } else if (current_temp < PrevTemp) {
    dir = "v";
  } else if (current_temp == PrevTemp) {
    dir = "=";
  }

  PrevTemp = current_temp;


  
  lcd.clear();
  lcd.setCursor(0, 0);
  
  if(cnt==1) {
      lcd.print(String(MinAllowedTemp) + "<---->" + String(MaxAllowedTemp));
      cnt = 0;
  } else if(cnt==0) {
     //lcd.print("A:" + String(current_temp) + dir + " W:" + String(wort_temp));
     
     lcd.print("A:" + String(current_temp) + "  W:" + String(wort_temp));
     cnt = 1;
  }



  lcd.setCursor(0, 1);
  lcd.print("                ");

  if (current_temp > MaxAllowedTemp) {
    Serial.println("TEMP WARNING: HIGH");

    //We turn the cold relay on
    if(current_temp >= TargetTemp) {
          Cold_on = 1;
    } 
    
  } 

  if (current_temp < MinAllowedTemp) {
    Serial.println("TEMP WARNING: LOW");
    
    //We turn the hot relay on
    if(current_temp < TargetTemp) {
          Hot_on = 1;
    } 

  } 
  
  //we turn off the hot/cold systems only when the optimal temperature is reached
  
  if(Cold_on == 1) {
    if(current_temp < TargetTemp) {
          Cold_on = 0;
    }
  } 
  
  if(Hot_on == 1) {
    if(current_temp > TargetTemp) {
          Hot_on = 0;
    }
  } 
  
  


  if (Hot_on == 1 && Cold_on==0) {
      lcd.setCursor(0, 1);
      lcd.print("HOT:ON");
      digitalWrite(cold_relay,LOW); //we make sure that the two systems aren't on at the same time
      digitalWrite(hot_relay,HIGH);
  } 

  if (Hot_on == 0 && Cold_on==1) {
      lcd.setCursor(8, 1);
      lcd.print("COLD:ON");
      digitalWrite(hot_relay,LOW); //we make sure that the two systems aren't on at the same time
      digitalWrite(cold_relay,HIGH);

  }  
  
  if (Hot_on == 0) {
      lcd.setCursor(0, 1);
      lcd.print("HOT:OFF");
      digitalWrite(hot_relay,LOW);
  }
  
  if (Cold_on == 0) {
      lcd.setCursor(8, 1);
      lcd.print("COLD:OFF");
      digitalWrite(cold_relay,LOW);
  }  


  dataString += String(current_temp) + "," + String(wort_temp) + ",";

  dataString += String(Hot_on) + "," + String(Cold_on);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.csv", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }



  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.csv");
  }

  delay(1500);
}
