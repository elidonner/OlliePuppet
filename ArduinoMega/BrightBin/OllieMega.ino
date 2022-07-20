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

#define FROM_ESP 18

Binary binary(52, 50, 48, 46, 44, 42, 40, 38);

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
    else if (rand_num == 1) {return 39;}
    else if (rand_num == 2) {return 41;}
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
  pinMode(FROM_ESP, INPUT);

  //start binary port
  binary.setup();
}

void loop()
{
  if(Serial.available()>0)
  {
    String data = Serial.readStringUntil('\n');

    int to_play;
    // Serial.print("data: ");

    to_play = data.toInt();

    to_play = handle_exceptions(to_play);

    binary.write(to_play);

    //tell esp to play servo file
    Serial1.println(to_play);

    binary.wait_for_audio();

    //wait for servo file
    while( digitalRead(FROM_ESP) == HIGH ){}

    //tell esp and RaspPi that audio is finished)
    Serial.print("<1>");

    // Serial.println("starting playback");
    // for(int i = 0; i<42; i++){
    //   Serial.print("Playing: ");
    //   Serial.println(i);
    //   binary.write(i);
    //   binary.wait_for_audio();
    //   Serial.println("file_done");
    // }
  }
}