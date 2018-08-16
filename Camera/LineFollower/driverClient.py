import socket
import time
import xboxdrv

TCP_IP = "127.0.0.1"
TCP_PORT = 4200

print "TCP target IP:", TCP_IP
print "TCP target port:", TCP_PORT

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

while True:
    if ord('a') == True:
        MESSAGE[0] = "1.0000"
    else:
        MESSAGE[0] = "0.0000"


    sock.sendto(MESSAGE, (TCP_IP, TCP_PORT))
    time.sleep(5)
