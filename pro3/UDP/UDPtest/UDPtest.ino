#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


char ssid[] = "UCInet Mobile Access";  //  your network SSID (name)
//char pass[] = "xxxxxx";       // your network password


WiFiUDP Udp;
unsigned int localUdpPort = 4220;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back
boolean udpConnected = false;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Wifi connected");
  
  udpConnected = connectUDP();
  if(!udpConnected){
    Serial.print("Reonnecting to UDP");
    udpConnected = connectUDP();
    }
  Serial.println();
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  
 
}

boolean connectUDP(){
  boolean state = false;

  
  if(Udp.begin(localUdpPort) == 1){

  state = true;
  }

  return state;

}
void loop()
{   
  udpConnected = connectUDP();
  if(!udpConnected){
      Serial.print("Reconnecting.");
      while(!udpConnected){
      Serial.print(".");
      udpConnected = connectUDP();
      }
    }
  delay(1000);
  
    Udp.beginPacket(Udp.remoteIP(), localUdpPort);
    Udp.write(replyPacket);
    Udp.endPacket();
    
  int packetSize = Udp.parsePacket();
  
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("Command receivied: %s\n", incomingPacket);
    

    // send back a reply, to the IP address and port we got the packet from

  }
}
