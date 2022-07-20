/**
 * @file servo_calibration.ino
 * @author Ethan Donlon
 * @brief
 * @version 0.1
 * @date 2022-07-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <espServo.h>

/**** SERVOSTUFF *****/
#define SERVOPIN 26
espServo servo(SERVOPIN, 0);
float servo_pulse;

void setup()
{
  Serial.begin(115200);
}

// Read in + and - characters to set the volume.
void loop()
{
  if (Serial.available() > 0)
  {
    // read a character from serial console
    String data = Serial.readStringUntil('\n');
    Serial.print("data: ");
    Serial.println(data);
    
    // increase
    if (data[0] == '+')
    {
      servo_pulse += (float)(data.length())/100.0;
    }
    // decrease
    else if (data[0] == '-')
    {
      servo_pulse -= (float)data.length()/100.0;
    }
    else
    {
        servo_pulse = data.toFloat();
    }

    Serial.println(servo_pulse);

    servo.sendPulse(servo_pulse);
  }
}