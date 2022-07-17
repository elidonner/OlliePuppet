#include "interactions.hpp"
//store if we have a valid case

Interactions::Interactions()
{
}

void Interactions::start_main_timer(Audio & _audio) : audio(_audio)
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
    static chrono::steady_clock::time_point Tbegin = chrono::steady_clock::now();

    chrono::steady_clock::time_point Tend;

    static int prev_mode = 0;
    if (mode == prev_mod)
    {
        //update the end time
        Tend = chrono::steady_clock::now();

        float diff = chrono::duration_cast<chrono::seconds>(Tend - Tbegin).count();

        //see if the time has been > 10 seconds
        if (diff <= 10)
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
    else {
        valid_case = false;
        first_time = false;
        //restart the timer
        Tbegin = chrono::steady_clock::now();
    }
}

void Interactions::find_case(int num_people)
{
    switch(num_people)
    case(0){
        current_case = 0;
        valid_case = false;
        break;
    }
    case(1){
        current_case = 1;
        is_case_valid(1);
    }
    case(2){
        current_case = 2;
        is_case_valid(2);
    }
    default{
        current_case = 3;
        is_case_valid(3);
    }

}

void Interactions::update_people(Std::vector<PersonInfo> &person_info, Std::vector<Person> &people, cv::Mat &frame)
{
    int centerX, centerY, i;
    vector<int> this_round;

    for (auto person : person_info)
    {
#ifdef DEBUG
        //            cv::Point pt1(person.x1, person.y1);
        //            cv::Point pt2(person.x2, person.y2);
        //            cv::rectangle(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2);
        putText(frame, format("%d", person.label), Point(tlwh[0], tlwh[1] - 5),
                0, 0.6, Scalar(0, 0, 255), 2, LINE_AA);
        // FIXME: check the scalar color input
        rectangle(frame, Rect(tlwh[0], tlwh[1], tlwh[2], tlwh[3]), (0, 0, 255), 2);
#endif

        // find center x and y
        centerX = int((person.x1 + person.x2)) / 2;

#ifdef DEBUG
        cv::Point center(centerX, centerY);
        cv::circle(frame, center, 5, (0, 0, 255), -1); //(image,(x,y),radius,col,thickness)
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
    find_case(people.size())
    //play an audio file from the pool of files if it's been enough time since the last audio file was played
    audio.handle(current_case, main_timer, valid_case, first_time, people);
}