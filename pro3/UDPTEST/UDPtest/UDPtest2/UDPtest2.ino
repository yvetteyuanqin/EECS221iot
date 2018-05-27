/*
Scalable conncection
May.3.2018
Yuan Qin 
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


#undef DEBUG

char ssid[] = "UCInet Mobile Access";  //  your network SSID (name)
//char pass[] = "xxxxxx";       // your network password


#define VERSIONNUMBER 1



#define LOGGERIPINC 20
#define SWARMSIZE 5
// 30 seconds is too old - it must be dead
#define SWARMTOOOLD 30000

int mySwarmID = 0;//corresponding to an IP address

// Packet Types

#define LIGHT_UPDATE_PACKET 0
//#define RESET_SWARM_PACKET 1
//#define CHANGE_TEST_PACKET 2
//#define RESET_ME_PACKET 3
#define DEFINE_SERVER_LOGGER_PACKET 4 //pi to every node
#define LOG_TO_SERVER_PACKET 5 //Master to Server
//#define MASTER_CHANGE_PACKET 6
//#define BLINK_BRIGHT_LED 7


//info kept by every node
unsigned int localPort = 4220;      // local port to listen for UDP packets

// master variables
boolean masterState = true;   // True if master, False if not
int swarmClear[SWARMSIZE];//SWARMSIZE=1
int swarmVersion[SWARMSIZE];
int swarmState[SWARMSIZE];
long swarmTimeStamp[SWARMSIZE];   // for aging what is the last time you receive from 

int swarmAddresses[SWARMSIZE];  // Swarm addresses
IPAddress serverAddress = IPAddress(0, 0, 0, 0); // default no IP Address.Do not know server address yet
//everybody record server address in case it becomes master












// variables for light sensor

int clearColor;//Only light level
//int redColor;
//int blueColor;
//int greenColor;


const int PACKET_SIZE = 14; // Light Update Packet
const int BUFFERSIZE = 255; //holding the info from udp packet maximum, much larger, only 7 in our case

byte packetBuffer[BUFFERSIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
//change TCS to normal analogue input
///* Initialise with default values (int time = 2.4ms, gain = 1x) */
//// Adafruit_TCS34725 tcs = Adafruit_TCS34725();
//
///* Initialise with specific int time and gain values */
//Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);


IPAddress localIP;// get from wifi.getip

void setup()
{





  Serial.begin(115200);
  Serial.println();
  Serial.println();



  Serial.println("");
  Serial.println("--------------------------");
  Serial.println("Lightlevel Communication");
  Serial.print("Version ");
  Serial.println(VERSIONNUMBER);
  Serial.println("--------------------------");

  Serial.println(F(" 5.3.2018"));
  Serial.print(F("Compiled at:"));
  Serial.print (F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println();
  //Showing initial signal the start signal
  pinMode(0, OUTPUT);

  digitalWrite(0, LOW);
  delay(500);
  digitalWrite(0, HIGH);
  
//tcs not used
//  randomSeed(analogRead(A0));
//  Serial.print("analogRead(A0)=");
//  Serial.println(analogRead(A0));
//
//
//
//  if (tcs.begin()) {
//    Serial.println("Found sensor");
//  } else {
//    Serial.println("No TCS34725 found ... check your connections");
//
//  }
//
//  // turn off the light
//  tcs.setInterrupt(true);  // true means off, false means on

  // everybody starts at 0 and changes from there
  mySwarmID = 0;//take table[0] as my info

  // We start by connecting to a WiFi network
  Serial.print("LightSwarm Instance: ");
  Serial.println(mySwarmID);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid);
 

  // initialize Swarm Address - we start out as swarmID of 0
  
//Connect wifi first then conncect to udp

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



  // initialize light sensor and arrays initialize the table
  int i;
  for (i = 0; i < SWARMSIZE; i++)//1 in our case, default value for every device
  {

    swarmAddresses[i] = 0;
    swarmClear[i] = 0;
    swarmTimeStamp[i] = -1;
  }
  swarmClear[mySwarmID] = 0;//mySwarmID=0
  swarmTimeStamp[mySwarmID] = 1;   // I am always in time to myself
  clearColor = swarmClear[mySwarmID];
  swarmVersion[mySwarmID] = VERSIONNUMBER;
  swarmState[mySwarmID] = masterState;//masterState is true initially
  Serial.print("clearColor =");
  Serial.println(clearColor);


  // set SwarmID based on IP address 

  
  localIP = WiFi.localIP();
  
  swarmAddresses[0] =  localIP[3];//The fourth byte as IP address
  
  
  mySwarmID = 0;
  
  Serial.print("MySwarmID=");
  Serial.println(mySwarmID);

}

void loop()
{
  int secondsCount;
  int lastSecondsCount;

  lastSecondsCount = 0;
#define LOGHOWOFTEN

  secondsCount = millis() / 100;



  int  c;
//
//  tcs.getRawData(&r, &g, &b, &c);//
  //To be finished read from light sensor
  c=100;//need modify



  
//  colorTemp = tcs.calculateColorTemperature(r, g, b);
//  lux = tcs.calculateLux(r, g, b);
//
//  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
//  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
//  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
//  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
//  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
//  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
//  Serial.println(" ");

  clearColor = c;
//  redColor = r;
//  blueColor = b;
//  greenColor = g;

  swarmClear[mySwarmID] = clearColor;//keep the sensor value in the table

  // wait to see if a reply is available
  delay(100);

  int cb = udp.parsePacket();

  if (!cb) {
    //  Serial.println("no packet yet");
    Serial.print(".");
  }
  else {
    // We've received a packet, read the data from it

    udp.read(packetBuffer, PACKET_SIZE); // read the packet into the buffer
    Serial.print("packetbuffer[1] =");
    Serial.println(packetBuffer[1]);//Command type
    if (packetBuffer[1] == LIGHT_UPDATE_PACKET)
    {
      Serial.print("LIGHT_UPDATE_PACKET received from LightSwarm #");
      Serial.println(packetBuffer[2]);//Swarm ID
      setAndReturnMySwarmIndex(packetBuffer[2]);//Go check if received previous data from this id, look up whether this id in table or not, if not save it

      Serial.print("LS Packet Recieved from #");
      Serial.print(packetBuffer[2]);
      Serial.print(" SwarmState:");
      if (packetBuffer[3] == 0)//state
        Serial.print("SLAVE");
      else
        Serial.print("MASTER");
      Serial.print(" CC:");
      Serial.print(packetBuffer[5] * 256 + packetBuffer[6]);
//      Serial.print(" RC:");
//      Serial.print(packetBuffer[7] * 256 + packetBuffer[8]);
//      Serial.print(" GC:");
//      Serial.print(packetBuffer[9] * 256 + packetBuffer[10]);
//      Serial.print(" BC:");
//      Serial.print(packetBuffer[11] * 256 + packetBuffer[12]);
      Serial.print(" Version=");
      Serial.println(packetBuffer[4]);

      // record the incoming clear color

      swarmClear[setAndReturnMySwarmIndex(packetBuffer[2])] = packetBuffer[5] * 256 + packetBuffer[6];
      swarmVersion[setAndReturnMySwarmIndex(packetBuffer[2])] = packetBuffer[4];
      swarmState[setAndReturnMySwarmIndex(packetBuffer[2])] = packetBuffer[3];
      swarmTimeStamp[setAndReturnMySwarmIndex(packetBuffer[2])] = millis();//time received


      // Check to see if I am master!
      checkAndSetIfMaster();// continues to be a master if this one not brighter

    }

//    if (packetBuffer[1] == RESET_SWARM_PACKET)
//    {
//      Serial.println(">>>>>>>>>RESET_SWARM_PACKETPacket Recieved");
//      masterState = true;
//      Serial.println("Reset Swarm:  I just BECAME Master (and everybody else!)");
//      digitalWrite(0, LOW);
//
//    }
//
//    if (packetBuffer[1] == CHANGE_TEST_PACKET)
//    {
//      Serial.println(">>>>>>>>>CHANGE_TEST_PACKET Packet Received");
//      Serial.println("not implemented");
//      int i;
//      for (i = 0; i < PACKET_SIZE; i++)
//      {
//        if (i == 2)
//        {
//          Serial.print("LPS[");
//          Serial.print(i);
//          Serial.print("] = ");
//          Serial.println(packetBuffer[i]);
//
//        }
//        else
//        {
//          Serial.print("LPS[");
//          Serial.print(i);
//          Serial.print("] = 0x");
//          Serial.println(packetBuffer[i], HEX);
//        }
//
//      }
//
//
//    }

//    if (packetBuffer[1] == RESET_ME_PACKET)
//    {
//      Serial.println(">>>>>>>>>RESET_ME_PACKET Packet Recieved");
//
//
//      if (packetBuffer[2] == swarmAddresses[mySwarmID])
//      {
//        masterState = true;
//        Serial.println("Reset Me:  I just BECAME Master");
//        digitalWrite(0, LOW);
//
//
//      }
//      else
//      {
//        Serial.print("Wanted #");
//        Serial.print(packetBuffer[2]);
//        Serial.println(" Not me - reset ignored");
//      }
//
//
//    }

  }
//information from pi
  if (packetBuffer[1] ==  DEFINE_SERVER_LOGGER_PACKET)
  {
    Serial.println(">>>>>>>>>DEFINE_SERVER_LOGGER_PACKET Packet Recieved");
    serverAddress = IPAddress(packetBuffer[4], packetBuffer[5], packetBuffer[6], packetBuffer[7]);
    Serial.print("Server address received: ");
    Serial.println(serverAddress);




  }
//  if (packetBuffer[1] ==  BLINK_BRIGHT_LED)
//  {
//    Serial.println(">>>>>>>>>BLINK_BRIGHT_LED Packet Recieved");
//    if (packetBuffer[2] == swarmAddresses[mySwarmID])
//    {
//
//
//      tcs.setInterrupt(false);  // true means off, false means on
//      delay(packetBuffer[4] * 100);
//      tcs.setInterrupt(true);  // true means off, false means on
//    }
//    else
//    {
//      Serial.print("Wanted #");
//      Serial.print(packetBuffer[2]);
//      Serial.println(" Not me - reset ignored");
//    }
//
//
//
//  }



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
  
  

  broadcastARandomUpdatePacket();//node broadcast
  //  sendARandomUpdatePacket();
  sendLogToServer();//Only the master need to do

}

// send an LIGHT Packet request to the swarms at the given address
//setup local table
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
//  packetBuffer[7] = (redColor & 0xFF00) >> 8; // Red High Byte
//  packetBuffer[8] = (redColor & 0x00FF); // Red Low Byte
//  packetBuffer[9] = (greenColor & 0xFF00) >> 8; // green High Byte
//  packetBuffer[10] = (greenColor & 0x00FF); // green Low Byte
//  packetBuffer[11] = (blueColor & 0xFF00) >> 8; // blue High Byte
//  packetBuffer[12] = (blueColor & 0x00FF); // blue Low Byte
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

  delay(randomDelay);//To avoid collision of messages

  IPAddress sendSwarmAddress(192, 168, 1, sendToLightSwarm); // my Swarm Address
  sendLightUpdatePacket(sendSwarmAddress);




}


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


// send log packet to Server if master and server address defined

void sendLogToServer()
{

  // build the string

  char myBuildString[1000];
  myBuildString[0] = '\0';
//Do only if self is master
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

        sprintf(swarmString, " %i,%i,%i,%i,%s,%i ", i, swarmState[i], swarmVersion[i], swarmClear[i], stateString, swarmAddresses[i]);

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

