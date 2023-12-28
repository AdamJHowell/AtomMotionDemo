/*
ATOM Motion:
    SCL 21
    SDA 25
    PORT.B (black connector), 23 white, 33 yellow
    PORT.C (blue connector), 19 yellow, 22 white
Servo angle range 0 ~ 180
DC motor speed range -127~127
ATOM Lite HY2.0-4P:
    G, 5V, G26, G32
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
unsigned long lastLoop = 0;
unsigned int speed = 180;
unsigned int buttonCount = 0;
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
			M5.dis.drawpix( 0, RED );
		}
		else
		{
			Atom.SetMotorSpeed( 1, -100 );
			Atom.SetMotorSpeed( 2, -100 );
			M5.dis.drawpix( 0, BLUE );
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
	M5.dis.drawpix( 0, WHITE );

	Wire.begin( sdaGPIO, sclGPIO );
}


void loop()
{
	M5.update();

	if( millis() - lastLoop >= 50 )
	{
		if( M5.Btn.wasPressed() )
		{
			direction = !direction;
			if( speed == 180 )
				speed = 0;
			else
				speed = 180;
			Serial.printf( "New speed: %d\n", speed );
			switch( buttonCount )
			{
				case 0:
					M5.dis.drawpix( 0, RED );
					break;
				case 1:
					M5.dis.drawpix( 0, ORANGE );
					break;
				case 2:
					M5.dis.drawpix( 0, YELLOW );
					break;
				case 3:
					M5.dis.drawpix( 0, GREEN );
					break;
				case 4:
					M5.dis.drawpix( 0, BLUE );
					break;
				case 5:
					M5.dis.drawpix( 0, INDIGO );
					break;
				case 6:
					M5.dis.drawpix( 0, VIOLET );
					break;
				default:
					break;
			}
			buttonCount++;
			if( buttonCount >= 7 )
				buttonCount = 0;
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
		lastLoop = millis();
	}
}
