#include "pump.h"

pump::pump(uint8_t output_pin, uint8_t input_pin)
{
    _output_pin = output_pin;
    _input_pin  = input_pin;
    _setup      = false;
    _step       = 0;
}

pump::~pump()
{
}

void pump::setup(void)
{
    if (_output_pin == 0)
        return;

    pinMode(_output_pin, OUTPUT);
    pinMode(_input_pin, INPUT);
    _setup = true;
}

void pump::loop(void)
{
    if (!conditions_ok())
        return;

    switch (_step)
    {
        case 0: //init
            _step++;
            break;

        case 1:
            digitalWrite(_output_pin, HIGH);
            _start = millis();
            _step++;
            break;

        case 2:
            if (get_duration_ms(_start) >= _delay_ms)
                _step++;
            break;

        case 3:
            digitalWrite(_output_pin, LOW);
            _step = 0;
            break;
    }
}


bool pump::conditions_ok(void)
{
    if (!_setup)
        return false;

    return true;
}