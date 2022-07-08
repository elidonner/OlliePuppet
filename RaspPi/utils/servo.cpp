#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pigpio.h>
#include "servo.hpp"

Servo::Servo(int pin,
            int limit,
            int min_width,
            int max_width,
            int pulseWidth)
    : _pin(pin),
        _limit(limit),
      _min_width(min_width),
      _max_width(max_width),
      _pulseWidth(pulseWidth)
{}

void Servo::moveToAngle(int angle)
{

    //Convert the angle to a pulseWidth target
    angle = convertAngle(angle);

    //comand the pulse width
    gpioServo(_pin, angle);

}

void Servo::kill()
{
    //tidy up the gpio
    gpioServo(_pin, 0);
    gpioTerminate();
}

int Servo::convertAngle(int angle)
{
    float angle_convert;
    int pulseWidth;

    if(angle<0)
    {
        angle = 0;
    }
    else if (angle > 270)
    {
        angle = 270;
    }

    angle_convert = angle/270.0;

    pulseWidth = angle_convert*(_max_width-_min_width)+_min_width;

    return pulseWidth;
}
