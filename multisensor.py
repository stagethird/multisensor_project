#!/usr/bin/env python3
import serial
import smtplib
import time
import sys

# Email variables
serverAddress = "smtp.DOMAIN.com"
loginName = "LOGIN"
loginPassword = "PASSWORD"
senderAddress = "SENDER@ADDRESS.COM"
recipientAddress = "RECIPIENT@ADDRESS.COM"

emailSent = False
timeDelay = 3600 # Wait time before a second email can be sent.
                 # 3600 Seconds = 1 hour
timeRef = 0
dict = {'accelTriggered': False, 'accelTotal': 0,
'tempTriggered': False, 'Temp': 0.0,
'motionTriggered': False, 'motionTotal': 0}

emailBody = """\
Accelerometer Triggered = {}
Number of times = {}

Temp = {} *F

Motion Sensor Triggered = {}
Number of Times = {}"""

try:
    ser = serial.Serial('/dev/ttyACM0', 115200, timeout = 1)
    ser.reset_input_buffer()
except Exception as e:
    print("Exception", e)
    sys.exit("Check connection to serial device")

while True:
    if ser.in_waiting > 0:
        message = ser.readline().decode('Ascii').rstrip()
        if message == 'start':
            while not message == 'end':
                if ser.in_waiting > 0:
                    message = ser.readline().decode('Ascii').rstrip()
                    if message != 'end':
                        templist = message.split('=')
                        x = templist[0]
                        y = templist[1]
                        if x in dict:
                            dict[x] = y

            if 'True' in dict.values() and not emailSent:
                # Send email here
                server = smtplib.SMTP_SSL(serverAddress, 465)
                server.login(loginName, loginPassword)
                server.sendmail(
                    senderAddress,
                    recipientAddress,
                    emailBody.format(dict['accelTriggered'],
                    dict['accelTotal'], dict['Temp'], dict['motionTriggered'],
                    dict['motionTotal']))
                server.quit()

                emailSent = True # Block sending of multiple emails
                timeRef = int(time.time()) # This sets timeRef to (whole)
                                           # seconds since epoch
                                           # (Jan. 1, 1970, 00:00)

    time.sleep(1.0) # Throttles program to reduce processor load
    timeCheck = int(time.time())
    if timeRef and timeCheck - timeRef >= timeDelay: # Allow sending of email
        emailSent = False                            # after 'timeDelay' seconds

