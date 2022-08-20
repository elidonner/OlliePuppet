#include <Arduino.h>
#line 1 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/ArduinoMega/BrightBin/OllieMega.ino"
/**
 * UnoBright.ino
 * @author Ethan Donlon
 *
 * @details
 * This is code for arudino mega which handles communication between two ESP32 picos
 * a raspberry pi, and a BrightSign LS424 digital media player
 *
 */

#include "binary.h"

#define SERVO_ESP 17
#define MOUTH_ESP 18

Binary binary(52, 50, 48, 46, 44, 42, 40, 38);

const uint8_t numBytes = 32;
uint8_t receivedBytes[numBytes];
uint8_t numReceived = 0;
int initialState;

bool newData = false;

#line 25 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/ArduinoMega/BrightBin/OllieMega.ino"
void recvBytesWithStartEndMarkers();
#line 66 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/ArduinoMega/BrightBin/OllieMega.ino"
int handle_exceptions(int to_play);
#line 122 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/ArduinoMega/BrightBin/OllieMega.ino"
void setup();
#line 139 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/ArduinoMega/BrightBin/OllieMega.ino"
void loop();
#line 25 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/ArduinoMega/BrightBin/OllieMega.ino"
void recvBytesWithStartEndMarkers()
{
  static bool recvInProgress = false;
  static uint8_t ndx = 0;
  uint8_t startMarker = 0x3C; // this is the start marker <
  uint8_t endMarker = 0x3E;   // this is the start marker >
  uint8_t rb;

  while (Serial.available() > 0 && newData == false)
  {
    rb = Serial.read();

    if (recvInProgress == true)
    {
      if (rb != endMarker)
      {
        receivedBytes[ndx] = rb;
        ndx++;
        if (ndx >= numBytes)
        {
          ndx = numBytes - 1;
        }
      }
      else
      {
        receivedBytes[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        numReceived = ndx; // save the number for use when printing
        ndx = 0;
        newData = true;
      }
    }

    else if (rb == startMarker)
    {
      recvInProgress = true;
    }
  }
}


int handle_exceptions(int to_play)
{
  int rand_num;

  if(to_play == 0)
  {
    //options are 0, 0b (32), 0c(33)
    rand_num = random(0,3); //random from 0 (inclusive) to 3 (non-nclusive)
    if(rand_num == 0) {return 0;}
    else if (rand_num == 1) {return 32;}
    else if (rand_num == 2) {return 33;}
  }
  else if (to_play == 2)
  {
    //options are 2, 2b (34)
    rand_num = random(0,2); //random from 0 (inclusive) to 2 (non-nclusive)
    if(rand_num == 0) {return 2;}
    else if (rand_num == 1) {return 34;}
  }
  else if (to_play == 3)
  {
    //options are 3, 3b (35), 3c (36), 3d (37)
    rand_num = random(0,4); //random from 0 (inclusive) to 4 (non-nclusive)
    if(rand_num == 0) {return 3;}
    else if (rand_num == 1) {return 35;}
    else if (rand_num == 2) {return 36;}
    else if (rand_num == 3) {return 37;}
  }
  else if (to_play == 17)
  {
    //options are 17, 17b (38)
    rand_num = random(0,2); //random from 0 (inclusive) to 2 (non-nclusive)
    if(rand_num == 0) {return 17;}
    else if (rand_num == 1) {return 38;}
  }
  else if (to_play == 23)
  {
    //options are 23, 23a (39) 23b (40)
    rand_num = random(0,3); //random from 0 (inclusive) to 3 (non-nclusive)
    if(rand_num == 0) {return 23;}
    else if (rand_num == 1) {return 39;}
    else if (rand_num == 2) {return 40;}
  }
  else if (to_play == 29)
  {
    //options are 29, 29b (41)
    rand_num = random(0,2); //random from 0 (inclusive) to 2 (non-nclusive)
    if(rand_num == 0) {return 29;}
    else if (rand_num == 1) {return 41;}
  }

  //if all fails, return to_play
  return to_play;

}

void setup()
{
  // Start the USB serial port for talking to the rasberry pi
  Serial.begin(115200);

  // Start the serial 1 for talking to esp
  Serial1.begin(115200);
  Serial2.end();
  pinMode(SERVO_ESP, OUTPUT);
  pinMode(MOUTH_ESP, OUTPUT);
  digitalWrite(SERVO_ESP, LOW);
  digitalWrite(MOUTH_ESP, LOW);

  //start binary port
  binary.setup();
}

void loop()
{
  recvBytesWithStartEndMarkers();

  if (newData == true)
  {
    digitalWrite(SERVO_ESP, HIGH);
    digitalWrite(MOUTH_ESP, HIGH);

    String data = "";
    for (uint8_t n = 0; n < numReceived; n++)
    {
      data = data + String(((char)receivedBytes[n]));
    }
    int to_play;

    to_play = data.toInt();

    // if(to_play == 60)
    // {
    //   //turn on mouth
    //   digitalWrite(MOUTH, HIGH);
    // }
    // else if (to_play == 70)
    // {
    //   //turn off mouth
    //   digitalWrite(MOUTH, LOW);
    // }

    to_play = handle_exceptions(to_play);


    //validity check
    if(to_play <= 41)
    {
      //tell brightsign to play
      binary.write(to_play);

      //tell esp to play servo file
      Serial1.print("<"+data+">");

      //wait for bright_sign
      binary.wait_for_audio();

      //Tell servo we are done
      digitalWrite(SERVO_ESP, LOW);
      digitalWrite(MOUTH_ESP, LOW);
    }
    newData = false;
    Serial.print("<1>");
  }

}
