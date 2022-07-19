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

#include "binary.h"

Binary binary(52, 50, 48, 46, 44, 42, 40, 38);

void setup()
{
  // Start the USB serial port for talking to the rasberry pi
  Serial.begin(115200);

  //start binary port
  binary.setup();
}

void loop()
{
  if(Serial.available()>0)
  {
    // String data = Serial.readStringUntil('\n');

    // int to_play;
    // Serial.print("data: ");

    // to_play = data.toInt();

    // binary.write(to_play);

    // binary.wait_for_audio();

    // Serial.println("file_done");

    Serial.println("starting playback");
    for(int i = 0; i<42; i++){
      Serial.print("Playing: ");
      Serial.println(i);
      binary.write(i);
      binary.wait_for_audio();
      Serial.println("file_done");
    }
  }
}