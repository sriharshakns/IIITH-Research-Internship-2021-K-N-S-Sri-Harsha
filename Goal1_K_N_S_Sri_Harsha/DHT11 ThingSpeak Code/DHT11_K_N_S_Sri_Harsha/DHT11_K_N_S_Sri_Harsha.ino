//The Connections needed to implement this code are D3/GPIO 0 pin of Nodemcu to Data Pin of DHT Sensor & Vcc & GND respectively.
//Transmitting DHT Sensor Data to ThingSpeak using Nodemcu
#include <DHT.h>                                                      // Including the library of the DHT11 temperature and humidity sensor 
#include <ESP8266WiFi.h>                                             //To use the Wi-Fi library
 
String apiKey = "";                                 //  Enter your Write API key from ThingSpeak

const char *ssid =  "HARSHA";                                       // Replace with your wifi ssid and wpa2 key
const char *pass =  "welcome123";
const char* server = "api.thingspeak.com";

#define DHTPIN 0                                                   //Digital Pin where the DHT11 is connected
 
DHT dht(DHTPIN, DHT11);                                           //Instantiating the DHT dht with Digital Pin and DHT sensor type

WiFiClient client;
 
void setup() 
{
       Serial.begin(115200);                                      //To begin the serial communication.
       delay(10);                                                 //To give an appropriate delay
       dht.begin();
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);                                   //To connect Nodemcu to Wifi
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 
}
 
void loop() 
{
  
      float h = dht.readHumidity();                                     //Using float variables to read the temperature and humidity values from the sensor
      float t = dht.readTemperature();
      float hi = dht.computeHeatIndex(t,h,false);
      
              if (isnan(h) || isnan(t) || isnan(hi)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }

                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com is the server
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(hi);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");                                      //Updating the Data to Server in respective fields
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.print("%. Heat Index: ");
                             Serial.print(hi);
                             Serial.println(". Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, I've set it to 30 seconds
  delay(10000);
}
