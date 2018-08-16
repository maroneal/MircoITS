import socket

TCP_IP = "127.0.0.1"
TCP_PORT = 4200
xboxInput = [0,0]

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP
sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    xboxInput = data.split(" ")

    print xboxInput[0]
    print xboxInput[1]



    print "received message:", data
