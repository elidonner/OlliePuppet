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
#include <HardwareSerial.h>

/**
 * Pin Definitions
 */
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5
#define LED 2

/**
 * Miscellaneous Definitions
 */
// ONLY HAVE ONE OF BELOW UNCOMMENTED, OR BOTH COMMENTED FOR PLAY MODE
#define RECORD
// #define CALIBRATE

/**
 * Constants
 */

/**
 * Global Variables
 */
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
// espServo mouthServo(25, 0, 1.23, 1.45)

#ifdef RECORD
SPIClass spi = SPIClass(VSPI);
bool recording = false;
String file_name = "/none.txt";
File dataFile;
String dataString;
#endif // RECORD

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

void handle_files()
{
#ifdef RECORD
  recvBytesWithStartEndMarkers();

  if (newData == true)
  {
    file_name = "";
    for (uint8_t n = 0; n < numReceived; n++)
    {
      file_name += String(((char)receivedBytes[n]));
    }
    Serial.println(file_name);

    int num = file_name.toInt();
    if (num == 50)
    {
      recording = false;
      dataFile.close();
      digitalWrite(LED, LOW);
    }
    else
    {
      file_name = "/" + file_name + ".txt";
      recording = true;
      dataFile = SD.open(file_name, FILE_WRITE);
      digitalWrite(LED, HIGH);
    }

    newData = false;
  }
#endif
}

/**
 * Get PS3 Notifications
 */
void notify()
{
  handle_files();

  int leftNeckThrottle = (Ps3.data.analog.stick.ly);  // Left stick  - y axis - throttle control
  int rightNeckThrottle = (Ps3.data.analog.stick.ry); // Left stick  - y axis - throttle control
  int leftShoulder = (Ps3.data.analog.button.l1);     // left shoulder button
  int rightShoulder = (Ps3.data.analog.button.r1);    // right shoulder button

  // Read ps3 stick and append to data string
  int leftNeck = map(leftNeckThrottle, -128, 128, 0, 100);
  int rightNeck = map(rightNeckThrottle, 128, -128, 0, 100);
  int leftEar = map(leftShoulder, 0, 255, 0, 100);
  int rightEar = map(rightShoulder, 0, 255, 100, 0);

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
  leftEarServo.sendServo(leftEar);
  rightEarServo.sendServo(rightEar);

#endif // CALIBRATE
  delay(15);

// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.
#ifdef RECORD
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  if (recording)
  {
    dataString = "";
    dataString += String(leftNeck) + ",";
    dataString += String(rightNeck) + ",";
    dataString += String(leftEar) + ",";
    dataString += String(rightEar);
    // if the file is available, write to it:
    if (dataFile)
    {
      dataFile.println(dataString);
      // print to the serial port too:
      Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else
    {
      Serial.println("error opening servopos.txt");
    }
  }

#endif
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
  delay(3000);

#ifdef RECORD
  // pin to communciate with arduino
  pinMode(LED, OUTPUT);
  SerialPort.begin(115200, SERIAL_8N1, 9, 10);

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