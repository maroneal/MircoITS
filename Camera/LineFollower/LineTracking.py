# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
from time import sleep
import cv2
import numpy as np
 
# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (640, 480)
camera.framerate = 32
rawCapture = PiRGBArray(camera)
i=1;

# allow the camera to warmup
sleep(0.1)
# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
        # grab the raw NumPy array representing the image - this array
        # will be 3D, representing the width, height, and # of channels
        image = frame.array

        crop_img = image[320:640, 0:480]
        
        gray = cv2.cvtColor(crop_img, cv2.COLOR_BGR2GRAY)

        blur=cv2.GaussianBlur(gray,(5,5),0)
        
        ret, thresh = cv2.threshold(blur,100,255,cv2.THRESH_BINARY_INV)
        
        # Erode and dilate to remove accidental line detections
        mask = cv2.erode(thresh, None, iterations=2)
        mask = cv2.dilate(mask, None, iterations=2)
        
        # Find the contours of the frame
        _,contours,_ = cv2.findContours(mask, 1, cv2.CHAIN_APPROX_NONE)

        # Find the biggest contour (if detected)

        if len(contours) > 0:

                c = max(contours, key=cv2.contourArea)
                M = cv2.moments(c)
 
                cx = int(M['m10']/M['m00'])
                cy = int(M['m01']/M['m00'])
 
                cv2.line(crop_img,(cx,0),(cx,720),(255,0,0),1)
                cv2.line(crop_img,(0,cy),(1280,cy),(255,0,0),1)

                cv2.drawContours(crop_img, contours, -1, (0,255,0), 1)

                if cx >= 360:
                    print ("Turn Right")

                if cx < 360 and cx > 120:
                    print ("On Track!")

                if cx <= 120:
                    print ("Turn Left")

        else:
                print("I don't see the line")
        
        # show the frame
        cv2.imshow("Frame", mask)
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
        

