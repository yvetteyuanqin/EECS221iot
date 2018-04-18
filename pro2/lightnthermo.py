
import spidev
import time
import os
import RPi.GPIO as GPIO
import Adafruit_GPIO.SPI as SPI
import Adafruit_MCP3008

SPI_PORT   = 0
SPI_DEVICE = 0
mcp = Adafruit_MCP3008.MCP3008(spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE))

ONBOARDLED = 17

def setup():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(ONBOARDLED,GPIO.OUT)
    # Open SPI bus
    
 
# Function to read SPI data from MCP3008 chip
# Channel must be an integer 0-7
def ReadChannel(channel):
  data = mcp.read_adc(channel)
  #data = ((adc[1]&3) << 8) + adc[2]
  print(data)
  return data
 
# Function to convert data to voltage level,
# rounded to specified number of decimal places.
def ConvertVolts(data,places):
  volts = (data * 3.3) / float(1023)
  volts = round(volts,places)
  return volts
 
# Function to calculate temperature from
# TMP36 data, rounded to specified
# number of decimal places.
def ConvertTemp(data,places):
 
  # ADC Value
  # (approx)  Temp  Volts
  #    0      -50    0.00
  #   78      -25    0.25
  #  155        0    0.50
  #  233       25    0.75
  #  310       50    1.00
  #  465      100    1.50
  #  775      200    2.50
  # 1023      280    3.30
 
  temp = ((data * 330)/float(1023))-50
  temp = round(temp,places)
  return temp
 
# Define sensor channels
light_channel = 0
temp_channel  = 1
 
# Define delay between readings
delay = 0.5
def lightnthermo():
    while True:
        
        # Read the light sensor data
      light_level = ReadChannel(light_channel)
      light_volts = ConvertVolts(light_level,2)
     #turn on on board light if above threshold
      GPIO.output(ONBOARDLED,GPIO.LOW)
      if light_level>80:
          GPIO.output(ONBOARDLED,GPIO.HIGH)
      # Read the temperature sensor data
      #temp_level = ReadChannel(temp_channel)
      #temp_volts = ConvertVolts(temp_level,2)
      #temp       = ConvertTemp(temp_level,2)
     
      # Print out results
      #print("--------------------------------------------")
      #print("Light: {} ".format(light_level))
      #print("Temp : {} ({}V) {} deg C".format(temp_level,temp_volts,temp))
     
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
 
