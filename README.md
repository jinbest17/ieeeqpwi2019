# ieeeqpwi2019

This repo currently has the example code for all the parts
we will use in our project for reference purposes.
TODO: Still working on master code.

************************************************************
MPU6050_Example.ino
************************************************************
Example code that calculates realtime angle with MPU6050 
The example code demonstrates every function of MPU6050 and prints out the data
onto screen. 

From Lesson 16 GY-521 Module & MPU-6050 on Youtube
Video Link: https://www.youtube.com/watch?v=y5X2zwbO6e4&t=4s

Elegoo Tutorial's Github link: 
https://github.com/rmorenojr/ElegooTutorial/tree/master/Lesson%2016%20-%20GY-521%20Module

*************************************************************
Transceiver_Example.ino
*************************************************************
From nRF24L01+ Tutorial by Rahui Iyer

The example code demonstrates two arduino sending data back and forth with nRF24L01+ transceivers.
The same code is loaded onto both terminals, and both can switch between sender/receiver.

Link: https://www.deviceplus.com/how-tos/arduino-guide/nrf24l01-rf-module-tutorial/

*************************************************************
Solve_SPI_Confilct.ino
*************************************************************
How to solve the conflict by using Arduino both as SPI slave to receive transmission from radio chip and
as a master to write data to sd card to record wrong posture. 

Example from SD Shield, SPI Conflicting posted by LeahM
His program has a conflict when geting data from AD and writing it to an SD card, both of which uses SPI bus.
Link: https://forum.arduino.cc/index.php?topic=157006.0

*************************************************************
TODO: the write code for sd card
*************************************************************
Documentation of SD card shield: 

https://cdn-learn.adafruit.com/downloads/pdf/adafruit-data-logger-shield.pdf

Additional info:

Logger Shield: Datalogging for Arduino(has RTClib & SDlib downloads)
https://www.instructables.com/id/Logger-Shield-Datalogging-for-Arduino/

Youtube example(temperature recorder):
https://www.youtube.com/watch?v=nCQZWdL3BGE






