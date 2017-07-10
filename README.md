# ECEN361-Air-Hockey-Project

# trackPuck - the main code for executing the air hockey table code on the pi. As long as the correct port for the serial output is selected on the pi, it should run autonomously. 

# puckLib - defines a funtion calcPaddlePosition() that trackPuck uses to determinewhere the paddle should be positioned to block the puck. It also maps the camera's pixel coordinates to the arduino's coordinate system.

# simpleSerial - a test program that simply tests to see if the serial communicationbetween the raspberry pi and the ariduino is working correctly (without having toworry about the camera). It expects the user to enter a three-digit number, a comma, and then another three-digit number (no spaces), otherwise, it will not work. 
