
# RPi mqtt
import time 
import paho.mqtt.client as mqtt

# aws mqtt
import RPi.GPIO as GPIO
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
from time import sleep
from datetime import date, datetime
newrecv=0
#old=None
# Setup callback functions that are called when MQTT events happen like 
# connecting to the server or receiving data from a subscribed feed. 
def on_connect(client, userdata, flags, rc): 
   print("Connected with result code " + str(rc)) 
   # Subscribing in on_connect() means that if we lose the connection and 
   # reconnect then subscriptions will be renewed. 
   client.subscribe("/homeauto/sensor1")
   client.subscribe("/homeauto/sensor2")


# The callback for when a PUBLISH message is received from the server. 
def on_message(client, userdata, msg):
   print("-----------------------------")
   #print(msg.topic+" "+str( msg.payload)) 
   # Check if this is a message for the Pi LED.
   #   global old
   #   print('old',old)
   if msg.topic == '/homeauto/sensor1':
       # Look at the message data and perform the appropriate action. 
       #global newrecv
       newrecv=int(msg.payload)
       print("new value from PRI1 received: ", newrecv)
       #DATA PROCESSING OMITED
       now = datetime.utcnow()
       now_str = now.strftime('%Y-%m-%dT%H:%M:%SZ') #e.g. 2016-04-18T06:12:25.877Z
       # LUGE send back name
       payload = '{ "Date_and_time:": "' + now_str + '","Intruder": ' + "name to be replaced" + ' }'
       print (payload)
       myMQTTClient.publish("homesec/cam", payload, 0)
   if msg.topic == '/homeauto/sensor2':
        # Look at the message data and perform the appropriate action.
        #global newrecv
        newrecv=int(msg.payload)
        print("new value from PRI2 received: ", newrecv)
        now = datetime.utcnow()
        now_str = now.strftime('%Y-%m-%dT%H:%M:%SZ') #e.g. 2016-04-18T06:12:25.877Z
        # LUGE send back name
        payload = '{ "Date_and_time:": "' + now_str + '","Intruder": ' + "name to be replaced" + ' }'
        print (payload)
        myMQTTClient.publish("homesec/cam", payload, 0)

# AWS IoT certificate based connection
myMQTTClient = AWSIoTMQTTClient("123afhlss456")
myMQTTClient.configureEndpoint("auf9cn18w1qbv.iot.us-east-2.amazonaws.com", 8883)
myMQTTClient.configureCredentials("/home/pi/certs/root-CA.pem", "/home/pi/certs/4b4aef80d1-private.pem.key", "/home/pi/certs/4b4aef80d1-certificate.pem.crt")
myMQTTClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
myMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
myMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
myMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec
 
# connect and publish to  cloud
myMQTTClient.connect()
myMQTTClient.publish("homesec/cam", "connected", 0)
           
# Create MQTT client and connect to localhost, i.e. the Raspberry Pi running 
# this script and the MQTT server. 
client = mqtt.Client() 
client.on_connect = on_connect 
client.on_message = on_message 
client.connect('localhost', 1883, 60) 
# Connect to the MQTT server and process messages in a background thread. 
client.loop_start() 
# Main loop to listen for button presses. 
print('Script is running, press Ctrl-C to quit...')


while True: 
   # Look for a change from high to low value on the button input to 
   # signal a button press. 
   time.sleep(0.001)  # Delay for about 20 milliseconds to debounce.
   
   
           

