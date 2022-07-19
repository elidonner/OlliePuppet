#include "person.hpp"

Person::Person(int _id, int _centerX)
    : label(_id), centerX(_centerX)
{
        time = std::chrono::steady_clock::now();
}

void Person::update_x(int _centerX)
{
    centerX = _centerX;
}

void Person::heard(unsigned int audio_flag)
{
        heard_audio |= audio_flag;
}
