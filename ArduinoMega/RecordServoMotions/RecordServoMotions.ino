/**
 * RecordServoMotions.ino
 * @author Ethan Donlon
 *
 * @details
 * This is code for arudino mega to communciate with esp32 to record servo movements
 * BrightSign LS424 digital media player
 *
 */

#include "binary.h"

#define TO_ESP 15

Binary binary(52, 50, 48, 46, 44, 42, 40, 38);

void setup()
{
    // Start the USB serial port for talking to the rasberry pi
    Serial.begin(115200);

    // for comm with ESP
    Serial2.end();
    pinMode(TO_ESP, INPUT);
    // Start the serial 1 for talking to esp
    Serial1.begin(115200);

    // start binary port
    binary.setup();
}

void loop()
{
    if (Serial.available() > 0)
    {
        Serial.println("starting recording");

        for (int i = 0; i < 42; i++)
        {
            //which file is playing
            Serial.print("Playing: ");
            Serial.println(i);
            //tell ESP
            Serial1.println(i);
            //then play audio file
            binary.write(i);
            binary.wait_for_audio();
            //once file is over
            Serial1.println(50);
            Serial.println("file_done");
            delay(2000); //give user time to reset
        }
        Serial.println("all done");
        while(1);
    }
}