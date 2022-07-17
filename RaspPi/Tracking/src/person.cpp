#include "person.hpp"
Person::Person(int _id, Audio &_audio)
    : id(_id), audio(_audio)
{
        time = chrono::steady_clock::now();
}

Person::~Person()
{
        // FIXME: make sure this is how destructor works
        audio.pool |= heard_audio;
}

void Person::heard(unsigned int audio_flag)
{
        heard_audio |= audio_flag;
}