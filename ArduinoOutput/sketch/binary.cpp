#line 1 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/ArduinoMega/BrightBin/binary.cpp"
#include "binary.h"

Binary::Binary(int _pin0, int _pin1, int _pin2, int _pin3, int _pin4, int _pin5, int _status_pin, int _input_pin):
pin0(_pin0),pin1(_pin1),pin2(_pin2),pin3(_pin3),pin4(_pin4),pin5(_pin5), status_pin(_status_pin), input_pin(_input_pin)
{
    pins[0] = pin0;
    pins[1] = pin1;
    pins[2] = pin2;
    pins[3] = pin3;
    pins[4] = pin4;
    pins[5] = pin5;
}

void Binary::setup()
{
    for (int pin : pins)
    {
        pinMode(pin, OUTPUT);
    }
    pinMode(status_pin, OUTPUT);
    clear_pins();
    pinMode(input_pin, INPUT);
    //cycle the bright sign in case
    digitalWrite(status_pin, HIGH);
    delay(100);
    digitalWrite(status_pin, LOW);
}

void Binary::write(int num)
{
    int i = 1;
    for (int pin : pins)
    {
        if(num & i)
        {
            digitalWrite(pin, HIGH);
        }
        else{
        }
        i <<= 1;
    }
    delay(1000);
    digitalWrite(status_pin, HIGH);
    clear_pins();
}

void Binary::wait_for_audio()
{
    if(digitalRead(input_pin)==LOW){
        while(digitalRead(input_pin)==LOW){}
    }
    while(digitalRead(input_pin) == HIGH){}
}

void Binary::clear_pins()
{
    for (int pin : pins)
    {
        digitalWrite(pin, LOW);
    }
    digitalWrite(status_pin, LOW);
}