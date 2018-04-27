
#define MIN 0
#define MAX 1000

#define SENSOR_PIN A0

#define ledPin1 D6     // select the pin for the LED


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
  Serial.println();
   
  
}

// the loop function runs over and over again forever
void loop() {

while(1){
    unsigned long currentMillis = millis();
    
      if(diff>10){
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
            //diff= what received
            diff=-20;
            }
        
        
      }else if(diff<-10){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(500);
        digitalWrite(ledPin1, LOW);   // turn the LED on (HIGH is the voltage level)
        delay(500);
        if (currentMillis - previousMillis1 >= interval1) {
            previousMillis1 = currentMillis;
            Serial.println("---------------------------------------------------");
            Serial.print("New average sensor value to be sent:");
            Serial.println(average);
            Serial.println("Sending new average and receiving instruction......");
            //to be finished
            //diff= what received
            diff=20;
            }
            
      }
      //calculate average value of last 10 seconds
      if (currentMillis - previousMillis2 >= interval2) {   //interval2=1 sec
            previousMillis2 = currentMillis;
            sensorValue=analogRead(SENSOR_PIN);
            Serial.print("SensorValue:");
            Serial.println(sensorValue);
            count++;
            total+=sensorValue;
            if(count>=10){
              average =total/10 ;
              Serial.print("Average of past ten seconds:");
              Serial.println(average);
              count = 0;
              total = 0;

            }
            
            
      }
     
  }
}
