#ifndef ESPSERVO_H
#define ESPSERVO_H

#define LEDC_RES_BITS 12        // cannot be higher than 14 bits
#define LEDC_RES ((1<<LEDC_RES_BITS)-1)
#define LEDC_FREQ_HZ 60

class espServo
{
public:
    espServo(int _pin, int _channel, float _min_pulse, float _max_pulse);

    void sendServo(int percent);

private:
    int pin, channel;
    float min_pulse, max_pulse;
    float difference;

}

#endif //ESPSERVO_H