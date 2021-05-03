#include<ESP8266HTTPClient.h>              //Including the necessary Libraries
#include <ESP8266WiFi.h>
#include<Servo.h>

#define MAIN_SSID "HARSHA"                //Wifi Credentials  
#define MAIN_PASS "welcome123"

#define CSE_IP "192.168.43.92"            //OM2M Server Details
#define CSE_PORT    8080
#define HTTPS     false
#define OM2M_ORGIN    "admin:admin"
#define OM2M_MN     "/~/mn-cse/mn-name/"
#define OM2M_AE_0     "LAMP_0"
#define OM2M_AE_1     "LAMP_1"
#define OM2M_AE_2     "SENSOR_ACTUATOR"
#define OM2M_DATA_CONT  "DATA"
#define LISTENER_PORT  8000

#define LDRPIN 10                         //Sensor Pin
#define SERVOPIN 12                       //Actuator Pin

#define LED1 4                            //LED Pins
#define LED2 14

WiFiServer listener(LISTENER_PORT);
HTTPClient http; 
Servo servo;

void setup()
{
   Serial.begin(115200);
   Serial.println("Connecting to "+ String()+ MAIN_SSID);
   WiFi.begin(MAIN_SSID, MAIN_PASS);
   Serial.println("Connection Successful");
   listener.begin(LISTENER_PORT);
   Serial.println("ESP listener started");
   Serial.println("Ip Address is"+WiFi.localIP());
   delay(500);
   pinMode(LED1,OUTPUT);
   pinMode(LED2,OUTPUT);
   pinMode(LDRPIN,INPUT);
   pinMode(SERVOPIN, OUTPUT);
   servo.attach(SERVOPIN);
   servo.write(0);
}
void loop(){
listener.begin(LISTENER_PORT);
WiFiClient client = listener.available();
//To send Lamps State to Server
   //Lamp-0 Object 
   String server="http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String()+OM2M_MN;
   http.begin(server + String() +OM2M_AE_0 + "/" + OM2M_DATA_CONT + "/");
   http.addHeader("X-M2M-Origin", OM2M_ORGIN);
   http.addHeader("Content-Type", "application/json;ty=23");
   http.addHeader("Content-Length", "100");
   String req_data = String() + "{\"m2m:sub\": {"
     + "\"rn\": \"led_sub_test\""+","
     + "\"nu\": \"" + "http://"+WiFi.localIP().toString()+":"+LISTENER_PORT + "\","
     + "\"nct\": \"2\""
     + "}}";
   int x=http.POST(req_data);
   http.end();
   //Lamp-1 Object
   http.begin(server + String() +OM2M_AE_1 + "/" + OM2M_DATA_CONT + "/");
   http.addHeader("X-M2M-Origin", OM2M_ORGIN);
   http.addHeader("Content-Type", "application/json;ty=23");
   http.addHeader("Content-Length", "100");
   req_data = String() + "{\"m2m:sub\": {"  
     + "\"rn\": \"led_sub_test\""+","
     + "\"nu\": \"" + "http://"+WiFi.localIP().toString()+":"+LISTENER_PORT + "\","
     + "\"nct\": \"2\""
     + "}}";
   x =http.POST(req_data);
   Serial.println(x);
   http.end();
String w=client.readString();                   //Logic to control the Lamps
Serial.println(w);
int a=w.indexOf("false");
int b=w.indexOf("true");
int c=w.indexOf("LAMP_0");
int d=w.indexOf("LAMP_1");

if(b>0)
{
  if(c>0)
  {
  digitalWrite(LED1,HIGH);
  Serial.println("Light-1 ON");
  }
  else if(d>0)
  { 
  digitalWrite(LED2,HIGH);
  Serial.println("Light-2 ON");
  }
}
else if(a>0)
{
  if(c>0)
  {
  digitalWrite(LED1,LOW);
  Serial.println("Light-1 OFF");
  }
  else if(d>0)
  {  
  digitalWrite(LED2,LOW);
  Serial.println("Light-2 OFF");
  }
}
int light=digitalRead(LDRPIN);                //LDR and Servo Functionality
Serial.println(light);
static int i=20, j=0, k=0;
if(light==0)
{
  if(j==0)
  {
  servo.write(i);
  i+=20;
  }
Serial.println(i);
  if(i>180)
  {
    Serial.println("Max Angle Reached");
    j++;
  }
}
//To send LDR and Servo Data to Server
//Constructing the URL based on these parameters.
String data ="["+ String(light) +","+ String(i) +"]";
String server1 = "http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String()+OM2M_MN;
String URL= server1 + String() +OM2M_AE_2 + "/" + OM2M_DATA_CONT + "/";
http.begin(URL);

//Attaching the headers before sending the request

http.addHeader("X-M2M-Origin", OM2M_ORGIN);
http.addHeader("Content-Type", "application/json;ty=4");
http.addHeader("Content-Length", "100");

//Constructing the request with the data values of all the three sensor values 
//Here data parameters are combined as a String Object.

String req_data1 = String() + "{\"m2m:cin\": {"+ "\"con\": \"" + data + "\","+ "\"rn\": \"" + "cin_"+String(k++)+ "\","+ "\"cnf\": \"text\""+ "}}";

//Publishing the Data

int Response_code = http.POST(req_data1);
http.end();

Serial.print(" Code: ");
Serial.println(Response_code);
client.flush();
// Send HTTP response to the client
String s = "HTTP/1.1 200 OK\r\n";
client.print(s);
delay(100);
}
