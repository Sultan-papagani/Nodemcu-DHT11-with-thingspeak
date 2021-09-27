#include <DHT.h>  
#include <ESP8266WiFi.h>
#define DHTPIN 5   // dht data pin to D1 (gpio5) 
                   // dht 11 was powered by 3.3v
                   // no resistor needed

// for deep sleep to work; 
// connect D0 with RST pin (with a jumper cable or any cable)
// also you can check this website 
// https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/

// [thingspeak] on Channel Settings you need two Fields first for temp. second for humidty

String apiKey = "thingspeak api key";     // your api key 
const char *ssid =  "wifi name";          // your wifi name
const char *pass =  "wifi password";"     // your wifi password
const char* server = "api.thingspeak.com";// no need to change

float t, h;
     
DHT dht(DHTPIN, DHT11); // change this line if your sensor is dht22
WiFiClient client;

void setup() {
   dht.begin();
   WiFi.hostname("This name will show up when nodemcu connected to wifi"); // give it a simple and short name
   WiFi.begin(ssid, pass);
   while (WiFi.status() != WL_CONNECTED) 
   {
      delay(100); // do not remove this delay (it will cause boot loop)
   }
   h = dht.readHumidity();
   t = dht.readTemperature();
   if (isnan(h) || isnan(t)) 
   {
      SendMessage(0.0f, 0.0f);  // if dht11 cant return value send 0.0f to thingspeak (for debug maybe)
   }
   else
   {
      SendMessage(t, h);
   }
   ESP.deepSleep(6e+8);
  // 6e+8 = 10 minutes
  // 3e+8 = 5 minutes 
  // to convert any value google " x minutes to microseconds"
}

void SendMessage(float tp, float hm)
{
   if (client.connect(server,80))
    {                     
       String postStr = apiKey;
       postStr +="&field1=";  // first field
       postStr += String(tp);
       postStr +="&field2=";  // second field
       postStr += String(hm);
       postStr += "\r\n\r\n";
     
       // you can add fields and send more data
    
       client.print("POST /update HTTP/1.1\n");
       client.print("Host: api.thingspeak.com\n");
       client.print("Connection: close\n");
       client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
       client.print("Content-Type: application/x-www-form-urlencoded\n");
       client.print("Content-Length: ");
       client.print(postStr.length());
       client.print("\n\n");
       client.print(postStr);
       delay(500);
       client.stop();
    }                  
}

void loop() {
  // no loop 
  // nodemcu will execute start function, and deep sleep 10 minutes. then restart itself with D0 -> RST pin cable
}
