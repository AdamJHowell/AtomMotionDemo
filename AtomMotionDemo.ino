/*
scl 21
sda 25
PORT.B, 23 white, 33 yellow
PORT.C, 19 yellow, 22 white
Servo angle range 0 ~ 180
DC motor speed range -127~127
*/


#include <M5Atom.h>
#include "AtomMotion.h"


#define PORT_B 23
#define PORT_C 22


AtomMotion Atom;
xSemaphoreHandle CtlSemaphore;
bool direction = true;


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
    // A name just for task.
    TaskMotion, "TaskMotion"
    // This stack size can be checked & adjusted by reading the Stack Highwater.
    , 4096
    // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    , NULL, 2
    // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    , NULL, 0 );
    pinMode( PORT_B, INPUT_PULLUP );
    pinMode( PORT_C, INPUT_PULLUP );
}


void loop()
{
  M5.update();
  if( M5.Btn.wasPressed() )
    direction = !direction;
  if( !digitalRead( PORT_B ) )
    Serial.printf( "Hit limit B!\n" );
  if( !digitalRead( PORT_C ) )
    Serial.printf( "Hit limit C!\n" );
  delay( 100 );
}
