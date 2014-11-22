
/*

An IoT solution for monitoring the price of Bitcoin with Visual and Auditory cues pertaining to the volatility of the the CoinDesk exchange over a given time interval.
 
 Board:
 * Intel Edison
 
 Shield:
 *Grove Base Shield V2
 
 Sensors:
 *Grove LCD RGB Backlight
 
 by Paul DeCarlo
 */


#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
int speakerPin = 3;                  // Grove Buzzer connect to D3

char ssid[] = "YOURNETWORK"; //  your network SSID (name) 
char pass[] = "YOURPASSWORD";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(204,79,197,200);  // numeric IP for Bing (no DNS)
char server[] = "api.coindesk.com";   

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

void setup() {
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  pinMode(speakerPin, OUTPUT);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    lcd.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 

  String fv = WiFi.firmwareVersion();
  if( fv != "1.1.0" )
    lcd.println("Please upgrade the firmware");
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) { 
    WiFi.disconnect();
    lcd.print("Connecting...");
    //lcd.print(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);
  
    // wait 10 seconds for connection:
    delay(8000);
    lcd.clear();
  } 
  lcd.clear();
  lcd.print("Connected");
  delay(2000);
  //printWifiStatus();
 
}

float currentNumericRate = 0;
float lastNumericRate = 0;
float accumulator = 0;
//The closer alpha is to 1.0, the faster the moving average updates in response to new values
float alpha = .15;
float volatilityIndex = 0;
//defined as ratio of Moving average (accumulator) / currentNumericRate
float volatilityAlertThreshold = 7;

void loop() {

  lcd.setCursor(0,0);
  lcd.clear();  
    
  //Get BTC Price  
  char* result = makeWebRequest();
  
  //Begin Parsing
  char* rateBegin = strstr(result, "\"rate\"");
  char* rate = parseJson(rateBegin);
  currentNumericRate = atof(rate);
  
  //Print Output
  lcd.print("Current:");
  lcd.print(currentNumericRate);
  
  //Begin Processing
  if(lastNumericRate == 0)
    lastNumericRate = currentNumericRate;
    
  if(accumulator == 0)
    accumulator = currentNumericRate;

  //Begin calculation of moving average http://stackoverflow.com/questions/10990618/calculate-rolling-moving-average-in-c-or-c
  accumulator = (alpha * currentNumericRate) + (1.0 - alpha) * accumulator;
  
  volatilityIndex = ((float)currentNumericRate / accumulator);
  
  lcd.setCursor(0,1);
  lcd.print("A:");
  lcd.print(accumulator);
  
  //Begin alert processing
  lcd.print(" ");
  lcd.print(volatilityIndex);
  lcd.print("%");
    
  if(volatilityIndex <=  (volatilityAlertThreshold * -1)) //falling knife
    playNote('d', 5000);
  else if (volatilityIndex >= volatilityAlertThreshold)  //to the moon
    playNote('c', 5000);
  
  //Begin consecutive trend anaylysis
  if(lastNumericRate > currentNumericRate)
    lcd.setRGB(100,0,0);
  else if(lastNumericRate < currentNumericRate)
    lcd.setRGB(0,100,0);
  
  lastNumericRate = currentNumericRate;
  
  //Poll every 30s
  delay(30000);
}

char* parseJson(char *p)
{
  int index = 0;
  while(p[index] != ':')
    index++;
      
  index = index + 2;
  int start = index;
  
  while(p[index] != '"')
    index++;
  
  int fin = index;    
  
  int len = fin - start;
  
  static char result[30];
  strncpy(result, p+start, len); 
  
  result[len+1] = '\0';
  
  return result;
}

char* makeWebRequest(){
  
  static char result[1000];
  
   if (client.connect(server, 80)) 
   {
      // Make a HTTP request:
      client.println("GET /v1/bpi/currentprice/USD.json HTTP/1.1");
      client.println("Cache-Control: max-age=0");
      client.println("Host: api.coindesk.com");
      client.println("Connection: close");
      client.println();
      
      // if there are incoming bytes available 
      // from the server, read them and print them:
      int index = 0;
      while (client.available()) 
      {
        result[index] = client.read();
        index++;
      } 
      result[index] = '\0';
      client.stop();
   }  

  return result;
}

void playTone(int tone, int duration) {
    for (long i = 0; i < duration * 1000L; i += tone * 2) {
        digitalWrite(speakerPin, HIGH);
        delayMicroseconds(tone);
        digitalWrite(speakerPin, LOW);
        delayMicroseconds(tone);
    }
}

void playNote(char note, int duration) {
    char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
    int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

    // play the tone corresponding to the note name
    for (int i = 0; i < 8; i++) {
        if (names[i] == note) {
            playTone(tones[i], duration);
        }
    }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  lcd.print("SSID: ");
  lcd.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  lcd.print("IP Address: ");
  lcd.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  lcd.print("signal strength (RSSI):");
  lcd.print(rssi);
  lcd.println(" dBm");
}





