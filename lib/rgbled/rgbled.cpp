#include "rgbled.h"

rgbled::rgbled()
{
    _setup    = false;

    _red      = 0;
    _green    = 0;
    _blue     = 0;
    _act_led  = 0;

    _step     = 0;
    _delay_ms = 1000;
}

rgbled::rgbled(uint8_t red, uint8_t green, uint8_t blue)
{
    _setup    = false;
    
    _red      = red;
    _green    = green;
    _blue     = blue;
    _act_led  = 0;
    
    _step     = 0;
    _delay_ms = 1000;

    setup();
}

rgbled::~rgbled()
{
}

void rgbled::setup(void)
{
    if ((_red == 0) || (_green == 0) || (_blue == 0))
        return;

    pinMode(_red,   OUTPUT);
    pinMode(_green, OUTPUT);
    pinMode(_blue,  OUTPUT);

    _setup = true;
}

void rgbled::loop(void)
{
    if (!conditions_ok())
        return;

    switch (_step)
    {
        case 0: //init
            if (_act_led == 0)
                _act_led = _red;
            else
            if (_act_led == _red)
                _act_led = _green;
            else
            if (_act_led == _green)
                _act_led = _blue;
            else
            if (_act_led == _blue)
                _act_led = _red;
            
            _step++;

        case 1:
            digitalWrite(_act_led, HIGH);
            _start = millis();
            _step++;
            break;
            
        case 2:
            if (get_duration_ms(_start) >= _delay_ms)
                _step++;
            break;
            
        case 3:
            digitalWrite(_act_led, LOW);
            _step = 0;
            break;
    }
}

void rgbled::set_rgb_port(uint8_t red, uint8_t green, uint8_t blue)
{
    _red      = red;
    _green    = green;
    _blue     = blue;

    setup();
}

void rgbled::set_delay_ms(uint32_t delay_ms)
{
    _delay_ms = delay_ms;
}


bool rgbled::conditions_ok(void)
{
    if (!_setup)
        return false;
    
    return true;
}