/**
 * @brief OUTLINE OF INTERACTIONS
 *
 * AUDIO FILES ORGANIZED INTO 2 SCOPES
 * things to say when people are there and cases that are triggered by specific events
 *
 * specific events:
 * 1 person
 * 2 people
 * 3 or more people
 *
 * determine a specific event, and then wait ten seconds before pulling from the audio, redetermine the specific event, play the audio, and wait ten seconds again before pulling from the audio.
 * If at anytime the specific event changes, reset the timer
 * exception: single person exiting, say exiting bit right away
 *
 * create a pool of stuff to say depending on the case, could be a list of numbers.
 * each time something is said, remove it from the list
 * ideally keep a list of things that each person has heard and don't repopulate the item to the list, until that person is gone
 * each time ollie speaks, assign that track to the newest person
 * newest person should be the person at the end of list of people
 *
 * could use uint_32
 *
 * store when person was initiated
 */

#ifndef INTERACTIONS_HPP_INCLUDED
#define INTERACTIONS_HPP_INCLUDED

#include <iostream>
#include "audio.hpp"

// interaction class object
class Interactions
{
public:
    Interactions();

    void start_main_timer();
    void update_people(Std::vector<PersonInfo> &person_info, Std::vector<Person> &people, cv::Mat &frame);

public:
    bool valid_case, first_time;
    int current_case = 0;
    chrono::steady_clock::time_point main_timer = 0;

private:

    bool in_vector(int key, std::vector<int> v);
    void is_case_valid(int mode);
    void find_case(int num_people);

private:
    Audio & audio;
    Std::vector<int> prev_round;
}

#endif //INTERACTIONS_HPP_INCLUDED