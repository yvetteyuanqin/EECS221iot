#include <ESP8266WiFi.h> 


#define SENSOR_PIN A0
#define ledPin1 D6     // select the pin for the LED
#define ledPin2 D7
#define ledPin3 D8

int sensorValue = 0;  // variable to store the value coming from the sensor
WiFiServer server(80);

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);//to indicate on/off of sys

  //initialize wifi and print out para
  Serial.begin(115200);
  Serial.println();
  //WiFi.begin("network-name", "pass-to-network"); 
  WiFi.begin("31B206", "4CE262W301D6E"); 
  
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print("."); 
   }
  Serial.println(); 
  Serial.print("Connected, IP address: "); 
  Serial.println(WiFi.localIP()); 
  Serial.print("Connected, MAC address: "); 
  Serial.println(WiFi.macAddress()); 

  //initialize server
  server.begin();
  Serial.println("Server started.");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

// the loop function runs over and over again forever
void loop() {
  //check if client connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println("Welcome new client action!");
  while(!client.available()){
      delay(1);
    }
    
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

   // Match the request
 
  // Set ledPin according to the request
//digitalWrite(ledPin, value);
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(LED_BUILTIN, LOW);
    value = HIGH;
    //start light sensing part
    sensorValue = analogRead(SENSOR_PIN);
    
    for(int i = 0;i<2;i++){
        sensorValue+=analogRead(SENSOR_PIN);
        delay(100);
      }
      
      sensorValue/=2;
      Serial.println(sensorValue);
    if(value == HIGH && sensorValue<350){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, HIGH);
        digitalWrite(ledPin3, HIGH);
      }
      else if(value == HIGH && 350<=sensorValue<800){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, HIGH);
        digitalWrite(ledPin3, LOW);
        }
      else{
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, LOW);
        digitalWrite(ledPin3, LOW);
      }
    
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(LED_BUILTIN, HIGH);
    value = LOW;
    digitalWrite(ledPin1, LOW);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
  }

 
  // Return the response on server
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led is now: ");
 
  if(value == HIGH) {
    client.print("ON");
  } else {
    client.print("OFF");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");  
  client.println("</html>");
 
// delay(1);
//  Serial.println("Client disonnected");
//  Serial.println("");

//TEST ENDS HERE
  
//  while(value){
//    //stop if off
//  request = client.readStringUntil('\r');
//  client.flush();
//  if (request.indexOf("/LED=ON") != -1)  {
//    digitalWrite(LED_BUILTIN, LOW);
//    value = HIGH;
//  }
//  if (request.indexOf("/LED=OFF") != -1)  {
//    digitalWrite(LED_BUILTIN, HIGH);
//    value = 0;
//  }
//  
    
    
  //  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //  delay(1000);                       // wait for a second
  //  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  //  delay(1000);                       // wait for a second
}
