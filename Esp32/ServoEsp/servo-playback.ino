/*
 * RC Car with ps3 controller
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
String buffer;

void readFile(fs::FS &fs, const char * path){

  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    buffer = file.readStringUntil('\n');
    Serial.println(buffer);
    earServo.sendServo(buffer.toInt());
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
  
  readFile(SD, "/hello.txt");

}
void loop()
{
}