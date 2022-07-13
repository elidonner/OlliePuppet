# 1 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino"
/*

 * RC Car with ps3 controller

 * https://github.com/jvpernis/esp32-ps3/blob/master/examples/Ps3Demo/Ps3Demo.ino

 *

 * @author Ethan Donlon

 */
# 8 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino"
/**

 * Includes

 */
# 11 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino"
# 12 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino" 2
# 13 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino" 2
# 14 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino" 2
# 15 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino" 2
# 16 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino" 2

/**

 * Pin Definitions

 */



/**

 * Miscellaneous Definitions

 */
# 28 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino"
// #define RECORD

/**

 * Constants

 */
# 34 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino"
/**

 * Global Variables

 */
# 38 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino"
/**

 * Global Objects

 */
# 41 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino"
// espServo leftEarServo(25, 0, 1.33, 2.0);
// espServo rightEarServo(26, 2, 1.33, 2.0);
espServo rightNeck(32, 4, 1.0, 2.3);
espServo leftNeck(33, 6, 1.0, 2.3);
// espServo mouthServo(25, 0, 1.33, 1.80)

SPIClass spi = SPIClass(3 /*SPI bus normally attached to pins 5, 18, 19 and 23, but can be matrixed to any pins*/);

/* -------------------------------------------------------------------------- */

/**

 * Get PS3 Notifications

 */
# 54 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino"
void notify()
{
  int leftNeckThrottle = (Ps3.data.analog.stick.ly); // Left stick  - y axis - throttle control
  int rightNeckThrottle = (Ps3.data.analog.stick.ry); // Left stick  - y axis - throttle control
  int leftShoulder = (Ps3.data.analog.button.l1); // left shoulder button
  int rightShoulder = (Ps3.data.analog.button.r1); // right shoulder button

  // Read ps3 stick and append to data string
  int leftVal = map(leftNeckThrottle, 128, -128, 0, 100);
  int rightVal = map(rightNeckThrottle, -128, 128, 0, 100);

  // Write to the servo
  // Delay to allow servo to settle in position
  leftNeck.sendServo(leftVal);
  rightNeck.sendServo(rightVal);
  delay(15);

// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.




  // throttle = (throttle >= 0 ? throttle : 0);
  // baseSpeed = throttle/100.0;

  // xAxisValue = map( xAxisValue, 127, -127, -255, 255);
  // yAxisValue = map( yAxisValue, 127, -127, -255, 255);

  // double steeringMag = sqrt(xAxisValue^2 + yAxisValue^2);
  // double steeringX = xAxisValue/steeringMag;
  // double steeringY = xAxisValue/steeringMag;

  // base.driveCartesian(baseSpeed * 1.0, 0.0, 0.0);
}

void appendFile(fs::FS &fs, const char *path, int message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, "a");
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
# 171 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/servo-record.ino"
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
