/*

An IoT solution for monitoring the price of Bitcoin with Visual and Auditory cues pertaining to the volatility of the the CoinDesk exchange over a given time interval.
 
 Board:
 * LinkIt One
 
 Sensors:
 * Grove LCD RGB Backlight
 
 Hardware:
 * HXD Piezo Buzzer
 
 by Paul DeCarlo
 */

#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>
#include <Wire.h>
#include "rgb_lcd.h"

#define WIFI_AP "YOURNETWORK"
#define WIFI_PASSWORD "YOURPASSWORD"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define SITE_URL "api.coindesk.com"

LWiFiClient client;
rgb_lcd lcd;
int speakerPin = 12; // Grove Buzzer connect to D12

float currentNumericRate = 0;
float lastNumericRate = 0;
float accumulator = 0;
//The closer alpha is to 1.0, the faster the moving average updates in response to new values
float alpha = .15;
float volatilityIndex = 0;
//Defined as ratio of currentNumericRate / Moving average (accumulator)
float volatilityAlertThreshold = .01; //Price of BTC is moving at a full percent away from Moving average (accumulator) in consecutive polls, indicates HIGH volatility

void setup()
{
  LWiFi.begin();
  lcd.setCursor(0,0);
  lcd.begin(16, 2);
  Serial.begin(115200);

  pinMode(speakerPin, OUTPUT); 
  playNote('c', 500); 
  playNote('d', 500);
  playNote('e', 500);
  playNote('f', 500);
  playNote('g', 500);
  playNote('a', 500);
  playNote('b', 500);
  playNote('C', 1000); 
 

  // keep retrying until connected to AP
  lcd.clear();
  lcd.println("Connecting");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
    delay(1000);
  }
}

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
  
  volatilityIndex = ((float)currentNumericRate / accumulator) - 1;
  
  lcd.setCursor(0,1);
  lcd.print("A:");
  lcd.print(accumulator);
  
  //Begin alert processing
  lcd.print(" ");
  lcd.print(volatilityIndex * 100);
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
  
    // keep retrying until connected to website
  while (0 == client.connect(SITE_URL, 80))
  {
    delay(1000);
  }

  // send HTTP request, ends with 2 CR/LF
  client.println("GET /v1/bpi/currentprice/USD.json HTTP/1.1");
  client.println("Cache-Control: max-age=0");
  client.println("Host: " SITE_URL);
  client.println("Connection: close");
  client.println();    
      
  // if there are incoming bytes available 
  // from the server, read them and print them:
  while (!client.available())
  {
    delay(100);
  }
  
  int index = 0;
  
  while (client.available()) 
  {
    result[index] = client.read();
    index++;
  } 
    result[index] = '\0';
    client.stop();  

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
