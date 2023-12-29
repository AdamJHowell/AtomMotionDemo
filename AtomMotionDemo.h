//
// Created by adam.howell on 2023-12-28.
//

#ifndef ATOMMOTIONDEMO_ATOMMOTIONDEMO_H
#define ATOMMOTIONDEMO_ATOMMOTIONDEMO_H

#include <Wire.h>
#include <M5Atom.h>
#include <M5_DLight.h>
#include "AtomMotion.h"


#define PORT_B	 23
#define PORT_C	 22
#define RED		 0xFF0000
#define ORANGE	 0xFF8000
#define YELLOW	 0xFFFF00
#define GREEN	 0x00FF00
#define BLUE	 0x0000FF
#define INDIGO	 0x4B0082
#define VIOLET	 0xEE82EE
#define BLACK	 0x000000
#define MAGENTA 0xFF00FF
#define CYAN	 0x00FFFF
#define WHITE	 0xFFFFFF


AtomMotion Atom;
xSemaphoreHandle CtlSemaphore;
bool direction = true;
unsigned int speed = 180;
unsigned int buttonCount = 0;
unsigned long lastLoop = 0;
const unsigned int numSensors = 4; // The number of sensors.
const unsigned long loopDelay = 10; // The maximum value of 4,294,967,295 allows for a delay of about 49.7 days.
const byte sdaGPIO = 26;			  // Use this to set the SDA GPIO if your board uses a non-standard GPIOs for the I2C bus.
const byte sclGPIO = 32;			  // Use this to set the SCL GPIO if your board uses a non-standard GPIOs for the I2C bus.
const int PCA_ADDRESS = 0x70;		  // The I2C address of the Pa.HUB.
uint16_t luxArray[4];				  // An array to hold light values.
M5_DLight sensorArray[4];			  // An array to hold sensors.
unsigned int sensorAddresses[4];	  // An array of sensor port numbers {0, 1, 4, 5};


void pcaSelect( uint8_t i );
void GetStatus();
void TaskMotion( void *pvParameters );
void setup();
void loop();

#endif //ATOMMOTIONDEMO_ATOMMOTIONDEMO_H
