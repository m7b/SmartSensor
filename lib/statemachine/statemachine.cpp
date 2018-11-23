#include "statemachine.h"

statemachine::statemachine()
{
    _step.id = 0;
    memset(_step.descr, 0, sizeof(_step.descr));

    _step_start = 0;
}
 
 
statemachine::~statemachine()
{
}


void statemachine::set_next_step(step next_step)
{
    _step       = next_step;
    _step_start = millis();
}


unsigned long statemachine::get_step(void)
{
    return _step;
}