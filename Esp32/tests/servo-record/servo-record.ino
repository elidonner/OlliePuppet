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
#include "espServo.h"

/**
 * Pin Definitions
 */
#define SCK  18
#define MISO  19
#define MOSI  23
#define CS  5

/**
 * Miscellaneous Definitions
 */

/**
 * Constants
 */

/**
 * Global Variables
 */

/**
 * Global Objects
 */
espServo earServo(0, 0, 1.0, 2.0);
SPIClass spi = SPIClass(VSPI);

/* -------------------------------------------------------------------------- */

/**
 * Get PS3 Notifications
 */
void notify()
{
    int throttle = (Ps3.data.analog.stick.ly);       // Left stick  - y axis - throttle control
    int xAxisValue = (Ps3.data.analog.stick.rx);     // left servo movement
    int yAxisValue = (Ps3.data.analog.stick.ry);     // right servo movement
    int leftShoulder = (Ps3.data.analog.button.l1);  // left shoulder button
    int rightShoulder = (Ps3.data.analog.button.r1); // right shoulder button

    // Read ps3 stick and append to data string
    int val = map(throttle, 128, -128, 0, 100);

    // Write to the servo
    // Delay to allow servo to settle in position
    earServo.sendServo(val);
    delay(15);

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    appendFile(SD, "/hello.txt", val);

    // throttle = (throttle >= 0 ? throttle : 0);
    // baseSpeed = throttle/100.0;

    // xAxisValue = map( xAxisValue, 127, -127, -255, 255);
    // yAxisValue = map( yAxisValue, 127, -127, -255, 255);

    // double steeringMag = sqrt(xAxisValue^2 + yAxisValue^2);
    // double steeringX = xAxisValue/steeringMag;
    // double steeringY = xAxisValue/steeringMag;

    // base.driveCartesian(baseSpeed * 1.0, 0.0, 0.0);
}

void appendFile(fs::FS &fs, const char * path, int message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.printf("%d\n", message)){
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void onConnect()
{
    Serial.println("Connected!.");
    deleteFile(SD, "/hello.txt");
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

    //SD Card
    spi.begin(SCK, MISO, MOSI, CS);

    if (!SD.begin(CS,spi,80000000)) {
        Serial.println("Card Mount Failed");
        return;
    } else 
    {
        Serial.println("Card Mounted");
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    } else 
    {
        Serial.println("SD card attached");
    }

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