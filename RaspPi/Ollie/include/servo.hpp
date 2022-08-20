#ifndef SERVO_HPP_INCLUDED
#define SERVO_HPP_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pigpio.h>

class Servo
{

public:

    /**
    * Constructor
    */
    Servo(int pin,
            int limit,
            int min_width,
            int max_width,
            int pulseWidth);


    /**
    * Move servo to given angle
    */
    void moveToAngle(int angle);

    /**
    * pause the servo
    */
    void pause();

    /**
    * kill servo
    */
    void kill();

protected:

private:
    int _pin;
    int _limit;
    int _min_width;
    int _max_width;
    int _pulseWidth;


    /**
    * convert an angle to pulseWidth angle
    */
    int convertAngle(int angle);
};


#endif // SERVO_HPP_INCLUDED
