# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
from time import sleep
import cv2
import numpy as np
import socket
import time

TCP_IP = "127.0.0.1"
TCP_PORT = 4200

print ("TCP target IP:"), TCP_IP
print ("TCP target port:"), TCP_PORT


DataOnLine = 0xAC
DataNoLine = 0xAF
DataRight = 0xAE
DataLeft = 0xAD
DataObject = 0xB0

# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (640, 480)
camera.framerate = 32
rawCapture = PiRGBArray(camera)
i=1;
OnLine=0
NoLine=0
ThreshBin = 90
RightLim = 380
LeftLim = 260
ThreshObjSize = 27000
# allow the camera to warmup
sleep(0.1)
# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    # grab the raw NumPy array representing the image - this array
    # will be 3D, representing the width, height, and # of channels

        image = frame.array

        crop_img_high = image[120:320, 0:640]
        gray = cv2.cvtColor(crop_img_high, cv2.COLOR_BGR2GRAY)
        blur=cv2.GaussianBlur(gray,(5,5),0)
        ret, thresh = cv2.threshold(blur,ThreshBin,255,cv2.THRESH_BINARY_INV)
        
        # Erode and dilate to remove accidental line detections
        mask = cv2.erode(thresh, None, iterations=2)
        mask = cv2.dilate(mask, None, iterations=2)
        
        # Find the contours of the frame
        _,contours,hierarchy = cv2.findContours(mask, 1, cv2.CHAIN_APPROX_NONE)

        # Find the biggest contour (if detected)
        if len(contours) > 0:
            c = max(contours, key=cv2.contourArea)
            M = cv2.moments(c)	
            cx = int(M['m10']/M['m00'])
            cy = int(M['m01']/M['m00'])

            cv2.line(crop_img_high,(cx,0),(cx,720),(255,0,0),1)
            cv2.line(crop_img_high,(0,cy),(1280,cy),(255,0,0),1)

            cv2.drawContours(crop_img_high, contours, -1, (0,255,0), 1)

            if (M['m00']<ThreshObjSize):
            
                crop_img_low = image[200:640, 0:640]

                gray2 = cv2.cvtColor(crop_img_low, cv2.COLOR_BGR2GRAY)
                blur2=cv2.GaussianBlur(gray2,(5,5),0)
                ret, thresh2 = cv2.threshold(blur2,ThreshBin,255,cv2.THRESH_BINARY_INV)

                # Erode and dilate to remove accidental line detections
                mask2 = cv2.erode(thresh2, None, iterations=2)
                mask2 = cv2.dilate(mask2, None, iterations=2)

                # Find the contours of the frame
                _,contours2,_ = cv2.findContours(mask2, 1, cv2.CHAIN_APPROX_NONE)
                

                # Find the biggest contour (if detected)
                if len(contours2) > 0:
                        

                    c = max(contours2, key=cv2.contourArea)
                    M = cv2.moments(c)
                    cv2.imshow("Ligne", mask2)

                    cx = int(M['m10']/M['m00'])
                    cy = int(M['m01']/M['m00'])

                    cv2.line(crop_img_low,(cx,0),(cx,720),(255,0,0),1)
                    cv2.line(crop_img_low,(0,cy),(1280,cy),(255,0,0),1)

                    cv2.drawContours(crop_img_low, contours2, -1, (0,255,0), 1)
        
                    if cx >= RightLim:
                        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                        sock.connect((TCP_IP, TCP_PORT))
                        OnLine = 0
                        NoLine = 0
                        #print ("Turn Right")
                        MESSAGE = TCP_IP+";0;0;0;0;0;0;0;0;"+chr(DataRight)+";"
                        sock.send((MESSAGE).encode())
                        sock.close()                    

                    if cx < RightLim and cx > LeftLim:
                     sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                     sock.connect((TCP_IP, TCP_PORT))
                     #print ("On Track!")
                     MESSAGE =TCP_IP+";0;0;0;0;0;0;0;0;"+chr(DataOnLine)+";"
                     sock.send((MESSAGE).encode())
                     sock.close()
                                        
                    if cx <= LeftLim:
                        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                        sock.connect((TCP_IP, TCP_PORT))
                        OnLine = 0
                        NoLine = 0
                        #print ("Turn Left")
                        MESSAGE = TCP_IP+";0;0;0;0;0;0;0;0;"+chr(DataLeft)+";"
                        sock.send((MESSAGE).encode())
                        sock.close()

                else:
                    if NoLine == 0:
                        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                        sock.connect((TCP_IP, TCP_PORT))
                        NoLine = 1
                        OnLine = 0
                        #print("I don't see the line")
                        MESSAGE = TCP_IP+";0;0;0;0;0;0;0;0;"+chr(DataNoLine)+";"
                        sock.send((MESSAGE).encode())
                        sock.close()
            else:
                if NoLine == 0:
                    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    sock.connect((TCP_IP, TCP_PORT))
                    NoLine = 1
                    OnLine = 0
                    print("I see an obstacle")
                    #MESSAGE = TCP_IP+";0;0;0;0;0;0;0;0;"+chr(DataObject)+";"
                    MESSAGE = TCP_IP+";0;0;0;0;0;0;0;0;"+chr(DataNoLine)+";"
                    sock.send((MESSAGE).encode())
                    sock.close()
        
        # show the frame
        #cv2.imshow("Ligne", mask2)
        cv2.imshow("Obstacle", mask)
        
        #cv2.imshow("Camera", image)
        key = cv2.waitKey(1) & 0xFF
 
        # clear the stream in preparation for the next frame
        rawCapture.truncate(0)

        #Press "r" to start a record and "s" to stop
        if key == ord("r"):
                camera.start_recording('/home/pi/Desktop/MicroITS/Camera/Server/video.h264')
                if key == ord("s"):
                        camera.stop_recording()

        # if the `p` key was pressed, take a picture
        if key == ord("p"):
                camera.capture('/home/pi/Desktop/MicroITS/Camera/Server/capture%s.jpg' %i)
                i= i+1
                
        # if the `q` key was pressed, break from the loop
        if key == ord("q"):
                cv2.destroyAllWindows()
                break
