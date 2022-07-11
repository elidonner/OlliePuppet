/*
  Servo position playback
  servo-playback.ino
  Plays back servo movements from SD card
  Used with servo-record.ino
  Displays results on Serial Monitor
  DroneBot Workshop 2019
  https://dronebotworkshop.com
*/
 
// Include libraries
 
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
 
// CS pin for SD Card Module
const int chipSelect = 4;
 
// String to hold one line of text
String buffer;
  
// Create a Servo object
Servo myservo;
 
void setup() {
 
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
  Serial.print("Initializing SD card...");
 
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  
  // Attach servo on pin 9 to the servo object
  myservo.attach(9);  
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("servopos.txt");
  
 
  // If the file is available, read it
  if (dataFile) {
    while (dataFile.available()) {
    // Write one line to buffer
    buffer = dataFile.readStringUntil('\n');
    // Print to serial monitor
      Serial.println(buffer);
    // Convert string to integer and position servo
      myservo.write(buffer.toInt());
    delay(15); 
      
    }
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening servopos.txt");
  }
}
 
void loop() {
}