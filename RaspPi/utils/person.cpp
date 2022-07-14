#include "person.hpp"
Person::Person(int centerX,
            int centerY)
    : x(centerX),
        y(centerY)
{
cnt = 1; //person starts with cnt of 1
}

void Person::update_pose(int new_x, int new_y)
{
    x = new_x;
    y = new_y;
}

void Person::increment_cnt()
{
    //constrain the count so it doesn't take long to delete a person
    if (cnt<20)
    {
        cnt++;
    }
}

void Person::decerement_cnt()
{
    cnt--;
}
