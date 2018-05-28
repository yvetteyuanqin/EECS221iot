
# RPi
import time 
import paho.mqtt.client as mqtt
#newrecv=0
old=None
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
       '''
       if old !=None and old>newrecv:
           print('Brighter, send quicker instuction') 
       # Send a toggle message to the ESP8266 LED topic. 
           client.publish('/leds/esp8266_81', 'QUICKER')
       elif old !=None and old<=newrecv:
           print('Daker, send slower instuction') 
           # Send a toggle message to the ESP8266 LED topic. 
           client.publish('/leds/esp8266_81', 'SLOWER')
       '''
   if msg.topic == '/homeauto/sensor2':
        # Look at the message data and perform the appropriate action.
        #global newrecv
        newrecv=int(msg.payload)
        print("new value from PRI2 received: ", newrecv)
    #old=newrecv
           
           
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

   
           

