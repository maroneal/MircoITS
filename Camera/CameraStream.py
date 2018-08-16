# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
from time import sleep
import cv2
 
# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (320, 240)
camera.framerate = 32
rawCapture = PiRGBArray(camera, size=(320, 240))
i=1;

# allow the camera to warmup
sleep(0.1)
# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
        # grab the raw NumPy array representing the image - this array
        # will be 3D, representing the width, height, and # of channels
        image = frame.array
 
        # show the frame
        cv2.imshow("Frame", image)
        key = cv2.waitKey(1) & 0xFF
 
        # clear the stream in preparation for the next frame
        rawCapture.truncate(0)

        #Press "r" to start a record and "s" to stop
        if key == ord("r"):
                camera.start_recording('/home/pi/Desktop/MicroITS/Camera/video.h264')
                if key == ord("s"):
                        camera.stop_recording()

        # if the `p` key was pressed, take a picture
        if key == ord("p"):
                camera.capture('/home/pi/Desktop/MicroITS/Camera/capture%s.jpg' %i)
                i= i+1
                
        # if the `q` key was pressed, break from the loop
        if key == ord("q"):
                cv2.destroyAllWindows()
                break
        
