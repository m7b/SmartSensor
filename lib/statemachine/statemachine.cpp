#include "statemachine.h"

statemachine::statemachine()
{
    _step = 0;
}
 
 
statemachine::~statemachine()
{
}


void statemachine::set_next_step(int step, const char *descr)
{
    _step = step;

    strncpy(_descr, descr, sizeof(_descr));
    _descr[sizeof(_descr) - 1] = '\0';
}


int statemachine::get_step(void)
{
    return _step;
}