#include <ArduinoJson.h>
#include <NTPClient.h>
#include <FirebaseJson.h>
#include <WiFiUdp.h>
#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>

#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>                                                    // esp8266 library                                               // firebase library
#include "DHT.h"                                                    // dht11 temperature and humidity sensor library

#define FIREBASE_HOST "realtime-temp-and-humidity.firebaseio.com"                          // the project name address from firebase id
#define FIREBASE_AUTH "pSvZ2eR8wHzjkJeFXI07Jsq5GYGRgwgXXf7OKylu"            // the secret key generated from firebase

#define WIFI_SSID "522"                                             // input your home or public wifi name 
#define WIFI_PASSWORD "bedemanme522"// what digital pin we're connected to
#define DHTPIN D4
#define DHTTYPE DHT11                                                       // select dht type as DHT 11 or DHT22
DHT dht(DHTPIN, DHTTYPE);                                                     
const long utcOffsetInSeconds = 25200;
char daysOfTheWeek[7][12] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
String months[12]={"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
String DHT_Status = "";
void setup() {
  Serial.begin(9600);
  delay(1000);                
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                            //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                              // connect to firebase
  dht.begin();                                                               //Start reading dht sensor
  timeClient.begin();
}

void loop() { 
  delay(10000);
  DHT_Status = Firebase.getString("DHT_Status"); 
  if(DHT_Status == "start"){
  float h = dht.readHumidity();                                              // Reading temperature or humidity takes about 250 milliseconds!
  float t = dht.readTemperature();                                           // Read temperature as Celsius (the default)
    
  if (isnan(h) || isnan(t)) {                                                // Check if any reads failed and exit early (to try again).
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;


  int currentMonth = ptm->tm_mon+1;


  String currentMonthName = months[currentMonth-1];


  int currentYear = ptm->tm_year+1900;

  Serial.print("Humidity: ");  Serial.print(h);
  String fireHumid = String(h) + String("%") + " TimeStamp:" + daysOfTheWeek[timeClient.getDay()]+"," + String(monthDay)+" " + currentMonthName + " " + String(currentYear) +" "+timeClient.getHours() + ":"+timeClient.getMinutes() + ":" +timeClient.getSeconds();                                         //convert integer humidity to string humidity 
  Serial.print("%  Temperature: ");  Serial.print(t);  Serial.println("°C ");
  String fireTemp = String(t) + String("°C")+ " TimeStamp:" + daysOfTheWeek[timeClient.getDay()]+"," + String(monthDay)+" " + currentMonthName + " " + String(currentYear) +" "+timeClient.getHours() + ":"+timeClient.getMinutes() + ":" +timeClient.getSeconds();                                                     //convert integer temperature to string temperature
  
  timeClient.update();
  
  Firebase.pushString("/DHT11/Humidity", fireHumid);                                  //setup path and send readings
  Firebase.pushString("/DHT11/Temperature", fireTemp);                                //setup path and send readings
   }
}
