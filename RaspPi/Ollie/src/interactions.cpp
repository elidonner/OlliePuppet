#include "interactions.hpp"

Interactions::Interactions(Audio &_audio):audio(_audio)
{
    //initiate the main_timer
    start_main_timer();

    //add modes to mode list
    Mode no_people;
    Mode one_person;
    Mode two_people;
    Mode three_people;

    modes.insert(modes.end(), {no_people, one_person, two_people, three_people});

}

void Interactions::start_main_timer()
{
    main_timer = chrono::steady_clock::now();
}

bool Interactions::in_vector(int key, std::vector<int> v)
{
    // see if the person is in the labels
    if (std::find(v.begin(), v.end(), key) != v.end())
    {
        // element found
        return true;
    }
    else
    {
        return false;
    }
}

void Interactions::is_case_valid(int mode)
{
    for(int i = 0; i < modes.size(); i++)
    {
        if(i == mode)
        {
            modes.at(i).increment_cnt();
        }
        else
        {
            modes.at(i).decrement_cnt();
        }
    }

    //see which mode is the largest
    current_case = 0;
    for(int i = 1; i < modes.size(); i++)
    {
        if(modes.at(i).cnt > modes.at(current_case).cnt)
        {
            current_case = i;
        }
    }

    if (modes.at(current_case).cnt <= 10)
    {
        valid_case = false;
    }
    else
    {
        //if it wasn't already a valid case, then we want the pool to pull from the special mode first
        if(!valid_case)
        {
            //it is the first time of this case
            first_time = true;
            valid_case = true;
        }
    }
}

void Interactions::find_case(int num_people)
{
    switch(num_people)
    {
        case 0:
            is_case_valid(0);
            break;
        case 1:
            is_case_valid(1);
            break;
        case 2:
            is_case_valid(2);
            break;
        default:
            is_case_valid(3);
            break;
    }

}

void Interactions::update_people(std::vector<PersonInfo> &person_info, std::vector<Person> &people, cv::Mat &frame)
{
    int centerX, centerY, i;
    vector<int> this_round;

    for (auto person : person_info)
    {
#define DEBUG
#ifdef DEBUG
        cv::putText(frame, format("%d", person.label), Point(person.x1, person.y1 - 5),
                0, 0.6, Scalar(0, 0, 255), 2, LINE_AA);
        // FIXME: check the scalar color input
        cv::rectangle(frame, Rect(person.x1, person.y1, person.width, person.height), (0, 0, 255), 2);
#endif

        // find center x and y
        centerX = static_cast<int>(person.x1 + person.width/2);

#ifdef DEBUG
        centerY = static_cast<int>(person.y1+person.height/2);
        cv::Point center(centerX, centerY);
        cv::circle(frame, center, 3, (0, 0, 255), -1); //(image,(x,y),radius,col,thickness)
#endif

        //FIXME: MAY BE FASTER WAY TO DO THIS DEPENDING ON how data is stored in person_info
        // Handle if it's a new person
        if (!in_vector(person.label, prev_round))
        {
            //if this is the first person into the frame, start a timer
            if(people.size()==0)
            {
                start_main_timer();
            }
            // Element not found, create person
            Person new_person(person.label);
            people.push_back(new_person);
        }
        // set this label as found in this round
        this_round.push_back(person.label);
    }

    // check if elements from prev_round exist in this round, otherwise delete the person
    //TODO: Make sure this works
    i = 0;
    for (Person person : people)
    {
        if (!in_vector(person.label, this_round))
        {
            //add this persons audio back to the pool
            audio.pool |= person.heard_audio;
            people.erase(people.begin() + i);
            continue;
        }
        //TODO: Implement funny clip
        // else
        // {
        //     //see if person there for long time for funny clip
        // }
        i++;
    }
    // reset the label
    prev_round = this_round;
    find_case(people.size());

//    std::cout<<"current case: "<<current_case<<"\t"<<"valid case: "<<valid_case<<std::endl;

    //play an audio file from the pool of files if it's been enough time since the last audio file was played
    audio.handle(current_case, main_timer, valid_case, first_time, people);
}
