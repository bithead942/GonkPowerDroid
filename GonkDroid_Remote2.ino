/**********************************************************************************
   Gonk Droid Remote
   by Bithead942

    Board:  Fio v3 3.3V/8MHz

  Remote control of the voice playback for the Gonk platform

  The Fio v3 board interfaces with the Wii Nunchuck controller via the Nunchucky interface board.
  It initiates communication by sending a set of a string of values read from the Nunchuck.
  - Joystick X Position
  - Joystick Y Position
  - Accelerometer X Axis (not used)
  - Accelerometer Y Axis (not used)
  - Accelerometer Z Axis (not used)
  - Z Button (big)
  - C Button (small)

  PINS USED:
  Serial1 (Xbee)
  Tx       0
  Rx       1

  Nunchuck (I2C):
  A2       Ground
  A3       3.3V
  A4       Data
  A5       Clock

*************************************************************************************/

#include <Wire.h>
#include "nunchuck_funcs.h"

#define Xbee Serial1
#define Debug Serial
#define AUX1Pin 7
#define AUX2Pin 8
#define AUX3Pin 9

int loop_cnt = 0;
String outString = "";    // string to hold output

byte joyx, joyy, zbtn, cbtn, aux1, aux2, aux3;
//int ledPin = 13;


void setup()
{
  pinMode(AUX1Pin, INPUT);
  pinMode(AUX2Pin, INPUT);
  pinMode(AUX3Pin, INPUT);
  
  Debug.begin(9600);
  Xbee.begin(57600);  // Serial1 set up Serial1 port

  //nunchuck_setpowerpins();
  nunchuck_init(); // send the initilization handshake

  Debug.print("WiiChuckDemo ready\n");
}

void loop()
{
  if ( loop_cnt >= 100 ) { // every 100 msecs get new data
    loop_cnt = 0;
    outString = "";

    nunchuck_get_data();
    //nunchuck_print_data();

    joyx  = nunchuck_joyx();  // ranges from 30 - 226;  131 center
    joyy  = nunchuck_joyy();  // ranges from 38 - 231;  135 center
    zbtn = nunchuck_zbutton();
    cbtn = nunchuck_cbutton();
    aux1 = digitalRead(AUX1Pin);
    aux2 = digitalRead(AUX2Pin);
    aux3 = digitalRead(AUX3Pin);

    outString += (byte)joyx;
    outString += ",";
    outString += (byte)joyy;
    outString += ",";
    outString += (byte)zbtn;
    outString += ",";
    outString += (byte)cbtn;
    outString += ",";
    outString += (byte)aux1;
    outString += ",";
    outString += (byte)aux2;
    outString += ",";
    outString += (byte)aux3;

    Xbee.println(outString);
    Debug.println(outString);
  }
  loop_cnt++;
  delay(1);
}
