#include "interactions.hpp"

Interactions::Interactions(Audio &_audio, Servo &_servo):audio(_audio), servo(_servo)
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

void Interactions::track_w_servo(Person person)
{
    // experimentally x seemed to go from 40 - 600
    // so mapping them to an angle of 30 -> 200 degrees:
    int angle = int(float((560 - person.centerX)) * (170.0 / 560.0));
    servo.moveToAngle(angle);
//    std::cout<<"centerX: "<< (person.centerX) <<"\tAngle: "<<angle<<std::endl;
}

void Interactions::update_person(int label, std::vector<Person> &people, int coord)
{
    //uses lambda function to search for first instance where the person label is equal to the given person label
    auto it = std::find_if(people.begin(), people.end(), [& label](const Person& person) {return person.label == label;});

    //find the index using ptrdiff_t type
    long unsigned int index = std::distance(people.begin(), it);

    //updates the person x coordinate given someone is there
    if (index < people.size())
    {
        people.at(index).update_x(coord);
    }
    else{}

}


void Interactions::update_people(std::vector<PersonInfo> &person_info, std::vector<Person> &people, cv::Mat &frame)
{
    int centerX, centerY, i;
    vector<int> this_round;

    for (auto person : person_info)
    {
#ifdef DEBUG
        cv::putText(frame, format("%d", person.label), Point(person.x1, person.y1 - 5),
                0, 0.6, Scalar(0, 0, 255), 2, LINE_AA);
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
            Person new_person(person.label, centerX);
            //if this is the first person into the frame, start a timer
            if(people.size()==0)
            {
                start_main_timer();
                tracking_person = true;
            }
            people.push_back(new_person);
        }
        else
        {
            //upadte the persons x coordinate
             update_person(person.label, people, centerX);
        }
        // set this label as found in this round
        this_round.push_back(person.label);
    }




    //if we are tracking a person
    if(tracking_person)
    {
        //check if person_tracked is in this round, if not, set a new person to track
        if(!in_vector(people.at(0).label, this_round))
        {
            tracking_person = false;
            //delete the person we were tracking
            audio.pool |= people.at(0).heard_audio;
            people.erase(people.begin());
            i = 0;
        }
        else
        {
            i = 1;
        }
        //now update the rest of the people
        for (Person person : people)
        {
            if (!in_vector(person.label, this_round))
            {
                //add this persons audio back to the pool
                audio.pool |= person.heard_audio;
                people.erase(people.begin() + i);
                continue;
            }
            else
            {
                tracking_person = true;
            }
            //TODO: Implement funny clip
            // else
            // {
            //     //see if person there for long time for funny clip
            // }
            i++;
        }
    }


    // reset the label
    prev_round = this_round;
    //person to track
    find_case(people.size());

//    std::cout<<"tracking_person : "<<tracking_person<<std::end;
//    std::cout<<"current case: "<<tracking_person<<"\t"<<"valid case: "<<valid_case<<std::endl;

    //play an audio file from the pool of files if it's been enough time since the last audio file was played

    audio.handle(current_case, main_timer, valid_case, first_time, people);
}
