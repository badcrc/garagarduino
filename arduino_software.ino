/*

00	INIT
01	NET INIT
02	NTP INIT
03	SD INIT
10	HIGH TEMP
11	LOW TEMP
91  NET INIT FAILED
31  SD INIT FAILED

*/




#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SD.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress dnServer(8, 8, 8, 8);
// the router's gateway address:
IPAddress gateway(192, 168, 1, 1);
// the subnet:
IPAddress subnet(255, 255, 255, 0);

//the IP address is dependent on your network
IPAddress ip(192, 168, 1, 55);

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack

#define DS3231_I2C_ADDRESS 0x68


const int NTP_P_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_P_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

EthernetClient client;


unsigned long httplastConnTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long httppostInterv = 60L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers


///Temperature monitors
const int tempPin = A2;

#define ONE_WIRE_BUS 9

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

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


void setup() {
  
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  
    Serial.println("\nGARAGARDUINO v2.0\n");
    
    // activate LCD module
    Serial.println("LCD init...");
    lcd.begin (16,2); // for 16 x 2 LCD module
    lcd.setBacklightPin(3,POSITIVE);
    lcd.setBacklight(HIGH);
    
    
    lcd.home(); 
    lcd.print("GARAGARDUINO 2.0"); 
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
    lcd.print("Ethernet Init..."); 
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
    lcd.print("NTP Init..."); 
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
    analogReference(INTERNAL1V1);
    delay(900);
    
    lcd.clear();
    lcd.home(); 
    lcd.print("Init done!!!"); 
    lcd.setCursor (0,1);
    lcd.print("Let's brew!!!"); 
    delay(2000);

}

void loop() {
   
  
  


  float val_temp = analogRead(tempPin);
  sensors.requestTemperatures();

  float wort_temperature = sensors.getTempCByIndex(0);
  

  float current_temperature = (1.1 * val_temp * 100.0) / 1024;
  
  air_temp_sum += current_temperature;
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
  
  lcd.clear();
    lcd.home(); 
    lcd.print("Brewing  H:" + String(Hot) + " C:" + String(Cold)); 
    lcd.setCursor (0,1);
    lcd.print("W:" + String(wort_temperature) + "  A:" + String(current_temperature)); 

  //Serial.println("Wort:" + String(wort_temperature) + "  Air:" + String(current_temperature));
  //Serial.println("Hot:" + String(Hot) + " Cold:" + String(Cold)); 


  File logFile = SD.open("dev.csv", FILE_WRITE);
  logFile.println(displayTime() + "-" + String(current_temperature) + "|" + String(wort_temperature) + "|" + String(Hot) + "|" + String(Cold));
  logFile.close();

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  
  // todo: no mandar el dato puntual si no ir guardando el minuto completo en memoria y mandar la media del minuto
  if (millis() - httplastConnTime > httppostInterv) {
    
    httpRequest("/g.php?" + String((air_temp_sum/readings)) + "|" + String((wort_temp_sum/readings)) + "|" + String(Hot) + "|" + String(Cold));
    
    air_temp_sum = 0.0;
    wort_temp_sum = 0.0;
    readings=0;

  }

  // wait ten seconds before asking for the time again
  delay(10000);
  
}



// this method makes a HTTP connection to the server:
void httpRequest(String request) {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  lcd.clear();
   lcd.home(); 
    lcd.print("Sending http..."); 
    
  // if there's a successful connection:
  if (client.connect("AAA.BBB.CCC.DDD.", 80)) {
    // send the HTTP PUT request:
    client.println("GET " + requestr + " HTTP/1.1");
    client.println("Host: YOUR.HOSTNAME.TLD");
    client.println("User-Agent: garagarduino");
    client.println("Connection: close");
    client.println();   
    // note the time that the connection was made:
    httplastConnTime = millis();
  } 
  
  lcd.setCursor (0,1);
  lcd.print("Done!!");
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(char* address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_P_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_P_SIZE);
  Udp.endPacket();
}


unsigned long updateNTPdate() {
    sendNTPpacket("time.nist.gov"); // send an NTP packet to a time server
    
    // wait to see if a reply is available
    delay(500);
    if ( Udp.parsePacket() ) {
      // We've received a packet, read the data from it
      Udp.read(packetBuffer, NTP_P_SIZE); // read the packet into the buffer

      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      return (secsSince1900 - seventyYears);
    }
  
}



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








