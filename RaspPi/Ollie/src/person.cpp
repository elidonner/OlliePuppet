#include "person.hpp"

Person::Person(int _id)
    : label(_id)
{
        time = std::chrono::steady_clock::now();
}

void Person::heard(unsigned int audio_flag)
{
        heard_audio |= audio_flag;
}
