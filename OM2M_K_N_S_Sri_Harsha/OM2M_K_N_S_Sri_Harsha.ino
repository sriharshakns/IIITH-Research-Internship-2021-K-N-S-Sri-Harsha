//Uploading The Sensor data to OM2M Server using NodeMCU

#include<ESP8266HTTPClient.h>                //Including the required Libraries  
#include<ESP8266WiFi.h>
#include<DHT.h>

HTTPClient http;                             //The http object is created for HTTPClient Class

#define MAIN_SSID "HARSHA"                    //Wifi Credentials
#define MAIN_PASS "welcome123"

//Sensor Pins
#define DHTPIN 0     // DHT11 sensor pin is connected to D3
#define SOILPIN 15   //Soil Moisture sensor pin is connected to D8
#define WATERPIN A0  //Water Level Sensor Pin is connected to A0

DHT dht(DHTPIN,DHT11);                       //Instantiating the DHT dht with Digital Pin and DHT sensor type

//OM2M Configuration for POST Request URL Generation
#define CSE_IP "192.168.0.195"          
#define CSE_PORT 1234
#define HTPPS false
#define OM2M_ORGIN "admin:admin"
#define OM2M_MN "/~/in-cse/in-name/"
#define OM2M_AE "Farmlands"             
#define OM2M_DATA_CONT "Location-1_Outdoor_Plants/Data_Container"

void setup()
{
       Serial.begin(115200);                                      //To begin the serial communication.
       delay(10);                                                 //To give an appropriate delay
       dht.begin();
       
       Serial.println("Connecting to ");
       Serial.println(MAIN_SSID);
 
       WiFi.begin(MAIN_SSID, MAIN_PASS);                          //To connect Nodemcu to Wifi

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
  static int i=0;                                                          //Used to assign a number to the data points uploaded to Data Instance on Server
  float Humidity = dht.readHumidity();                                     //Using float variables to read the values from the sensors
  float Temperature = dht.readTemperature();
  float heatIndex = dht.computeHeatIndex(Temperature,Humidity,false);
  float waterLevel = analogRead(WATERPIN);
  float soilMoisture = analogRead(SOILPIN);
      
  if (isnan(Humidity) || isnan(Temperature) || isnan(heatIndex)) 
      {
         Serial.println("Failed to read from DHT sensor!");
          return;
      }
  if (isnan(waterLevel)) 
      {
         Serial.println("Failed to read from Water Level sensor!");
          return;
      }
  if (isnan(soilMoisture)) 
      {
         Serial.println("Failed to read from Soil Moisture sensor!");
          return;
      }
  Serial.print("Temperature: ");                               //Printing the data on Serial Monitor for debugging purpose
  Serial.print(Temperature);
  Serial.print(" degrees Celcius, Humidity: ");
  Serial.print(Humidity);
  Serial.print("%. Heat Index: ");
  Serial.print(heatIndex);
  Serial.print(" Water Level: ");
  Serial.print(waterLevel);
  Serial.print(" Soil Moisture: ");
  Serial.print(soilMoisture);

//Constructing the URL based on these parameters.
String data ="["+ String(Temperature) +","+ String(Humidity) +","+ String(heatIndex) +","+ String(waterLevel)+","+ String(soilMoisture)+"]";
String server = "http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String()+OM2M_MN;
String URL= server + String() +OM2M_AE + "/" + OM2M_DATA_CONT + "/";
http.begin(URL);

//Attaching the headers before sending the request

http.addHeader("X-M2M-Origin", OM2M_ORGIN);
http.addHeader("Content-Type", "application/json;ty=4");
http.addHeader("Content-Length", "100");

//Constructing the request with the data values of all the three sensor values 
//Here data parameters are combined as a String Object.

String req_data = String() + "{\"m2m:cin\": {"+ "\"con\": \"" + data + "\","+ "\"rn\": \"" + "cin_"+String(i++)+ "\","+ "\"cnf\": \"text\""+ "}}";

//Publishing the Data

int Response_code = http.POST(req_data);
http.end();

Serial.print(" Code: ");
Serial.println(Response_code);
delay(10000);
}
