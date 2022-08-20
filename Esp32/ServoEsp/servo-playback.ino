/*
 * Servo playback
 * https://github.com/jvpernis/esp32-ps3/blob/master/examples/Ps3Demo/Ps3Demo.ino
 *
 * @author Ethan Donlon
 */

/**
 * Includes
 */
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <espServo.h>
#include <HardwareSerial.h>

/**
 * Pin Definitions
 */
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5
#define FROM_ARDUINO 15

/**
 * Miscellaneous Definitions
 */

/**
 * Constants
 */

/**
 * Global Variables
 */
String file_name;
HardwareSerial SerialPort(1); // use UART1
const uint8_t numBytes = 32;
uint8_t receivedBytes[numBytes];
uint8_t numReceived = 0;
int initialState;

bool newData = false;

/**
 * Global Objects
 */
espServo leftEarServo(25, 0, 1.0, 1.6);
espServo rightEarServo(26, 2, 1.0, 1.6); // inverted though
espServo leftNeckServo(33, 4, 1.5, 1.9); // inverted though
espServo rightNeckServo(32, 6, 1.1, 1.5);

SPIClass spi = SPIClass(VSPI);
String buffer;

void readFile(fs::FS &fs, const char *path)
{

  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available())
  {
    if (digitalRead(FROM_ARDUINO) == LOW)
    {
      break;
    }
    
    int leftNeck, rightNeck, leftEar, rightEar;
    buffer = file.readStringUntil(',');
    leftNeck = buffer.toInt();
    buffer = file.readStringUntil(',');
    rightNeck = buffer.toInt();
    buffer = file.readStringUntil(',');
    leftEar = buffer.toInt();
    buffer = file.readStringUntil('\n');
    rightEar = buffer.toInt();
    Serial.println(buffer);

    leftNeckServo.sendServo(leftNeck);
    rightNeckServo.sendServo(rightNeck);
    leftEarServo.sendServo(leftEar);
    rightEarServo.sendServo(rightEar);

    delay(15);
  }
  file.close();
}

/* -------------------------------------------------------------------------- */

void recvBytesWithStartEndMarkers()
{
  static bool recvInProgress = false;
  static uint8_t ndx = 0;
  uint8_t startMarker = 0x3C; // this is the start marker <
  uint8_t endMarker = 0x3E;   // this is the start marker >
  uint8_t rb;

  while (SerialPort.available() > 0 && newData == false)
  {
    rb = SerialPort.read();
    Serial.println(rb);

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

/* -------------------------------------------------------------------------- */

void setup()
{
  // General
  Serial.begin(115200);
  SerialPort.begin(115200, SERIAL_8N1, 9, 10);
  delay(3000);
  pinMode(FROM_ARDUINO, INPUT);

  // SD Card
  spi.begin(SCK, MISO, MOSI, CS);

  if (!SD.begin(CS, spi, 80000000))
  {
    Serial.println("Card Mount Failed");
    return;
  }
  else
  {
    Serial.println("Card Mounted");
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }
  else
  {
    Serial.println("SD card attached");
  }
}
void loop()
{
  recvBytesWithStartEndMarkers();

  if (newData == true)
  {
    file_name = "";
    for (uint8_t n = 0; n < numReceived; n++)
    {
      file_name += String(((char)receivedBytes[n]));
    }
    file_name = "/" + file_name + ".txt";
    readFile(SD, file_name.c_str());
    
    //reset the servos
    leftNeckServo.sendServo(50);
    rightNeckServo.sendServo(50);
    leftEarServo.sendServo(0);
    rightEarServo.sendServo(100);
    //tell arduino we are finished
    newData = false;
  }

}