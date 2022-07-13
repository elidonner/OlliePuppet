// Example 6 - Receiving binary data
#include <HardwareSerial.h>

const uint8_t numBytes = 32;
uint8_t receivedBytes[numBytes];
uint8_t numReceived = 0;
int initialState;

bool newData = false;

HardwareSerial SerialPort(1); // use UART1

int LED = 15;

void setup()
{
    SerialPort.begin(115200, SERIAL_8N1, 9, 10);
    Serial.begin(115200);
    pinMode(LED, INPUT);
    initialState = digitalRead(LED);
}

void recvBytesWithStartEndMarkers() {
    static bool recvInProgress = false;
    static uint8_t ndx = 0;
    uint8_t startMarker = 0x02;//0x3C; //this is the start marker <
    uint8_t endMarker = 0x03;//0x3E; //this is the start marker >
    uint8_t rb;
   
    while (SerialPort.available() > 0 && newData == false) {
        rb = SerialPort.read();
        Serial.println(rb);

        if (recvInProgress == true) {
            if (rb != endMarker) {
                receivedBytes[ndx] = rb;
                ndx++;
                if (ndx >= numBytes) {
                    ndx = numBytes - 1;
                }
            }
            else {
                receivedBytes[ndx] = rb;
                ndx++;
                receivedBytes[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                numReceived = ndx;  // save the number for use when printing
                ndx = 0;
                newData = true;
            }
        }

        else if (rb == startMarker) {
            receivedBytes[ndx] = rb;
            ndx++;
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        for (uint8_t n = 0; n < numReceived; n++) {
            Serial.print(receivedBytes[n]);
        }
        Serial.println();
        newData = false;
    }
}

void loop() {
    recvBytesWithStartEndMarkers();
    showNewData();
    if(digitalRead(LED) != initialState){
        Serial.println("button pressed");
        while(digitalRead(LED) != initialState){};
        Serial.println("button released");
        SerialPort.print("PLAY /audio/Recording.mp4\r");
    }
}