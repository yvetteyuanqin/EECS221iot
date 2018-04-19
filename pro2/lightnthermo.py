
import spidev
import time
import os
import RPi.GPIO as GPIO
import Adafruit_GPIO.SPI as SPI
import Adafruit_MCP3008
import glob


SPI_PORT   = 0
SPI_DEVICE = 0
mcp = Adafruit_MCP3008.MCP3008(spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE))
base_dir = '/sys/bus/w1/devices/'
device_folder=glob.glob(base_dir+'28*')[0]
device_file=device_folder+'/w1_slave'
ONBOARDLED = 18
SELECTION = 

def setup():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(ONBOARDLED,GPIO.OUT)
    GPIO.setup()
    # Open SPI bus
    os.system('modprobe w1-gpio')
    os.system('modprobe w1-therm')
    
 
# Function to read SPI data from MCP3008 chip
# Channel must be an integer 0-7
def ReadChannel(channel):
  data = mcp.read_adc(channel)
  #data = ((adc[1]&3) << 8) + adc[2]
  return data
 
# Function to convert data to voltage level,
# rounded to specified number of decimal places.
def ConvertVolts(data,places):
  volts = (data * 3.3) / float(1023)
  volts = round(volts,places)
  return volts
 
# Function to calculate temperature from
def read_temp_raw():
    f = open(device_file, 'r')
    lines = f.readlines()
    f.close()
    return lines
def read_temp():
    lines = read_temp_raw()
    while lines[0].strip()[-3:] != 'YES':
        time.sleep(0.2)
        lines = read_temp_raw()
    equals_pos = lines[1].find('t=')
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
        temp_c = float(temp_string) / 1000.0
        temp_f = temp_c * 9.0 / 5.0 + 32.0
        return temp_c, temp_f
# Define sensor channels
light_channel = 0

 
# Define delay between readings
delay = 0.5
def lightnthermo():
    while True:
        
        # Read the light sensor data
      light_level = ReadChannel(light_channel)
      light_volts = ConvertVolts(light_level,2)
     #turn on on board light if above threshold
      GPIO.output(ONBOARDLED,GPIO.LOW)
      if light_level<400:
          GPIO.output(ONBOARDLED,GPIO.HIGH)
      # Read the temperature sensor data
      temp_c,temp_f = read_temp()
     
      # Print out results
      print("--------------------------------------------")
      print("Light: {} ".format(light_level))
      print("Temp : {} deg C {} deg F".format(temp_c,temp_f))
     
      # Wait before repeating loop
      time.sleep(delay)
      
def destroy():
    GPIO.cleanup()

if __name__=='__main__':
    
    setup()
    try:
        lightnthermo()
    except KeyboardInterrupt:
        print("Cleanning up")
    finally:
        destroy()
 
