#include "mode.hpp"

Mode::Mode()
{
}

void Mode::increment_cnt()
{
    if(cnt<40)
    {
        cnt++;
    }
}

void Mode::decrement_cnt()
{
    if(cnt>0)
    {
        cnt--;
    }
}
