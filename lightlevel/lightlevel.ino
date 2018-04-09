#define MIN 0
#define MAX 1000

#define SENSOR_PIN A0

#define ledPin1 D6     // select the pin for the LED
#define ledPin2 D7
#define ledPin3 D8


int count = 0;
int presensorValue = 0;
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);//to indicate on/off of sys

  //initialize wifi and print out para
  Serial.begin(115200);
  Serial.println();
   
  
}

// the loop function runs over and over again forever
void loop() {
  //check if client connected
   //part 1 
    while(1){
      int sensorValue = 0;  // variable to store the value coming from the sensor
      sensorValue=analogRead(SENSOR_PIN);
      sensorValue = 100 - ((sensorValue-MIN)*100/(MAX-MIN));//NORMALIZE
      Serial.print("SensorValue:");
      Serial.println(sensorValue);
    if(sensorValue>95){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, HIGH);
        digitalWrite(ledPin3, HIGH);
        digitalWrite(LED_BUILTIN, LOW);
      }
      else if( 80<sensorValue<=95){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, HIGH);
        digitalWrite(ledPin3, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
        }
      else if(50<sensorValue<=80){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, LOW);
        digitalWrite(ledPin3, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
      }else{
        digitalWrite(ledPin1, LOW);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, LOW);
        digitalWrite(ledPin3, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
        }
      delay(500);
      count++;
      if(count >= 120){
        count = 0;
        break;
        }
    }
//part2
while(1){
    int sensorValue = 0;  // variable to store the value coming from the sensor
    sensorValue=analogRead(SENSOR_PIN);
    for(int i = 0;i<5;i++){
        sensorValue+=analogRead(SENSOR_PIN);
        delay(1000);
      }
    sensorValue/=5;
    
      Serial.print("SensorValue:");
      Serial.println(sensorValue);
    if(sensorValue>95){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, HIGH);
        digitalWrite(ledPin3, HIGH);
        digitalWrite(LED_BUILTIN, LOW);
      }
      else if( 80<sensorValue<=95){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, HIGH);
        digitalWrite(ledPin3, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
        }
      else if(50<sensorValue<=80){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, LOW);
        digitalWrite(ledPin3, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
      }else{
        digitalWrite(ledPin1, LOW);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, LOW);
        digitalWrite(ledPin3, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
        }
      count++;
      if(count >= 12){
        count = 0;
        break;
        }
    }


//part 3
while(1){
    int sensorValue = 0;  // variable to store the value coming from the sensor
    sensorValue=analogRead(SENSOR_PIN);
 
      Serial.print("SensorValue:");
      Serial.println(sensorValue);
    if(abs(presensorValue-sensorValue)<0.5){
        digitalWrite(ledPin1, LOW);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, LOW);
        digitalWrite(ledPin3, LOW);
      }
      else if((sensorValue-presensorValue)>0.5){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, HIGH);
        digitalWrite(ledPin3, HIGH);
        }
      else if((presensorValue-sensorValue)>0.5){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(ledPin2, LOW);
        digitalWrite(ledPin3, LOW);
      }
      delay(400);
      presensorValue = sensorValue;
      count++;
      if(count >= 150){
        count = 0;
        break;
        }
    }
     
}
