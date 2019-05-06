#include "statemachine.h"

statemachine::statemachine(unsigned long init_step)
{
    _step.id = init_step;
    memset(_step.descr, 0, sizeof(_step.descr));

    _step_start = init_step;
}


statemachine::~statemachine()
{
}


void statemachine::set_next_step(const step &next_step)
{
    bool print_step_id = false;

    if (_step == next_step)
        return;

    _step       = next_step;
    _step_start = millis();

    char buffer [80];
    if (print_step_id)
        snprintf(buffer, sizeof(buffer), "Step %ld: %s\r\n", (unsigned long) _step, _step.descr);
    else
        snprintf(buffer, sizeof(buffer), "%s\r\n", _step.descr);

    Serial.print(buffer);
}


statemachine::step statemachine::get_step(void)
{
    return _step;
}


unsigned long statemachine::get_step_start(void)
{
    return _step_start;
}


void statemachine::print_step_info(const step &s)
{
    Serial.printf("%ld: %s \r\n", s.id, s.descr);
}