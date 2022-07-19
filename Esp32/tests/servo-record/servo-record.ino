/*
 * RC Car with ps3 controller
 * https://github.com/jvpernis/esp32-ps3/blob/master/examples/Ps3Demo/Ps3Demo.ino
 *
 * @author Ethan Donlon
 */

/**
 * Includes
 */
#include <Ps3Controller.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <espServo.h>

/**
 * Pin Definitions
 */
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5

/**
 * Miscellaneous Definitions
 */
// ONLY HAVE ONE OF BELOW UNCOMMENTED, OR BOTH COMMENTED FOR PLAY MODE
//  #define RECORD
// #define CALIBRATE

/**
 * Constants
 */

/**
 * Global Variables
 */

/**
 * Global Objects
 */
// espServo leftEarServo(25, 0, 1.33, 2.0);
// espServo rightEarServo(26, 2, 1.33, 2.0);
espServo leftNeckServo(33, 4, 1.4, 1.6);
espServo rightNeckServo(32, 6, 1.4, 1.6);
// espServo mouthServo(25, 0, 1.11, 1.40)

#ifdef RECORD
SPIClass spi = SPIClass(VSPI);
#endif // RECORD

/* -------------------------------------------------------------------------- */

/**
 * Get PS3 Notifications
 */
void notify()
{
  int leftNeckThrottle = (Ps3.data.analog.stick.ly);  // Left stick  - y axis - throttle control
  int rightNeckThrottle = (Ps3.data.analog.stick.ry); // Left stick  - y axis - throttle control
  int leftShoulder = (Ps3.data.analog.button.l1);     // left shoulder button
  int rightShoulder = (Ps3.data.analog.button.r1);    // right shoulder button

  // Read ps3 stick and append to data string
  int leftNeck = map(leftNeckThrottle, -128, 128, 0, 100);
  int rightNeck = map(rightNeckThrottle, 128, -128, 0, 100);
  int leftEar = map(leftShoulder, 0, 255, 0, 100);
  int rightEar = map(rightShoulder, 0, 255, 0, 100);

#ifdef CALIBRATE
  float leftFloatNeck = ((float)leftNeck / 50.0) + 0.5;
  float rightFloatNeck = ((float)rightNeck / 50.0) + 0.5;

  Serial.print("LeftNeck: ");
  Serial.print(leftFloatNeck);
  Serial.print("\tRightNeck: ");
  Serial.print(rightFloatNeck);

  // right: 1.1 , 1.5 , 1.9
  // left  1.9, 1.5, 1.1

  // Write to the servo
  // Delay to allow servo to settle in position
  leftNeckServo.sendPulse(leftFloatNeck);
  rightNeckServo.sendPulse(rightFloatNeck);

#else
  leftNeckServo.sendServo(leftNeck);
  rightNeckServo.sendServo(rightNeck);
  // leftEarServo.sendServo(leftEar);
  // rightEarServo.sendServo(rightEar);
#endif // CALIBRATE
  delay(15);


// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.
#ifdef RECORD
  appendFile(SD, "/hello.txt", val);
#endif
}

#ifdef RECORD

void appendFile(fs::FS &fs, const char *path, int message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.printf("%d\n", message))
  {
    Serial.println("Message appended");
  }
  else
  {
    Serial.println("Append failed");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path))
  {
    Serial.println("File deleted");
  }
  else
  {
    Serial.println("Delete failed");
  }
}
#endif // RECORD

void onConnect()
{
  Serial.println("Connected!.");
#ifdef RECORD
  deleteFile(SD, "/hello.txt");
#endif
}

void onDisConnect()
{
  Serial.println("Disconnected!.");
}

/* -------------------------------------------------------------------------- */

void setup()
{
  // General
  Serial.begin(115200);
  delay(3000);

#ifdef RECORD
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
#endif

  // PS3 Controller
  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.attachOnDisconnect(onDisConnect);
  Ps3.begin("00:1b:fb:94:e6:94");
  Serial.println("Ready.");
}

void loop()
{
  if (!Ps3.isConnected())
  {
  }
  else
  {
  }
}