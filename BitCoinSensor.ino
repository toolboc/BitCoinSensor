
/*

An IoT solution for monitoring the price of Bitcoin with Visual and Auditory cues pertaining to the volatility of the the CoinDesk exchange over a given time interval.
 
 Board:
 *Intel Edison
 
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

char ssid[] = "YOURNETWORK"; //  your network SSID (name) 
char pass[] = "YOURPASSWORD";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "api.coindesk.com";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

void setup() {
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
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

float lastNumericRate = 0;

void loop() {

lcd.clear();  
  
char* result = makeWebRequest();

//char* dateBegin = strstr(result, "\"updated\"");
//char* date = parseJson(dateBegin);
//lcd.print(date);

char* rateBegin = strstr(result, "\"rate\"");
char* rate = parseJson(rateBegin);
float currentNumericRate = atof(rate);

lcd.print(currentNumericRate);

if(lastNumericRate == 0)
  lastNumericRate = currentNumericRate;

if(lastNumericRate > currentNumericRate)
  lcd.setRGB(255,0,0);
else if(lastNumericRate < currentNumericRate)
  lcd.setRGB(0,255,0);

lastNumericRate = currentNumericRate;

delay(10000);
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





