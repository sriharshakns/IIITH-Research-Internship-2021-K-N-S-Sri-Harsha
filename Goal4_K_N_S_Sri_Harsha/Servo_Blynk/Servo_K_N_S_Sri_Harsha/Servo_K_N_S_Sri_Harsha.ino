//Servo Motor Control Using Blynk Application
//The connections needed for this project are Orange Pin to D8, Vcc (Red) & GND (Brown) respectively.
#define BLYNK_PRINT Serial
#define servo_pin 15                               //Servo PWM pin is taken as pin 15 (D8)

#include <ESP8266WiFi.h>                           //Inclding the ESP Library
#include <BlynkSimpleEsp8266.h>                    //Including the Blynk Library
#include<Servo.h>                                  //Including the Servo Motor Library

char auth[] = "edjGL8OZTUL--Bf9ZAPOCQTuIKg5EuVJ";   //Auth Token for the Blynk App
char ssid[] = "HARSHA";                             //Initialize Wifi Credentials
char pass[] = "welcome123";


Servo servo;

BLYNK_WRITE(V3)                                       //To take value from slider Input from Blynk App and write it to servo
{
  int potReading=param.asInt();
  servo.write(potReading);
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);                      //To initialize Blynk Connection
  servo.attach(servo_pin);
}


void loop()
{
  Blynk.run();                                        //To start a continuous connection of NodeMCU with Blynk App
}
