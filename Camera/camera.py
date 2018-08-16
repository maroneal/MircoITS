from picamera import PiCamera
from picamera.array import PiRGBArray
from time import sleep
import cv2

camera = PiCamera()

"""******************prise de photos**********************************
camera.start_preview()
for i in range(5):
    sleep(5)
    camera.capture('/home/pi/Desktop/MicroITS/Camera/image%s.jpg'% i)
    
camera.stop_preview()"""

"""******************prise de photos*******************************"""
camera.start_preview()
camera.start_recording('/home/pi/Desktop/MicroITS/Camera/video.h264')
sleep(5)
camera.stop_recording()
camera.stop_preview()

