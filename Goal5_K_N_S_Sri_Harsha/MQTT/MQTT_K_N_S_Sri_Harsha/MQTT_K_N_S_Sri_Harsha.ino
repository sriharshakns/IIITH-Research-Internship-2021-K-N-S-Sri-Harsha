#include <PubSubClient.h>                //To include all the necessary libraries
#include <DHT.h>
#include <ESP8266WiFi.h>    

#define DHTPIN 0                         // DHT Sensor Data Pin connected to digital pin D3.
#define DHTTYPE DHT11                      // Type of DHT sensor.
#define WATERPIN A0                        // Water Level Sensor Data Pin connected to analog pin A0.

DHT dht(DHTPIN, DHTTYPE); 
  
// Wifi Credentials: Network SSID & Password.
char ssid[] = "HARSHA";          
char pass[] = "welcome123";  
        
//MQTT Broker Settings & Topics
char mqttUserName[] = "Water Level Sensing";  // Use any name.
char mqttPass[] = "";      // MQTT API Key from Thingspeak Account > MyProfile.  

// Published Settings
char writeAPIKey[] = "";    // Channel write API key.
long channelID = ;                    // Channel ID.

//Subscriber Settings (demo only).
long subscribeChannelID_1 = 1371607;
String subscribeForTopic_Command_1="channels/" + String(subscribeChannelID_1)+ "/subscribe/fields/field1";

WiFiClient client;           
PubSubClient mqttClient(client);              // Initialize the PubSubClient library.
const char* server = "mqtt.thingspeak.com";
unsigned long lastConnectionTime = 0;         //To control the delay between Publishes
const unsigned long postingInterval = 20L * 1000L;

void setup() {
  
 Serial.begin(9600);
 WiFi.begin(ssid, pass);                 //To connect Nodemcu to Wifi
 
 while (WiFi.status() != WL_CONNECTED) 
 {
   delay(500);
   Serial.print(".");
 }
  Serial.println("");
  Serial.println("WiFi connected");
  mqttClient.setServer(server, 1883);   // Set the MQTT broker details.
}
void loop() 
{

  if (!mqttClient.connected())        // Reconnect if MQTT client is not connected.
  {
  char clientID[] ="ESP_0000000000";
  while (!mqttClient.connected()) 
  {
    Serial.print("Attempting MQTT connection...\n");
  for (int i=4;i<14;i++)
  {
    clientID[i]= char(48+random(10));
  }
    // Connect to the MQTT broker.
    if (mqttClient.connect(clientID,mqttUserName,mqttPass)) 
    {
      Serial.println("Successfully Connected to Broker");
      Serial.println("Client ID is " + String(clientID));
    } 
    else 
    {
      Serial.print("Broker Connection Failed, Trying Again in 5 Seconds\n");
      delay(5000);
    }
  }
}

  mqttClient.loop();   // Call the loop continuously to establish connection to the server.

  // If interval time has passed since the last connection, publish data to ThingSpeak.
  if (millis() - lastConnectionTime > postingInterval) 
  {
   int Humidity = dht.readHumidity();  // Read humidity from DHT sensor.
   int waterLevel = analogRead(WATERPIN); // Read voltage from light sensor.
  
  // Create data string to send to ThingSpeak.
  String data = String("field1=") + String(Humidity, DEC) + "&field2=" + String(waterLevel, DEC);
  int length = data.length();
  const char *msgBuffer;
  msgBuffer=data.c_str();
  Serial.println(msgBuffer);
  
  // Create a publishTopic string and publish data to ThingSpeak channel feed. 
  String publishTopic = "channels/" + String( channelID ) + "/publish/"+String(writeAPIKey);
  length = publishTopic.length();
  const char *topicBuffer;
  topicBuffer = publishTopic.c_str();
  boolean result=mqttClient.publish( topicBuffer, msgBuffer );
  if(result) Serial.println("Data Published Successfully");
  else Serial.println("Unable to Publish Data");
  
  lastConnectionTime = millis();
  }
}

 
