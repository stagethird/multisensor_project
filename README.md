# Multisensor Project (multisensor_project)
This project uses the program `multisensor_project.ino` running on an
Arduino to poll three different sensors:
* A motion sensor
* An accelerometer
* A temperature sensor

It can provide feedback via three LEDs and a buzzer that makes
different tones depending on which sensor is triggered. The buzzer has
a switch wired in series allowing it to be silenced.

The arduino keeps track of the number of times sensors have been
triggered, and passes this information and the sensors' current state
via serial connection to a Raspberry Pi.
_____
The Raspberry Pi runs the program `multisensor.py`, which reads the
serial port and collects the data from the Arduino, sending out an
email if the motion sensor or accelerometer have been triggered, or
if the temperature meets or exceeds 80F. A timer limits the sending of
emails to a maximum of one an hour.
_____
The Python script `comm_monitor.py` is a simple program that monitors
the comm port and sends incoming data to the screen for testing
purposes.
