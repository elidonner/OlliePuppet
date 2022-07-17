#ifndef PERSON_HPP_INCLUDED
#define PERSON_HPP_INCLUDED

#include <chrono>

class Person
{

public:
    /**
    * Constructor
    */
    Person(int _id);

    void heard(unsigned int audio_flag);

public:
    std::chrono::steady_clock::time_point time;
    unsigned int heard_audio = 0;
    int label;

protected:

private:


};


#endif // PERSON_HPP_INCLUDED
