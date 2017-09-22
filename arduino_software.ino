
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SD.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <Keypad.h>



// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress dnServer(8, 8, 8, 8);
// the router's gateway address:
IPAddress gateway(10, 0, 1, 1);

// the subnet:
IPAddress subnet(255, 255, 255, 0);

//the IP address is dependent on your network
IPAddress ip(10, 0, 1, 55);


//mqtt server address
IPAddress mqtt_server(aaa,bbb,ccc,ddd);


LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack

#define DS3231_I2C_ADDRESS 0x68


EthernetClient ethClient;
PubSubClient client(ethClient);


///Temperature monitors
#define ONE_WIRE_BUS 9

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//Probes
DeviceAddress ProbeAir = { 0x28, 0xCC, 0xEB, 0x03, 0x00, 0x00, 0x80, 0x52 }; 
DeviceAddress ProbeWort = { 0x28, 0xFF, 0x5C, 0x1E, 0x80, 0x16, 0x05, 0xCA }; 

//relays
const int cold_relay = 6;
const int hot_relay = 7;

//max-min and optimal temps
const float MaxAllowedTemperature = 28.00;
const float MinAllowedTemperature = 18.00;
const float target_temprerature = 25.00;

boolean Cold = 0;
boolean Hot = 0;

float wort_temp_sum = 0.0;
float air_temp_sum = 0.0;
int readings = 0;

unsigned long epoch;

//for the bubble counter, I'm aware of the typos :D
int boubbles = 0;

// Connect the opto signal pin to the first INT pin (digital pin 2 on an Arduino Uno)
#define resolution 800000 //only capture pulse that are greater than this to avoid noise
#define sample 4 // sample size of pulses for average starts at 0

volatile unsigned long pulseThen;
volatile unsigned long pulseNow;
volatile unsigned long numPulses;
volatile unsigned long keeppulse[sample];
volatile int count=0;

void isr() {
    boubbles +=1;
 /* 
  
  //for the future
 unsigned long now = micros();
 float pulselength;
 pulseNow = now;
 int averagepulse;
 int totalsample = 0;


 if ((pulseNow - pulseThen) > resolution)
 {
  //boubbles +=1;
   
   pulselength = (float)(pulseNow - pulseThen) /1000000;
   if( count < sample ) {
    keeppulse[count] = pulselength;
    count++;
   }
   else {
    count = 0;
    for( int x=0; (x<sample); x++ ) {
      totalsample += (int)keeppulse[x];
     }
    averagepulse = totalsample/sample;
    }

   pulseThen = pulseNow;
   ++numPulses;
 }
 */
}



void setup() {
  
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  
    Serial.println("\nGARAGARDUINO v3.0\n");
    
    // activate LCD module
    Serial.println("LCD init...");
    lcd.begin (20,4); // for 20 x 4 LCD module
    lcd.setBacklightPin(3,POSITIVE);
    lcd.setBacklight(HIGH);
    
    
    lcd.home(); 
    lcd.print("GARAGARDUINO 3.0"); 
    lcd.setCursor (0,1);
    lcd.print("Ethernet Init..."); 
    
    
    Serial.println("Ethernet init...");
    //Ethernet.begin(mac);
    
    Ethernet.begin(mac, ip, dnServer, gateway, subnet);
  //print out the IP address
    Serial.print("IP = ");
    Serial.println(Ethernet.localIP());
    delay(900);
    
    lcd.clear();
    lcd.home(); 
    lcd.setCursor (0,1);
    lcd.print("Time Init..."); 
    
    
    Serial.println("Time init...");
    // DS3231 seconds, minutes, hours, day, date, month, year
    //setDS3231time(30,48,18,3,24,5,16);
   // Udp.begin(8888);
   // epoch = updateNTPdate();
    delay(900);

    lcd.clear();
    lcd.home(); 
    //lcd.print("NTP Init..."); 
    lcd.setCursor (0,1);
    lcd.print("SD Init..."); 

    Serial.println("SD init...");
    pinMode(10, OUTPUT);
    SD.begin(4);
    delay(900);
    
    lcd.clear();
    lcd.home(); 
    lcd.print("SD Init..."); 
    lcd.setCursor (0,1);
    lcd.print("Relays Init..."); 

    Serial.println("Relays init...");
    pinMode(cold_relay, OUTPUT);
    pinMode(hot_relay, OUTPUT);
    delay(900);
    
    
    lcd.clear();
    lcd.home(); 
    lcd.print("Relays Init..."); 
    lcd.setCursor (0,1);
    lcd.print("Sensors Init..."); 

    Serial.println("Sensors init...");
    sensors.begin();
    sensors.setResolution(ProbeAir, 10);
    sensors.setResolution(ProbeWort, 10);
    analogReference(INTERNAL1V1);
    delay(900);
    
    lcd.clear();
    lcd.home(); 
    lcd.print("MQTT Init..."); 
    client.setServer(mqtt_server, 1883);
    client.setCallback(mqtt_callback);
    delay(900);

    lcd.clear();
    lcd.home(); 
    lcd.print("Boubles Init..."); 
    pinMode(19, INPUT); //probe output on pin 18
    attachInterrupt(digitalPinToInterrupt(19), isr, RISING);
    numPulses = 0;                      // prime the system to start a new reading
    delay(900);

    lcd.clear();
    lcd.home(); 
    lcd.print("Init done!!!"); 
    lcd.setCursor (0,1);
    lcd.print("Let's brew!!!"); 
    delay(2000);

}

void loop() {
  sensors.requestTemperatures();
  
  float wort_temperature = sensors.getTempC(ProbeWort);
  float air_temperature = sensors.getTempC(ProbeAir);
  
  int multiplier = 0;

  //no sensor attached!
  if (wort_temperature == -127.00) {
      //Serial.println("No wort sensor attached!");
      wort_temperature = 0.0;
      multiplier += 1;
  }

  if (air_temperature == -127.00) {
      //Serial.println("No air sensor attached!");
      air_temperature = 0.0;
      multiplier += 1;

  }
  float current_temperature  = 0.0;
  
  switch (multiplier) {
    case 0:
        current_temperature = (air_temperature + wort_temperature) / 2;
        break;

    default:
        current_temperature = (air_temperature + wort_temperature);
        break;
  }

  float current_temperature = (air_temperature + wort_temperature) / 2;
  air_temp_sum += air_temperature;
  wort_temp_sum += wort_temperature;
  readings +=1;
  
  if (current_temperature > MaxAllowedTemperature) {
    //We turn the cold relay on
    if (current_temperature >= target_temprerature) {
      Cold = 1;
    }
  }

  if (current_temperature < MinAllowedTemperature) {
    //We turn the hot relay on
    if (current_temperature <= target_temprerature) {
      Hot = 1;
    }
  }

  //we turn off the hot/cold systems only when the optimal temperature is reached

  if (Cold == 1) {
    if (current_temperature < target_temprerature) {
      Serial.println(String(current_temperature) +" < "+ String(target_temprerature));
      Cold = 0;
    }
  } else {
    digitalWrite(cold_relay, LOW);
  }

  if (Hot == 1) {
    if (current_temperature > target_temprerature) {
      Serial.println(String(current_temperature) +" > "+ String(target_temprerature));
      Hot = 0;
    }
  } else {
    digitalWrite(hot_relay, LOW);
  }

  if (Hot == 1 && Cold == 0) {
    digitalWrite(cold_relay, LOW); //we make sure that the two systems aren't on at the same time
    digitalWrite(hot_relay, HIGH);

  }

  if (Hot == 0 && Cold == 1) {
    digitalWrite(hot_relay, LOW); //we make sure that the two systems aren't on at the same time
    digitalWrite(cold_relay, HIGH);
  }
  
  //boubble counter
  int c = 0;
  boubbles = 0;

  for (c=0;c<5000;c+=1) {
    int ir = analogRead(ir_pin);
    if (ir < 900) {
        boubbles+=1;
        delay(25);
    } 
  }

  lcd.clear();
  lcd.home(); 
  lcd.print("Brewing  H:" + String(Hot) + " C:" + String(Cold)); 
  lcd.setCursor (0,1);
  lcd.print("W:" + String(wort_temperature) + "  A:" + String(air_temperature)); 
  lcd.setCursor (0,2);
  lcd.print("M:" + String(current_temperature) + " " + String(MinAllowedTemperature)+ "-" + String(MaxAllowedTemperature) ); 
  lcd.setCursor (0,3);
  lcd.print("BOUBLES: " + String(boubbles)); 


  //Serial.println("Wort:" + String(wort_temperature) + "  Air:" + String(current_temperature));
  //Serial.println("Hot:" + String(Hot) + " Cold:" + String(Cold)); 


  File logFile = SD.open("dev.csv", FILE_WRITE);
  logFile.println(displayTime() + "-" + String(current_temperature) + "|" + String(air_temperature) + "|" + String(Hot) + "|" + String(Cold) + "|" + String(boubbles));
  logFile.close();

  
  char message_buff[255];
  //String pubString = "{\"fermentation\": {\"air_temp\": \"" + String(current_temperature) + "\",\"wort_temp\": \"" + String(wort_temperature) + "\",\"hot_relay\": \"" + String(Hot) + "\",\"cold_relay\": \"" + String(Cold) + "\"}}";
  String pubString = String(wort_temperature) + "," + String(air_temperature) + "," + String(Hot) + "," + String(Cold) + "," + String(boubbles);
  pubString.toCharArray(message_buff, pubString.length()+1);
  
  if (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("garagarduino","garagarduino","changeme")) {
      Serial.println("connected");
      // Once connected, publish an announcement...      
      client.publish("garagarduino",message_buff);
      // ... and resubscribe
      //client.subscribe("inTopic");
    }
  } else {
    client.publish("garagarduino",message_buff);
  }
  client.loop();
  boubbles = 0;

  // wait ten seconds before asking for the time again
  delay(10000);
  
}


/*
 * Time
 */


void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
String displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,&year);
  // send it to the serial monitor
 
  return (String(year) + "-" + String(month) + "-" + String(dayOfMonth) + " " + String(hour) + ":" + String(minute) + ":" + String(second));

}

byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}

/*
 * MQTT
 */

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}





