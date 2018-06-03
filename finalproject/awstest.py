import RPi.GPIO as GPIO
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
from time import sleep
from datetime import date, datetime
 
# initialize GPIO
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.cleanup()
 
# AWS IoT certificate based connection
myMQTTClient = AWSIoTMQTTClient("123afhlss456")
myMQTTClient.configureEndpoint("auf9cn18w1qbv.iot.us-east-2.amazonaws.com", 8883)
myMQTTClient.configureCredentials("/home/pi/certs/root-CA.pem", "/home/pi/certs/4b4aef80d1-private.pem.key", "/home/pi/certs/4b4aef80d1-certificate.pem.crt")
myMQTTClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
myMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
myMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
myMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec
 
#connect and publish
myMQTTClient.connect()
myMQTTClient.publish("homesec/cam", "connected", 0)
 
#loop and publish sensor reading
while 1:
    now = datetime.utcnow()
    now_str = now.strftime('%Y-%m-%dT%H:%M:%SZ') #e.g. 2016-04-18T06:12:25.877Z
    #instance = dht11.DHT11(pin = 4) #BCM GPIO04
    #result = instance.read()
    #if result.is_valid():
    payload = '{ "Date_and_time:": "' + now_str + '","Intruder": ' + "name to be replaced" + ' }'
    print (payload)
    myMQTTClient.publish("homesec/cam", payload, 0)
    sleep(5)
    