// Example 6 - Receiving binary data
#include <HardwareSerial.h>

#define MOUTH 35
#define TO_ARDUINO 2
HardwareSerial SerialPort(1); // use UART1

void setup()
{
    SerialPort.begin(115200, SERIAL_8N1, 9, 10);
    pinMode(MOUTH, OUTPUT);
    pinMode(TO_ARDUINO, OUTPUT);
    digitalWrite(TO_ARDUINO, LOW);
}

void loop() {
    if(SerialPort.available()>0)
    {
      String data = Serial.readStringUntil('\n');

      int num = data.toInt();

      digitalWrite(TO_ARDUINO, HIGH);

      //play file
      digitalWrite(MOUTH, HIGH);
      delay(5000);
      digitalWrite(MOUTH, LOW);
      digitalWrite(TO_ARDUINO, LOW);
    }
}