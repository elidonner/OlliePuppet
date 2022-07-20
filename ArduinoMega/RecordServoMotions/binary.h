#ifndef BINARY_H
#define BINARY_H

#include <Arduino.h>

class Binary
{
public:
    Binary(int _pin0, int _pin1, int _pin2, int _pin3, int _pin4, int _pin5, int _status_pin, int _input_pin);

    void setup();

    void write(int num);

    void wait_for_audio();

    void clear_pins();

private:
    int pin0, pin1, pin2, pin3, pin4, pin5, status_pin, input_pin;
    int pins[6];

};

#endif //BINARY_H