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


void statemachine::set_next_step(const step &next_step)
{
    bool print_step_id = false;

    _step       = next_step;
    _step_start = millis();

    char buffer [80];
    if (print_step_id)
        snprintf(buffer, sizeof(buffer), "Step %ld: %s\r\n", (unsigned long) _step, _step.descr);
    else
        snprintf(buffer, sizeof(buffer), "%s\r\n", _step.descr);

    Serial.print(buffer);
}


unsigned long statemachine::get_step(void)
{
    return _step;
}