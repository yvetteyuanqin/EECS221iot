
 
import sys  
import time 


from netifaces import interfaces, ifaddresses, AF_INET

from socket import *


MYPORT = 4220





# set up sockets for UDP

s=socket(AF_INET, SOCK_DGRAM)
host = 'localhost';
s.bind(('',MYPORT))

print "--------------"







while(1) :
     time.sleep(5)
     data,address = s.recvfrom(4096)
     print("Sensor Value received:",data)
     if data:
         if data<predata:
             sent = sock.sendto("QUICKER", address)
         else:
             sent = sock.sendto("SLOWER", address)


