#ifndef PERSON_HPP_INCLUDED
#define PERSON_HPP_INCLUDED

#include <chrono>

class Person
{

public:
    /**
    * Constructor
    */
    Person(int _id, int _centerX);

    void heard(unsigned int audio_flag);
    void update_x(int _centerX);

public:
    std::chrono::steady_clock::time_point time;
    unsigned int heard_audio = 0;
    int label, centerX;

protected:

private:


};


#endif // PERSON_HPP_INCLUDED
