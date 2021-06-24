#include <WiFi101.h>
#include "ThingSpeak.h"
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SECRET_SSID "ADCON1"    //WiFi network name
#define SECRET_PASS "Shadow10113"  //WiFi password

#define SECRET_CH_ID 1277292     //channel number
#define SECRET_WRITE_APIKEY "I4BV5Q70NNDWH0SP"   //channel write API Key

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

int tempF = 0; // temperature variable

void setup() {
  Serial.begin(115200);      // Initialize serial 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  } 
  
  dht.begin();
}

void loop() {

  ThingSpeak.begin(client);  // Initialize ThingSpeak 

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  
  tempF = dht.readTemperature(true);

  // Write to ThingSpeak
  int x = ThingSpeak.writeField(myChannelNumber,  1, tempF, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  //Debugging
//  Serial.print("Temperature (Farenheit) = ");
//  Serial.println(tempF);

  delay(18000); // Wait 5 minutes to update the channel again
}
