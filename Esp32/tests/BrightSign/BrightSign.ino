// Example 6 - Receiving binary data
#include <HardwareSerial.h>
#include "BrightSign.h"

// Instantiate a BrightSign object and Attach to Serial1
HardwareSerial SerialPort(1); // use UART1
BrightSign BS(SerialPort);

// const uint8_t numBytes = 32;
// uint8_t receivedBytes[numBytes];
// uint8_t numReceived = 0;

// bool newData = false;


// int LED = 15;

// void setup()
// {
//     SerialPort.begin(115200, SERIAL_8N1, 9, 10);
//     Serial.begin(115200);
//     pinMode(15, INPUT);
// }

// void loop()
// {
//     static int oldState = 1;
//     recvBytesWithStartEndMarkers();
//     showNewData();
//     if (digitalRead(LED) != oldState)
//     {
//         SerialPort.write("<Hello Pi>");
//         oldState = oldState == 1 ? 0 : 1;
//     }
// }

void setup()
{
    // SerialPort.begin(115200, SERIAL_8N1, 9, 10);

    BS.debug(); // Enable debug msg over Serial

    Serial.begin(115200);
    delay(3000);

    Serial.println("Waiting BrightSign Power-Up");
    while (!BS.online())
        BS.update();

    Serial.println("BrightSign Powered-Up");

    BS.volume(100); // Set Volume to 20%
    BS.play("/audio/Recording.m4a");
}

void loop()
{
    BS.update(); // Update the BrightSign instance
    if (BS.media_ended())
    {
        BS.play("/audio/Recording(4).m4a");
        Serial.println("playing recording(4).m4a");
    }
}

// void recvBytesWithStartEndMarkers()
// {
//     static bool recvInProgress = false;
//     static uint8_t ndx = 0;
//     uint8_t startMarker = 0x3C; // this is the start marker <
//     uint8_t endMarker = 0x3E;   // this is the start marker >
//     uint8_t rb;

//     while (SerialPort.available() > 0 && newData == false)
//     {
//         rb = SerialPort.read();

//         if (recvInProgress == true)
//         {
//             if (rb != endMarker)
//             {
//                 receivedBytes[ndx] = rb;
//                 ndx++;
//                 if (ndx >= numBytes)
//                 {
//                     ndx = numBytes - 1;
//                 }
//             }
//             else
//             {
//                 receivedBytes[ndx] = '\0'; // terminate the string
//                 recvInProgress = false;
//                 numReceived = ndx; // save the number for use when printing
//                 ndx = 0;
//                 newData = true;
//             }
//         }

//         else if (rb == startMarker)
//         {
//             recvInProgress = true;
//         }
//     }
// }

// void showNewData()
// {
//     if (newData == true)
//     {
//         for (uint8_t n = 0; n < numReceived; n++)
//         {
//             Serial.write(receivedBytes[n]);
//         }
//         Serial.println();
//         newData = false;
//     }
// }