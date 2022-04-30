#! /usr/bin/env python3
import serial
import time
from subprocess import call

def clear():
    _ = call("clear")

ser = serial.Serial('/dev/ttyACM0', 115200, timeout = 0.50)
clear()
while True:
    while ser.in_waiting:
        message = ser.readline() # Recieve data
        # print("\33[A \33[A", message.decode('Ascii')) # decode() function converts from byte format
        print(message.decode('Ascii'))
        time.sleep(0.5)
