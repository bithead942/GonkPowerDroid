/********************************************************************************************
   Gonk Droid OnBoard
   by Bithead942

    Board:  Mega 2560 R3

  On board control of the Gonk Droid Functions

  The Mega 2560 board interfaces with the Xbee to receive Nunchuck commands and take action
  - Joystick X Position (23 - 226, 131 center)
  - Joystick Y Position (38 - 233, 133 center)
  - Z Button (big) - Play Track 0
  - C Button (small) - Spray Can
  - Aux 1 - Play Track 1
  - Aux 2 - Play Track 2
  - Aux 3 - Play Track 3

  PINS USED:
  Serial1 (Xbee)
  Tx       0
  Rx       1
  Xbee RX  2 (19)
  Xbee TX  3 (18)
  C Btn    5  (SprayCan Servo)
  Sound TX 14 (to 3-pin plug)
  Leg TX   16
  Leg RX   17

  Reqire pins on shield for Xbee control
  Shield 2 => Mega 19
  Shield 3 => Mega 18

  Sounds:
  Track 0 - Short Gonk Sound
  Track 1 - Creature Cantina (0:30 sec)
  Track 2 - Star Wars Theme (0:25 sec)
  Track 3 - Industrial Sounds (2:25 sec)
********************************************************************************************/

#include "Adafruit_Soundboard.h";
#include <Servo.h>

#define Debug Serial
#define Xbee Serial1   //19 (RX) and 18 (TX)
#define Legs Serial2   //17 (RX) and 16 (TX)
#define Sound Serial3 //14 (TX)
#define cbtnPin 5
#define SprayDelay 300 // x 100ms

// pass the serial pointer to Adafruit_soundboard, the second
// argument is the debug port (not used really) and the third 
// arg is the reset pin
Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial3, NULL, 4);

// Setup servo for spray can
Servo SprayCan;

int inByte = 0;         // incoming serial byte
String inString = "";    // string to hold input
byte joyx, joyy, zbtn, cbtn, aux1, aux2, aux3;
int MoveCounter = 0;
int SprayCounter = 0;

void setup() {
  pinMode (cbtnPin, OUTPUT);

  // initialize Serial ports
  Debug.begin(9600);
  Xbee.begin(57600);
  Legs.begin(115200);
  Sound.begin(9600);
  
  sfx.reset();
  sfx.playTrack((uint8_t)1);   //Play Short Gonk Sound to confirm start-up
  
  Legs.println("PL 0");
  SprayCan.attach(cbtnPin);
  SprayCan.write(90);  //set to down, resting position
  
  Debug.println("Ready.");
}

void loop() {   // 10 times per second
  while (Xbee.available() > 0) {
    int inChar = Xbee.read();
    //Debug.print(inChar);
    inString += (char)inChar;
    // if you get a newline, parse and print the data
    if (inChar == '\n') {
      if (ParseInput(inString) == 0) {
        //PrintData();
        TakeAction();
      }
      else {
        Debug.println("Input Error");
      }
      //Debug.println(inString);
      inString = "";
    }
  }
  delay(100);
}

int ParseInput(String inString) {
  String tempString = "";
  int i = 0;

  if (isDigit(inString[0]) == false)  return 1;   //First character must be a digit

  while (i < inString.length() && inString[i] != ',')
  {
    tempString += inString[i];
    i++;
  }
  joyx = tempString.toInt();
  if (joyx > 255) return 1;
  tempString = "";
  i++;

  while (i < inString.length() && inString[i] != ',')
  {
    tempString += inString[i];
    i++;
  }
  joyy = tempString.toInt();
  if (joyy > 255) return 1;
  tempString = "";
  i++;

  while (i < inString.length() && inString[i] != ',')
  {
    tempString += inString[i];
    i++;
  }
  zbtn = tempString.toInt();
  if (zbtn > 1) return 1;
  tempString = "";
  i++;

  while (i < inString.length() && inString[i] != ',')
  {
    tempString += inString[i];
    i++;
  }
  cbtn = tempString.toInt();
  if (cbtn > 1) return 1;
  tempString = "";
  i++;

  while (i < inString.length() && inString[i] != ',')
  {
    tempString += inString[i];
    i++;
  }
  aux1 = tempString.toInt();
  if (aux1 > 1) return 1;
  tempString = "";
  i++;

  while (i < inString.length() && inString[i] != ',')
  {
    tempString += inString[i];
    i++;
  }
  aux2 = tempString.toInt();
  if (aux2 > 1) return 1;
  tempString = "";
  i++;
  
  while (i < inString.length() && inString[i] != ',')
  {
    tempString += inString[i];
    i++;
  }
  aux3 = tempString.toInt();
  if (aux3 > 1) return 1;
  
  return 0;
}

void PrintData() {
  Debug.print("joy:");
  Debug.print((byte)joyx, DEC);
  Debug.print(",");
  Debug.print((byte)joyy, DEC);
  Debug.print("\t");

  Debug.print("but:");
  Debug.print((byte)zbtn, DEC);
  Debug.print(",");
  Debug.print((byte)cbtn, DEC);
  Debug.print(",");
  Debug.print((byte)aux1, DEC);
  Debug.print(",");
  Debug.print((byte)aux2, DEC);
  Debug.print(",");
  Debug.print((byte)aux3, DEC);
  
  Debug.println();  // newline
}

void TakeAction() {
  ////////////////////
  // Sounds
  ////////////////////
  if (zbtn == 1) {
    sfx.playTrack((uint8_t)1);   //Short Gonk
    Debug.println("Playing Sound");
  }

  if (aux1 == 1) {
    sfx.playTrack((uint8_t)0);   //Long Gonk & Industrial noises
    Debug.println("Playing Sound");
  }


  if (aux2 == 1) {
    sfx.playTrack((uint8_t)2);   //Star Wars Theme
    Debug.println("Playing Sound");
  }


  if (aux3 == 1) {
    sfx.playTrack((uint8_t)3);  //Creature Cantina Theme
    Debug.println("Playing Sound");
  }

  ////////////////////
  // Spray Can
  ////////////////////
  if (SprayCounter > 0) {   //Avoid spraying too often
    SprayCounter--;   // Spray can is warming up
  }
  
  if (cbtn == 1 and SprayCounter == 0) {
    //Brace legs?
    SprayCan.write(120);
    delay(400);
    SprayCan.write(90);  //back to resting position
    delay(500);
    SprayCounter = SprayDelay;
  }

  ////////////////////
  // Move Legs
  ////////////////////
  if (MoveCounter > 0) {
    MoveCounter--;   // Legs are moving, ignore new commands
  }
  else {
    /*Debug.print(joyx);
    Debug.print(", ");
    Debug.print(joyy);
    Debug.print(": "); */

    if (joyx >= 121 && joyx <= 141 && joyy >= 124 && joyy <= 144) {
      Legs.println("PL0 SQ0 ONCE");   //Stand (131, 134)
      Debug.println("Stand");
      MoveCounter = 10;
    }
    else if (joyx >= 113 && joyx <= 133 && joyy >= 223 && joyy <= 243) {
      Legs.println("PL0 SQ1 ONCE");   //Forward (123, 233)
      Debug.println("Forward");
      MoveCounter = 28;
    }
    else if (joyx >= 39 && joyx <= 59 && joyy >= 196 && joyy <= 216) {
      Legs.println("PL0 SQ2 ONCE");   //Fwd Left (49, 206)
      Debug.println("Fwd Left");
      MoveCounter = 28;
    }
    else if (joyx >= 188 && joyx <= 208 && joyy >= 197 && joyy <= 217) {
      Legs.println("PL0 SQ3 ONCE");   //Fwd Right (198, 207)
      Debug.println("Fwd Right");
      MoveCounter = 28;
    }
    else if (joyx >= 14 && joyx <= 45 && joyy >= 90 && joyy <= 144) {
      Legs.println("PL0 SQ4 ONCE");   //Left (24, 134)
      Debug.println("Left");
      MoveCounter = 15;
    }
    else if (joyx >= 200 && joyx <= 234 && joyy >= 90 && joyy <= 146) {
      Legs.println("PL0 SQ5 ONCE");   //Right (224, 136)
      Debug.println("Right");
      MoveCounter = 15;
    }
    else if (joyx >= 113 && joyx <= 133 && joyy >= 28 && joyy <= 48) {
      Legs.println("PL0 SQ6 ONCE");   //March (123, 38)
      Debug.println("March");
      MoveCounter = 20;
    }
    else {
      Legs.println("PL0");
      Debug.println("Leg Error");
    }
  }
}
