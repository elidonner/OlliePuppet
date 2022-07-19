/**
 * @file audio.cpp
 * @author Ethan Donlon
 * @brief makes use of a 32 bit int since we have 32 audio files to determine pool of audio files to play from
 *
 */
#include "audio.hpp"

Audio::Audio(Serial &_serial): gen( (std::random_device())() ), ser(_serial) //seed the random number generator
{
    wait_time = 1;
//    std::cout<<"wait time: "<<wait_time<<std::endl;

    // set played_pool to all zeros
    played_pool = 0;

    //set audio timer at beginning
    audio_timer = std::chrono::steady_clock::now();
}

int Audio::rand_int(int min, int max)
{
    //get random int inclusive
    std::uniform_int_distribution<int> rand_num(min, max);

    return rand_num(gen);
}

void Audio::clear_audio_heard(std::vector<Person> & people)
{
    //clear the audio heard by all the people
    for (Person person : people)
    {
        person.heard_audio = 0;
    }
    played_pool = 0;

}

unsigned int Audio::bit_ones(int n, int l, int r)
{
    //calculate number range having set bits in the range from l to r and all other bits 0
    int range = (((1 << (l - 1)) - 1) ^ ((1 << (r)) - 1));

    return (n | range);
}

void Audio::determine_pool(int mode, std::chrono::steady_clock::time_point main_timer, bool valid_case, bool & first_time, std::vector<Person> & people)
{
    int time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - main_timer).count();
    pool = 0;
    //if we have said one of the group sayings, go ahead and add general sayings to the audio pool
    if(!first_time)
    {
        if(time < 1)
        {
            // if a second hasn't passed, do nothing, to avoid noise
            return;
        }
        else if (time < 10)
        {
            //in the 0-10 sec
            pool |= bit_ones(0, 1, 5);
        }
        else if(time < 120){
            //in the 10-2 min
            pool |= bit_ones(0,6, 14);
        }
        else
        {
            //> than 2 min
            pool |= bit_ones(0,6, 17);
        }
    }

    // if it is a valid case, pull the given audio files
    if (valid_case)
    {
        switch (mode){
            case 1:
                //files 19-26
                pool |= bit_ones(0, 19, 26);
                break;
            case 2:
                //files 26-28
                pool |= bit_ones(0, 26, 28);
                break;
            case 3:
                //files 29-31
                pool |= bit_ones(0, 29, 31);
                break;
        }
    }
    else
    {
        if(pool == 0)
        {
            return;
        }
    }

    //remove any audio files that have been played
    pool &= ~(played_pool);
    //if all the audio files have been played, clear the audio files people have heard
    if(pool == 0)
    {
        clear_audio_heard(people);
        //redetermine the pool
        determine_pool(mode, main_timer, valid_case, first_time, people);
    }
}

int Audio::countTrailingZero(int x)
{
     // Map a bit value mod 37 to its position
     static const int lookup[] = {32, 0, 1,
     26, 2, 23, 27, 0, 3, 16, 24, 30, 28, 11,
     0, 13, 4, 7, 17, 0, 25, 22, 31, 15, 29,
     10, 12, 6, 0, 21, 14, 9, 5, 20, 8, 19,
     18};

     // Only difference between (x and -x) is
     // the value of signed magnitude(leftmostbit)
     // negative numbers signed bit is 1
     return lookup[(-x & x) % 37];
}

int Audio::count_set_bits(unsigned int v)
{
    unsigned int c; // c accumulates the total bits set in v
    for (c = 0; v; c++)
    {
    v &= v - 1; // clear the least significant bit set
    }
    return c;
}

unsigned int Audio::nthset(uint32_t x, int n) {
    int i = 1;
    int c = 0;
    while (c < n)
    {
        if(i & x)
        {
            c++;
        }
        i <<= 1;
    }
    return i>>1;
}

unsigned int Audio::pull_from_pool()
{
    //get a random audio file
    //get number of bits set to 1
    int n, rand_num;
    n = count_set_bits(pool);
//    std::cout<<"set bits "<<n<<std::endl;

    //select random number in range of bits set to 1
    rand_num = rand_int(1,n);

    //get the nth bit set to 1
    return (nthset(pool, rand_num));
}

void Audio::play_audio(std::vector<Person> & people, bool & first_time)
{
    //pull a random audio file
    unsigned int to_play = pull_from_pool();

    //assign the audio that was played to the newest person
    people.back().heard(to_play);

    //add it to the played pool
    played_pool |= to_play;

    //if we play a special file, remove the first time flag
    if(first_time)
    {
        first_time = false;
    }

    //play the audio file
    int int_to_send = countTrailingZero(to_play);

    std::string to_send = std::to_string(int_to_send)+"\n";

    audio_playing = true;

    ser.write((char*)to_send.c_str());
    fflush(stdout);
//    std::cout<<"file to play: "<<to_send<<std::endl;

}

void Audio::audio_done()
{
    //reset the wait_time flag
    wait_time = rand_int(8, 30);
//    std::cout<<"wait_time: "<<wait_time<<std::endl;
    audio_playing = false;
    audio_timer = std::chrono::steady_clock::now();
}

void Audio::handle(int current_case, std::chrono::steady_clock::time_point main_timer, bool valid_case, bool & first_time, std::vector<Person> & people)
{
//    std::cout<<"current_case: "<<current_case<<std::endl;
    //if case is 0 people, do nothing
    if(current_case == 0 || audio_playing)
    {
        return;
    }

    //check if time since last audio file is ready to play
    int diff = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - audio_timer).count();
//    std::cout<<"diff: "<<diff<<std::endl;
    if (diff >= wait_time)
    {
        determine_pool(current_case, main_timer, valid_case, first_time, people);
//        std::cout<<"pool:  "<< std::bitset<32>(pool)<<std::endl;
        if(pool!=0)
        {
            play_audio(people, first_time);
        }
    }
}
