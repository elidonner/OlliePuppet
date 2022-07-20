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
#define TO_ARDUINO 2
#define MOUTH 35

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

/* -------------------------------------------------------------------------- */

void setup()
{
  // General
  Serial.begin(115200);
  delay(3000);
  pinMode(TO_ARDUINO, OUTPUT);
  digitalWrite(TO_ARDUINO, LOW);

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
  if (SerialPort.available() > 0)
  {
    String file_name = Serial.readStringUntil('\n');
    int num = file_name.toInt();
    file_name = "/" + file_name + ".txt";
    digitalWrite(TO_ARDUINO, HIGH);
    digitalWrite(MOUTH, HIGH);
    readFile(SD, file_name.c_str());
    digitalWrite(TO_ARDUINO, LOW);
    digitalWrite(MOUTH, LOW);
  }
  
}