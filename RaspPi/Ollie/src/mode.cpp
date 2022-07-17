#include "mode.hpp"

Mode::Mode()
{
}

void Mode::increment_cnt()
{
    if(cnt<20)
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
