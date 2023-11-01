/*
scl 21
sda 25
PORT.B, 23 white, 33 yellow
PORT.C, 19 yellow, 22 white
Servo angle range 0 ~ 180
DC motor speed range -127~127
ATOM LITE HY2.0-4P:
  G, 5V, G26, G32
ADS1115 address 0x48
SHT40 default address: 0x44
APDS-9960 default address: 0x39
*/


/**
I2CBusScanner will scan the I2C bus for device addresses in the range of 0x08 to 0x77.
Using the default SDA and SCL GPIOs.
Addresses with a device will be represented by '#'.
Addresses without a device will be represented by '-'.
Addresses which return an error will be represented by 'E'.
    0123456789ABCDEF
0x0         --------
0x1 ----------------
0x2 ----------------
0x3 ---------#------
0x4 ----#---#-------
0x5 ----------------
0x6 ----------------
0x7 --------

3 devices found.
Address: 0x39
Address: 0x44
Address: 0x48

Scan # 12 complete.
Pausing for 5 seconds.
 */


#include <M5Atom.h>
#include "AtomMotion.h"
#include <Adafruit_ADS1X15.h>


#define PORT_B 23
#define PORT_C 22


AtomMotion Atom;
Adafruit_ADS1115 ads;
xSemaphoreHandle CtlSemaphore;
bool direction = true;
unsigned int speed = 180;
unsigned int FSM = 0;
const byte sdaGPIO = 26; // Use this to set the SDA GPIO if your board uses a non-standard GPIOs for the I2C bus.
const byte sclGPIO = 32; // Use this to set the SCL GPIO if your board uses a non-standard GPIOs for the I2C bus.


void GetStatus()
{
	for( int ch = 1; ch < 5; ch++ )
		Serial.printf( "Servo Channel %d: %d \n", ch, Atom.ReadServoAngle( ch ) );
	Serial.printf( "Motor Channel %d: %d \n", 1, Atom.ReadMotorSpeed( 1 ) );
	Serial.printf( "Motor Channel %d: %d \n", 2, Atom.ReadMotorSpeed( 2 ) );
}


void TaskMotion( void *pvParameters )
{
	while( 1 )
	{
		for( int ch = 1; ch < 5; ch++ )
			Atom.SetServoAngle( ch, 180 );
		GetStatus();
		vTaskDelay( 1000 / portTICK_RATE_MS );
		for( int ch = 1; ch < 5; ch++ )
			Atom.SetServoAngle( ch, 0 );
		GetStatus();
		vTaskDelay( 1000 / portTICK_RATE_MS );
		if( direction )
		{
			Atom.SetMotorSpeed( 1, 100 );
			Atom.SetMotorSpeed( 2, 100 );
			M5.dis.drawpix( 0, 0xff0000 );
		}
		else
		{
			Atom.SetMotorSpeed( 1, -100 );
			Atom.SetMotorSpeed( 2, -100 );
			M5.dis.drawpix( 0, 0x0000ff );
		}
	}
}


void setup()
{
	M5.begin( true, false, true );
	Atom.Init();
	vSemaphoreCreateBinary( CtlSemaphore );
	xTaskCreatePinnedToCore(
		 TaskMotion,	// Pointer to the task entry function.
		 "TaskMotion", // A descriptive name for the task.
		 4096,			// The size of the task stack specified as the number of bytes.
		 NULL,			// Pointer that will be used as the parameter for the task being created.
		 2,				// Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		 NULL,			// Used to pass back a handle by which the created task can be referenced.
		 0 );				// Values 0 or 1 indicate the index number of the CPU which the task should be pinned to.
	pinMode( PORT_B, INPUT_PULLUP );
	pinMode( PORT_C, INPUT_PULLUP );
	M5.dis.drawpix( 0, 0xffffff ); // White

	Wire.begin( sdaGPIO, sclGPIO );

	if( !ads.begin() )
	{
		Serial.println( "Failed to initialize ADS." );
		while( 1 )
			delay( 10 );
	}
}


void loop()
{
	int16_t adc0 = ads.readADC_SingleEnded( 0 );
	int16_t adc1 = ads.readADC_SingleEnded( 1 );
	int16_t adc2 = ads.readADC_SingleEnded( 2 );
	int16_t adc3 = ads.readADC_SingleEnded( 3 );
	float volts0 = ads.computeVolts( adc0 );
	float volts1 = ads.computeVolts( adc1 );
	float volts2 = ads.computeVolts( adc2 );
	float volts3 = ads.computeVolts( adc3 );

	Serial.println( "-----------------------------------------------------------" );
	Serial.printf( "AIN0: %u, %.2fV\n", adc0, volts0 );
	Serial.print( "AIN1: " );
	Serial.print( adc1 );
	Serial.print( "  " );
	Serial.print( volts1 );
	Serial.println( "V" );
	Serial.print( "AIN2: " );
	Serial.print( adc2 );
	Serial.print( "  " );
	Serial.print( volts2 );
	Serial.println( "V" );
	Serial.print( "AIN3: " );
	Serial.print( adc3 );
	Serial.print( "  " );
	Serial.print( volts3 );
	Serial.println( "V" );

	M5.update();
	if( M5.Btn.wasPressed() )
	{
		direction = !direction;
		if( speed == 180 )
			speed = 0;
		else
			speed = 180;
		Serial.printf( "New speed: %d\n", speed );
		switch( FSM )
		{
			case 0:
				M5.dis.drawpix( 0, 0xfff000 ); // YELLOW 黄色
				break;
			case 1:
				M5.dis.drawpix( 0, 0xff0000 ); // RED  红色
				break;
			case 2:
				M5.dis.drawpix( 0, 0x0000ff ); // BLUE  蓝色
				break;
			case 3:
				M5.dis.drawpix( 0, 0x00ff00 ); // GREEN  绿色
				break;
			default:
				break;
		}
		FSM++;
		if( FSM >= 4 )
			FSM = 0;
	}
	if( !digitalRead( PORT_B ) )
	{
		Atom.SetServoAngle( 4, 90 );
		Serial.printf( "Hit limit B!\n" );
	}
	else
		Atom.SetServoAngle( 4, speed );
	if( !digitalRead( PORT_C ) )
	{
		Atom.SetServoAngle( 2, 90 );
		Serial.printf( "Hit limit C!\n" );
	}
	else
		Atom.SetServoAngle( 2, speed );
	delay( 100 );
}
