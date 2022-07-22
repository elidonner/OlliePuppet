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

const uint8_t numBytes = 32;
uint8_t receivedBytes[numBytes];
uint8_t numReceived = 0;
int initialState;

bool newData = false;

Binary binary(52, 50, 48, 46, 44, 42, 40, 38);

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

void record_file(int i)
{
    // which file is playing
    Serial.print("Playing: ");
    Serial.println(i);
    // tell ESP
    Serial1.print("<" + String(i) + ">");
    // then play audio file
    binary.write(i);
    binary.wait_for_audio();
    // once file is over
    Serial1.print("<50>");
    Serial.println("file_done");
    delay(2000); // give user time to reset
}

void setup()
{
    // Start the USB serial port for talking to the rasberry pi
    Serial.begin(115200);

    // for comm with ESP
    Serial2.end();
    // Start the serial 1 for talking to esp
    Serial1.begin(115200);

    // start binary port
    binary.setup();
}

void loop()
{
    recvBytesWithStartEndMarkers();

    if (newData == true)
    {
        String data = "";
        for (uint8_t n = 0; n < numReceived; n++)
        {
            data = data + String(((char)receivedBytes[n]));
        }
        Serial.println(data);

        Serial.println("starting recording");
        if (data.equals("all"))
        {
            for (int i = 0; i < 42; i++)
            {
                record_file(i);
            }
        }
        else
        {
            int num = data.toInt();
            record_file(num);
        }

        Serial.println("all done");

        newData = false;
    }
}