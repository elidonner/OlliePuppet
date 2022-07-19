// Example 6 - Receiving binary data
#include <HardwareSerial.h>
#include "BrightSign.h"

// Instantiate a BrightSign object and Attach to Serial1
HardwareSerial SerialPort(1); // use UART1
BrightSign BS(SerialPort);

HardwareSerial Serial2(2); 

void setup()
{
    Serial2.begin(115200, SERIAL_8N1, 21, 22);

    BS.debug(); // Enable debug msg over Serial

    Serial.begin(115200);

    Serial.println("Waiting BrightSign Power-Up");
    // while (!BS.online())
    //     BS.update();

    Serial.println("BrightSign Powered-Up");

    // Set Volume to 20%
    BS.volume(100);
    BS.play("AUDIO/RECORDING.MP4");
}

void loop()
{
    // Update the BrightSign instance
    BS.update();
    if (BS.media_ended())
    {
        BS.play("AUDIO/RECORDING.MP4");
        Serial2.print("<Audio finished>");
    }
}