#ifndef AUDIO_HPP_INCLUDED
#define AUDIO_HPP_INCLUDED

#include <vector>
#include <random>
#include <bitset>
#include "person.hpp"
#include "serial.hpp"

// interaction class object
class Audio
{
public:
    Audio();

    void handle(int current_case, std::chrono::steady_clock::time_point main_timer, bool valid_case, bool & first_time, std::vector<Person> & people);
    void audio_done();
public:
    /**
     * @brief Pool is 32 bit int where each bit is a flag for if the audio file is in the pool
     *
     * 0-18 are general
     *      {
     *          0-5 are beginning (before 10 seconds on new visitor)
     *          6-13 are middle (between 10 and 2 min)
     *          14-18 are end (after 2 min)
     *      }
     * 19-26 are single visitor
     *      {
     *          19-22 are single visitor
     *          23-26 are single visitor exiting
     *      }
     * 27-28 are two visitors
     * 29-31 are >= 3 visitors
     *
     *
     */
    unsigned int pool, played_pool;
    bool first_special, audio_playing = false;

    int wait_time;

private:
    int rand_int(int min, int max);
    void clear_audio_heard(std::vector<Person> & people);
    void determine_pool(int mode, std::chrono::steady_clock::time_point main_timer, bool valid_case, bool & first_time, std::vector<Person> & people);
    int count_set_bits(unsigned int v);
    int countTrailingZero(int x);
    unsigned int nthset(uint32_t x, int n);
    unsigned int pull_from_pool();
    unsigned int bit_ones(int n, int l, int r);
    void play_audio(std::vector<Person> & people, bool & first_time);

private:
    std::mt19937 gen; //random number generator
    std::chrono::steady_clock::time_point audio_timer;
};

#endif //AUDIO_HPP_INCLUDED
