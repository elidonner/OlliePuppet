#line 1 "/Users/ethandonlon/Documents/School/Fifth Year/FWM/OlliePuppet/Esp32/tests/servo-record/espServo.cpp"
#include "espServo.h"

espServo::espServo(int _pin, int _channel, float _min_pulse, float _max_pulse) : pin(_pin), channel(_channel), min_pulse(_min_pulse), max_pulse(_max_pulse)
{
    difference = _max_pulse-_min_pulse;
    ledcSetup(channel, LEDC_FREQ_HZ, LEDC_RES_BITS); // channel, freq, bits

    ledcAttachPin(pin, channel);
}

void espServo::sendServo(int percent)
{
    float pulse_ms = (difference*(float(percent)/100.0))+1.0;
    ledcWrite(channel, (pulse_ms / 1000.0) * LEDC_FREQ_HZ * LEDC_RES);
}