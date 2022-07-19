/**
 * UnoBright.ino
 * @author Ethan Donlon
 *
 * @details
 * This is code for arudino mega which handles communication between two ESP32 picos
 * a raspberry pi, and a BrightSign LS424 digital media player
 *
 * Uses script uploaded to BrightSign from Zarpli: https://github.com/zarpli/BSuControl/
 */

#include "BrightSign.h"

// Instantiate a BrightSign object and Attach to Serial1
BrightSign BS(Serial1);

void setup()
{
  // Enable debug msg over Serial
  BS.debug();

  // Start the USB serial port for talking to the rasberry pi
  Serial.begin(115200);

  // set up digital pin for telling the mouth servo to talk

  // while(!BS.online()){BS.update();}
  Serial.print("BS online");

  // Set Volume to 50%
  BS.volume(20);
  BS.play("AUDIO/RECORDING (4).M4A");
}

void loop()
{
  BS.update();
  if (BS.media_ended())
  {
    BS.play("AUDIO/RECORDING (4).M4A");
  }
  // wait for a message from the RaspberryPi, saying which audio file to play
  // if (Serial.available() > 0)
  // {
  //   // read the message from the RaspPi
  //   String data = Serial.readStringUntil('\n');

  //   // tell the BrightSign to play this audio filex
  //   //  BS.play(data);

  //   // tell the esp to play this servo file
  //   //  Serial2.print("1");

  //   // Tell the mouth servo to turn on
  //   BS.play("AUDIO/1.WAV");

  //   // wait for the file to end
  //   // while (1)
  //   // {
  //   //   BS.update();
  //   //   if (BS.media_ended())
  //   //   {
  //   //     BS.play("AUDIO/RECORDING (4).M4A");
  //   //   }
  //   // }
  //   // Tell the RaspPi the audio is done
  //   Serial.print("<1>");
  // }
}