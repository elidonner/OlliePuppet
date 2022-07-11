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
#include "espServo.h"

/**
 * Pin DefinitionsP
 */
const int chipSelect = 4;
// Analog pin for potentiometer
int analogPin = 0;
// Integer to hold potentiometer value
int val = 0;

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
    
    Serial.println(throttle);
    // make a string for assembling the data to log:
    String dataString = "";

    // Read pot value and append to the string
    // Map to range of 0-180 for servo
    val = map(throttle, 128, -128, 0, 100);

    dataString += String(val);

    // Write to the servo
    // Delay to allow servo to settle in position
    earServo.sendServo(val);
    delay(15);

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("servopos.txt", FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile)
    {
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else
    {
        Serial.println("error opening servopos.txt");
    }

    // throttle = (throttle >= 0 ? throttle : 0);
    // baseSpeed = throttle/100.0;

    // xAxisValue = map( xAxisValue, 127, -127, -255, 255);
    // yAxisValue = map( yAxisValue, 127, -127, -255, 255);

    // double steeringMag = sqrt(xAxisValue^2 + yAxisValue^2);
    // double steeringX = xAxisValue/steeringMag;
    // double steeringY = xAxisValue/steeringMag;

    // base.driveCartesian(baseSpeed * 1.0, 0.0, 0.0);
}

void onConnect()
{
    Serial.println("Connected!.");
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
    while (!Serial)
    {
    };

    // PS3 Controller
    Ps3.attach(notify);
    Ps3.attachOnConnect(onConnect);
    Ps3.attachOnDisconnect(onDisConnect);
    Ps3.begin("00:1b:fb:94:e6:94");
    Serial.println("Ready.");

    Serial.print("Initializing SD card...");

    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect))
    {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        while (1)
            ;
    }
    Serial.println("card initialized.");
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