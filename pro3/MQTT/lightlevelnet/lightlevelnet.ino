#include <CircularBuffer.h>
#define MIN 0
#define MAX 1000
#define MAXCOUNT 10

#define SENSOR_PIN A0

#define ledPin1 D6     // select the pin for the LED

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define LED_PIN D6
#define pot A0 // define our potentiometer at pin0 which is the ADC for the ESP8266 board (the only ADC pin available)
/************************* WiFi Access Point *********************************/
//
//#define WLAN_SSID       "31B206" 
//#define WLAN_PASS       "4CE262W301D6E" 
#define WLAN_SSID       "Yuan" 
#define WLAN_PASS       "11111111"
/************************* Adafruit.io Setup *********************************/

#define MQTT_SERVER      "172.20.10.2" // change this to your Pi IP_address
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL
#define MQTT_USERNAME    "" // empty
#define MQTT_KEY         "" // empty

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
CircularBuffer<int, MAXCOUNT> sensedArray;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);

/****************************** data stream publish/subscribe ***************************************/

Adafruit_MQTT_Publish pot_stream = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/esp/pot81"); // change the topic


Adafruit_MQTT_Subscribe esp8266_led = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/leds/esp8266_81"); 

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();
int count = 0;
int average=0;
int total=0;
int sensorValue = 0;  // variable to store the value coming from the sensor
int diff=-20;
unsigned long previousMillis1 = 0;        // will store last time LED was updated
unsigned long previousMillis2 = 0;        // will store last time LED was updated
const long interval1 = 5000;           // interval at which to send and receive
const long interval2 = 1000;           // interval at which to read sensor

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin1, OUTPUT);

  //pinMode(LED_BUILTIN, OUTPUT);//to indicate on/off of sys

  //initialize wifi and print out para
   
  Serial.begin(115200);
  delay(10);
  pinMode(pot,INPUT); // potentiometer connected to A0 to obtain analog value to be streamed to Raspberry Pi (broker)


  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for esp8266_led feed. 
  mqtt.subscribe(&esp8266_led); 
}

int command = 1  ;
// the loop function runs over and over again forever
void loop() {

 
    unsigned long currentMillis = millis();
    //check still mqtt active or not
    MQTT_connect();

      if(command==1){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(100);
        digitalWrite(ledPin1, LOW);   // turn the LED on (HIGH is the voltage level)
        delay(100);
        if (currentMillis - previousMillis1 >= interval1) {
            previousMillis1 = currentMillis;
            Serial.println("-------------------------------------------");
            Serial.print("New average sensor value to be sent:");
            Serial.println(average);
            Serial.println("Sending new average and receiving instruction......");
            //to be finished
            //send average
              pot_stream.publish(average); // publish to Raspberry Pi under topic "/esp/pot88"
            
            delay(100);//for publish
            //Subscribe ,get command
            Adafruit_MQTT_Subscribe *subscription; 
            while ((subscription = mqtt.readSubscription())) { 
              if (subscription == &esp8266_led) { 
                 char *message = (char *)esp8266_led.lastread; 
                 Serial.print(F("Got: ")); 
                 Serial.println(message); 
                    // Check if the message was SLOWER OR QUICKER 
               if (strncmp(message, "QUICKER", 7) == 0) { 
                 // Turn the LED faster. 
          
                 command = 1; 
               } 
               else if (strncmp(message, "SLOWER", 6) == 0) { 
                 // Turn the LED slower. 
                 command = 2; 
          
               } 
               
             } 
           }

            }
        
        
      }else if(command==2){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(500);
        digitalWrite(ledPin1, LOW);   // turn the LED on (HIGH is the voltage level)
        delay(500);
         if (currentMillis - previousMillis1 >= interval1) {
            previousMillis1 = currentMillis;
            Serial.println("-------------------------------------------");
            Serial.print("New average sensor value to be sent:");
            Serial.println(average);
            Serial.println("Sending new average and receiving instruction......");
            //to be finished
            //send average
              pot_stream.publish(average); // publish to Raspberry Pi under topic "/esp/pot88"
            
            delay(100);//for publish
            //Subscribe ,get command
            Adafruit_MQTT_Subscribe *subscription; 
            while ((subscription = mqtt.readSubscription())) { 
              if (subscription == &esp8266_led) { 
                 char *message = (char *)esp8266_led.lastread; 
                 Serial.print(F("Got: ")); 
                 Serial.println(message); 
                    // Check if the message was SLOWER OR QUICKER 
               if (strncmp(message, "QUICKER", 7) == 0) { 
                 // Turn the LED faster. 
          
                 command = 1; 
               } 
               else if (strncmp(message, "SLOWER", 6) == 0) { 
                 // Turn the LED slower. 
                 command = 2; 
          
               } 
               
             } 
           }

            }
        
            
      }
      
      //calculate average value of last 10 seconds
      if (currentMillis - previousMillis2 >= interval2) {   //interval2=1 sec
            previousMillis2 = currentMillis;
            sensorValue=analogRead(SENSOR_PIN);
            if(count<MAXCOUNT){          
              count++;
              total+=sensorValue/MAXCOUNT;
              sensedArray.unshift(sensorValue);
              for (int i=0;i<sensedArray.size();i++){
                Serial.print(sensedArray[i]);
                Serial.print("\t");    // prints a tab
                }
              Serial.println();
              }
            else{
              int temp=sensedArray.pop();
              total-= temp/MAXCOUNT;
              total+=sensorValue/MAXCOUNT;
              sensedArray.unshift(sensorValue);
              for (int i=0;i<sensedArray.size();i++){
                Serial.print(sensedArray[i]);
                Serial.print("\t");    // prints a tab
                }
              Serial.print("Average of pasat ten seconds:");
              Serial.println(total);
              }
              average=total;
            }
            
            
      
     
  }
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
