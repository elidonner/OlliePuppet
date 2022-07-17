#ifndef PERSON_HPP_INCLUDED
#define PERSON_HPP_INCLUDED

#include "audio.hpp"

class Person
{

public:
    int time;

    /**
    * Constructor
    */
    Person(int _id, Audio &audio);

    /**
     * Destructor
     */
    ~Person();

public:
    chrono::steady_clock::time_point time;

protected:

private:
    Audio &audio;
    unsigned int heard_audio = 0;

};


#endif // PERSON_HPP_INCLUDED
