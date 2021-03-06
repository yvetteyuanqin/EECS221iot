#include <CircularBuffer.h>
#include <Adafruit_TCS34725.h>

/*
Cooperative IOT Self Organizing Example
SwitchDoc Labs, August 2015

 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#define ledPin1 D6
#define MAXCOUNT 10
#undef DEBUG

char ssid[] = "Yuan"; //  your network SSID (name)
char pass[] = "11111111"; // your network password


#define VERSIONNUMBER 1

#define SENSOR_PIN A0

#define LOGGERIPINC 20
#define SWARMSIZE 1 // only one now
// 30 seconds is too old - it must be dead
#define SWARMTOOOLD 30000

int mySwarmID = 0;

// Packet Types

#define LIGHT_UPDATE_PACKET 0
#define RESET_SWARM_PACKET 1
#define CHANGE_TEST_PACKET 2
#define RESET_ME_PACKET 3
#define DEFINE_SERVER_LOGGER_PACKET 4
#define LOG_TO_SERVER_PACKET 5
#define MASTER_CHANGE_PACKET 6
#define BLINK_BRIGHT_LED 7



unsigned int localPort = 4220;      // local port to listen for UDP packets

// master variables
boolean masterState = true;   // True if master, False if not
int swarmClear[SWARMSIZE];
int swarmVersion[SWARMSIZE];
int swarmState[SWARMSIZE];
long swarmTimeStamp[SWARMSIZE];   // for aging

IPAddress serverAddress = IPAddress(172,20,10,2); // default no IP Address


int swarmAddresses[SWARMSIZE];  // Swarm addresses


// variables for light sensor

int clearColor;
int redColor = 0;
int blueColor = 0;
int greenColor = 0;

int window[10]; // sliding window
int mean = 0;
int count = 0;
int lastSecondsCount = 0;
int val = 0;
int lastval = 0;
int average=0;
int total=0;
unsigned long previousMillis1 = 0;        // will store last time LED was updated
unsigned long previousMillis2 = 0;        // will store last time LED was updated
const long interval1 = 5000;           // interval at which to send and receive
const long interval2 = 1000;         
const int PACKET_SIZE = 14; // Light Update Packet
const int BUFFERSIZE = 1024;
CircularBuffer<int, MAXCOUNT> sensedArray;
byte packetBuffer[BUFFERSIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
//Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);


IPAddress localIP;

uint16_t temp[10];

void setup()
{

  Serial.begin(115200);
  Serial.println();
  Serial.println();



  Serial.println("");
  Serial.println("--------------------------");
  Serial.println("LightSwarm");
  Serial.print("Version ");
  Serial.println(VERSIONNUMBER);
  Serial.println("--------------------------");

  Serial.println(F(" 08/05/2018"));
  Serial.print(F("Compiled at:"));
  Serial.print (F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println();
  pinMode(D6, OUTPUT);

  digitalWrite(D6, LOW);
  delay(500);
  digitalWrite(D6, HIGH);

  // everybody starts at 0 and changes from there
  mySwarmID = 0;

  // We start by connecting to a WiFi network
  Serial.print("LightSwarm Instance: ");
  Serial.println(mySwarmID);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
 

  // initialize Swarm Address - we start out as swarmID of 0
  

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");

  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());



  // initialize light sensor and arrays
  int i;
  for (i = 0; i < SWARMSIZE; i++)
  {

    swarmAddresses[i] = 0;
    swarmClear[i] = 0;
    swarmTimeStamp[i] = -1;
  }
  swarmClear[mySwarmID] = 0;
  swarmTimeStamp[mySwarmID] = 1;   // I am always in time to myself
  clearColor = swarmClear[mySwarmID];
  swarmVersion[mySwarmID] = VERSIONNUMBER;
  swarmState[mySwarmID] = masterState;
  //Serial.print("clearColor =");
  //Serial.println(clearColor);


  // set SwarmID based on IP address 

  
  localIP = WiFi.localIP();
  
  swarmAddresses[0] =  localIP[3];
  
  
  mySwarmID = 0;
  
  //Serial.print("MySwarmID=");
  //Serial.println(mySwarmID);
  for (int i = 0; i < 10 ;i++) {
      window[i] = analogRead(A0);
      mean += window[i]; 
    }
   mean = mean / 10;
   lastSecondsCount = millis() / 10; 
  // Serial.println(lastSecondsCount);

}
int prevaverage=0;

int sensorValue = 0;  // variable to store the value coming from the sensor
int diff=0;

void loop()
{
  unsigned long currentMillis = millis();
  
  if(diff<0){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(100);
        digitalWrite(ledPin1, LOW);   // turn the LED on (HIGH is the voltage level)
        delay(100);
        if (currentMillis - previousMillis1 >= interval1) {
            previousMillis1 = currentMillis;
            
            Serial.print("New average sensor value to be sent:");
            Serial.println(average);
            Serial.println("Sending new average and receiving instruction......");
            
            //to be finished
            //send average
            swarmClear[mySwarmID] = average;
            broadcastARandomUpdatePacket();
            sendLogToServer();

            Serial.println("Brighter, quicker");
            Serial.println("-------------------------------------------");
            diff=average-prevaverage;
            prevaverage=average;
            }
        
        
      }else if(diff>0){
        digitalWrite(ledPin1, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(500);
        digitalWrite(ledPin1, LOW);   // turn the LED on (HIGH is the voltage level)
        delay(500);
        if (currentMillis - previousMillis1 >= interval1) {
            previousMillis1 = currentMillis;
      
            Serial.print("New average sensor value to be sent:");
            Serial.println(average);
            Serial.println("Sending new average and receiving instruction......");
       
            //to be finished
            swarmClear[mySwarmID] = average;
            broadcastARandomUpdatePacket();
            sendLogToServer();
           
            diff=average-prevaverage;
            
            Serial.println("Darker, slower");
            Serial.println("-------------------------------------------");
            prevaverage=average;
            }
            
      }
      else {
      
        digitalWrite(ledPin1, HIGH);
        delay(500);
        if (currentMillis - previousMillis1 >= interval1) {
          previousMillis1 = currentMillis;
          diff=average-prevaverage;
          prevaverage=average;
          
         Serial.println("Same light level");
         Serial.println("-------------------------------------------");
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
              Serial.print("Average of past ten seconds:");
              Serial.println(total);
              }
              average=total;

            }
            
            
      }
     
  /*    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int secondsCount;
  secondsCount = millis() / 10;
  if (val < lastval) {
    digitalWrite(D6, LOW);
    delay(100);
    digitalWrite(D6, HIGH);
    delay(100);
    }
   else {
    digitalWrite(D6, LOW);
    delay(250);
    digitalWrite(D6, HIGH);
    delay(250);
    }
  //Serial.println(val, DEC);
  if ( secondsCount - lastSecondsCount > 99) {
  lastSecondsCount = secondsCount;
  lastval = val;
  
  val = analogRead(A0);

  int l_val = window[9];
  for (int i = 9; i > 0 ;i--) {
      window[i] = window[i - 1];
    }
    window[0] = val;
    mean = mean + (- l_val + val) / 10;
    //Serial.println(mean, DEC);

  clearColor = mean;
  swarmClear[mySwarmID] = clearColor;
  Serial.print("Light: "); Serial.print(clearColor, DEC); Serial.print(" ");
  Serial.println(" ");  

  // wait to see if a reply is available
  count++;

/***
  int cb = udp.parsePacket();

  if (!cb) {
    //  Serial.println("no packet yet");
    Serial.print(".");
  }
  else {
    // We've received a packet, read the data from it

    udp.read(packetBuffer, PACKET_SIZE); // read the packet into the buffer
    Serial.print("packetbuffer[1] =");
    Serial.println(packetBuffer[1]);
    if (packetBuffer[1] == LIGHT_UPDATE_PACKET)
    {
      Serial.print("LIGHT_UPDATE_PACKET received from LightSwarm #");
      Serial.println(packetBuffer[2]);
      setAndReturnMySwarmIndex(packetBuffer[2]);

      Serial.print("LS Packet Recieved from #");
      Serial.print(packetBuffer[2]);
      Serial.print(" SwarmState:");
      if (packetBuffer[3] == 0)
        Serial.print("SLAVE");
      else
        Serial.print("MASTER");
      Serial.print(" CC:");
      Serial.print(packetBuffer[5] * 256 + packetBuffer[6]);
      Serial.print(" RC:");
      Serial.print(packetBuffer[7] * 256 + packetBuffer[8]);
      Serial.print(" GC:");
      Serial.print(packetBuffer[9] * 256 + packetBuffer[10]);
      Serial.print(" BC:");
      Serial.print(packetBuffer[11] * 256 + packetBuffer[12]);
      Serial.print(" Version=");
      Serial.println(packetBuffer[4]);

      // record the incoming clear color

      swarmClear[setAndReturnMySwarmIndex(packetBuffer[2])] = packetBuffer[5] * 256 + packetBuffer[6];
      swarmVersion[setAndReturnMySwarmIndex(packetBuffer[2])] = packetBuffer[4];
      swarmState[setAndReturnMySwarmIndex(packetBuffer[2])] = packetBuffer[3];
      swarmTimeStamp[setAndReturnMySwarmIndex(packetBuffer[2])] = millis();


      // Check to see if I am master!
      checkAndSetIfMaster();

    }

    if (packetBuffer[1] == RESET_SWARM_PACKET)
    {
      Serial.println(">>>>>>>>>RESET_SWARM_PACKETPacket Recieved");
      masterState = true;
      Serial.println("Reset Swarm:  I just BECAME Master (and everybody else!)");
      digitalWrite(0, LOW);

    }

    if (packetBuffer[1] == CHANGE_TEST_PACKET)
    {
      Serial.println(">>>>>>>>>CHANGE_TEST_PACKET Packet Received");
      Serial.println("not implemented");
      int i;
      for (i = 0; i < PACKET_SIZE; i++)
      {
        if (i == 2)
        {
          Serial.print("LPS[");
          Serial.print(i);
          Serial.print("] = ");
          Serial.println(packetBuffer[i]);

        }
        else
        {
          Serial.print("LPS[");
          Serial.print(i);
          Serial.print("] = 0x");
          Serial.println(packetBuffer[i], HEX);
        }

      }


    }

    if (packetBuffer[1] == RESET_ME_PACKET)
    {
      Serial.println(">>>>>>>>>RESET_ME_PACKET Packet Recieved");


      if (packetBuffer[2] == swarmAddresses[mySwarmID])
      {
        masterState = true;
        Serial.println("Reset Me:  I just BECAME Master");
        digitalWrite(0, LOW);


      }
      else
      {
        Serial.print("Wanted #");
        Serial.print(packetBuffer[2]);
        Serial.println(" Not me - reset ignored");
      }


    }

  }

  if (packetBuffer[1] ==  DEFINE_SERVER_LOGGER_PACKET)
  {
    Serial.println(">>>>>>>>>DEFINE_SERVER_LOGGER_PACKET Packet Recieved");
    serverAddress = IPAddress(packetBuffer[4], packetBuffer[5], packetBuffer[6], packetBuffer[7]);
    Serial.print("Server address received: ");
    Serial.println(serverAddress);




  }
  if (packetBuffer[1] ==  BLINK_BRIGHT_LED)
  {
    Serial.println(">>>>>>>>>BLINK_BRIGHT_LED Packet Recieved");
    if (packetBuffer[2] == swarmAddresses[mySwarmID])
    {


      tcs.setInterrupt(false);  // true means off, false means on
      delay(packetBuffer[4] * 100);
      tcs.setInterrupt(true);  // true means off, false means on
    }
    else
    {
      Serial.print("Wanted #");
      Serial.print(packetBuffer[2]);
      Serial.println(" Not me - reset ignored");
    }

  }


  Serial.print("MasterStatus:");
  if (masterState == true)
  {
    digitalWrite(0, LOW);
    Serial.print("MASTER");
  }
  else
  {
    digitalWrite(0, HIGH);
    Serial.print("SLAVE");
  }
  Serial.print("/cc=");
  Serial.print(clearColor);
  Serial.print("/KS:");
  Serial.println(serverAddress);
  
  Serial.println("--------");

  
  int i;
  for (i = 0; i < SWARMSIZE; i++)
  {
    Serial.print("swarmAddress[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(swarmAddresses[i]); 
  }
  Serial.println("--------");

  
  if (count == 5) {
     broadcastARandomUpdatePacket();
     //  sendARandomUpdatePacket();
     sendLogToServer();
     count = 0;
    }
  }*/

// send an LIGHT Packet request to the swarms at the given address 
unsigned long sendLightUpdatePacket(IPAddress & address)
{

  //Serial.print("sending Light Update packet to:");
  // Serial.println(address);

  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, PACKET_SIZE);
  // Initialize values needed to form Light Packet
  // (see URL above for details on the packets)
  packetBuffer[0] = 0xF0;   // StartByte
  packetBuffer[1] = LIGHT_UPDATE_PACKET;     // Packet Type
  packetBuffer[2] = localIP[3];     // Sending Swarm Number
  packetBuffer[3] = masterState;  // 0 = slave, 1 = master
  packetBuffer[4] = VERSIONNUMBER;  // Software Version
  packetBuffer[5] = (clearColor & 0xFF00) >> 8; // Clear High Byte
  packetBuffer[6] = (clearColor & 0x00FF); // Clear Low Byte
  packetBuffer[7] = (redColor & 0xFF00) >> 8; // Red High Byte
  packetBuffer[8] = (redColor & 0x00FF); // Red Low Byte
  packetBuffer[9] = (greenColor & 0xFF00) >> 8; // green High Byte
  packetBuffer[10] = (greenColor & 0x00FF); // green Low Byte
  packetBuffer[11] = (blueColor & 0xFF00) >> 8; // blue High Byte
  packetBuffer[12] = (blueColor & 0x00FF); // blue Low Byte
  packetBuffer[13] = 0x0F;  //End Byte



  // all Light Packet fields have been given values, now
  // you can send a packet requesting coordination
  udp.beginPacketMulticast(address,  localPort, WiFi.localIP()); //
  //udp.beginPacket(address,  localPort); //
  udp.write(packetBuffer, PACKET_SIZE);
  udp.endPacket();
}

// delay 0-MAXDELAY seconds
#define MAXDELAY 500
void broadcastARandomUpdatePacket()
{

  int sendToLightSwarm = 255;
  Serial.print("Broadcast ToSwarm = ");
  Serial.print(sendToLightSwarm);
  Serial.print(" ");

  // delay 0-MAXDELAY seconds
  int randomDelay;
  randomDelay = random(0, MAXDELAY);
  Serial.print("Delay = ");
  Serial.print(randomDelay);
  Serial.print("ms : ");

  delay(randomDelay);

  IPAddress sendSwarmAddress(172,20,10,2); // my Swarm Address
  //IPAddress sendSwarmAddress(169, 234, 28, 120); // my Swarm Address
  sendLightUpdatePacket(sendSwarmAddress);

}

/***
void checkAndSetIfMaster()
{

  int i;
  for (i = 0; i < SWARMSIZE; i++)
  {


#ifdef DEBUG

    Serial.print("swarmClear[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.print(swarmClear[i]);
    Serial.print("  swarmTimeStamp[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(swarmTimeStamp[i]);
#endif

    Serial.print("#");
    Serial.print(i);
    Serial.print("/");
    Serial.print(swarmState[i]);
    Serial.print("/");
    Serial.print(swarmVersion[i]);
    Serial.print(":");
    // age data
    int howLongAgo = millis() - swarmTimeStamp[i] ;

    if (swarmTimeStamp[i] == 0)
    {
      Serial.print("TO ");
    }
    else if (swarmTimeStamp[i] == -1)
    {
      Serial.print("NP ");
    }
    else if (swarmTimeStamp[i] == 1)
    {
      Serial.print("ME ");
    }
    else if (howLongAgo > SWARMTOOOLD)
    {
      Serial.print("TO ");
      swarmTimeStamp[i] = 0;
      swarmClear[i] = 0;

    }
    else
    {
      Serial.print("PR ");


    }
  }

  Serial.println();
  boolean setMaster = true;

  for (i = 0; i < SWARMSIZE; i++)
  {

    if (swarmClear[mySwarmID] >= swarmClear[i])
    {
      // I might be master!

    }
    else
    {
      // nope, not master
      setMaster = false;
      break;
    }

  }
  if (setMaster == true)
  {
    if (masterState == false)
    {
      Serial.println("I just BECAME Master");
      digitalWrite(0, LOW);
    }

    masterState = true;
  }
  else
  {
    if (masterState == true)
    {
      Serial.println("I just LOST Master");
      digitalWrite(0, HIGH);
    }

    masterState = false;
  }

  swarmState[mySwarmID] = masterState;

}
***/
/***
int setAndReturnMySwarmIndex(int incomingID)
{
 
  int i;
  for (i = 0; i< SWARMSIZE; i++)
  {
    if (swarmAddresses[i] == incomingID)
    {
       return i;
    } 
    else
    if (swarmAddresses[i] == 0)  // not in the system, so put it in
    {
    
      swarmAddresses[i] = incomingID;
      Serial.print("incomingID ");
      Serial.print(incomingID);
      Serial.print("  assigned #");
      Serial.println(i);
      return i;
    }
    
  }  
  
  // if we get here, then we have a new swarm member.   
  // Delete the oldest swarm member and add the new one in 
  // (this will probably be the one that dropped out)
  
  int oldSwarmID;
  long oldTime;
  oldTime = millis();
  for (i = 0;  i < SWARMSIZE; i++)
 {
  if (oldTime > swarmTimeStamp[i])
  {
    oldTime = swarmTimeStamp[i];
    oldSwarmID = i;
  }
  
 } 
 
 // remove the old one and put this one in....
 swarmAddresses[oldSwarmID] = incomingID;
 // the rest will be filled in by Light Packet Receive
 
  
}

***/
// send log packet to Server if master and server address defined

void sendLogToServer()
{

  // build the string

  char myBuildString[1000];
  myBuildString[0] = '\0';

  if (masterState == true)
  {
    // now check for server address defined
    if ((serverAddress[0] == 0) && (serverAddress[1] == 0))
    {
      return;  // we are done.  not defined
    }
    else
    {
      // now send the packet as a string with the following format:
      // swarmID, MasterSlave, SoftwareVersion, clearColor, Status | ....next Swarm ID
      // 0,1,15,3883, PR | 1,0,14,399, PR | ....



      int i;
      char swarmString[20];
      swarmString[0] = '\0';

      for (i = 0; i < SWARMSIZE; i++)
      {

        char stateString[5];
        stateString[0] = '\0';
        if (swarmTimeStamp[i] == 0)
        {
          strcat(stateString, "TO");
        }
        else if (swarmTimeStamp[i] == -1)
        {
          strcat(stateString, "NP");
        }
        else if (swarmTimeStamp[i] == 1)
        {
          strcat(stateString, "PR");
        }
        else
        {
          strcat(stateString, "PR");
        }
        //sprintf(swarmString, " %i,%i,%i,%i,%s,%i ", i, swarmState[i], swarmVersion[i], swarmClear[i], stateString, swarmAddresses[i]);
        sprintf(swarmString, " %i", swarmClear[i]);

        strcat(myBuildString, swarmString);
        if (i < SWARMSIZE - 1)
        {

          strcat(myBuildString, "|");

        }
      }


    }


    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, BUFFERSIZE);
    // Initialize values needed to form Light Packet
    // (see URL above for details on the packets)
    packetBuffer[0] = 0xF0;   // StartByte
    packetBuffer[1] = LOG_TO_SERVER_PACKET;     // Packet Type
    packetBuffer[2] = localIP[3];     // Sending Swarm Number
    packetBuffer[3] = strlen(myBuildString); // length of string in bytes
    packetBuffer[4] = VERSIONNUMBER;  // Software Version
    int i;
    for (i = 0; i < strlen(myBuildString); i++)
    {
      packetBuffer[i + 5] = myBuildString[i];// first string byte
    }

    packetBuffer[i + 5] = 0x0F; //End Byte
    Serial.print("Sending Log to Sever:");
    Serial.println(myBuildString);
    int packetLength;
    packetLength = i + 5 + 1;

    udp.beginPacket(serverAddress,  localPort); //

    udp.write(packetBuffer, packetLength);
    udp.endPacket();

  }



}

