#include "cycletimemeassure.h"

cycletimemeassure::cycletimemeassure(int buffer_size)
{
    _buffer = new unsigned long[buffer_size];
    _buffer_size = buffer_size;
    _cycle_time = 0;
    _i    = 0;
    _tmp1 = 0;
    _tmp2 = 0;
    _calculated = false;
}

cycletimemeassure::~cycletimemeassure()
{
    delete _buffer;
}

void cycletimemeassure::take_meassure_point(void)
{

    if (_i >= _buffer_size) {
        _i = _buffer_size-1;
        for (int i=0; i<_i; i++)
            _buffer[i] = _buffer[i+1];
    }

    if (_tmp1 == 0) {
        _tmp1 = micros();
    }
    else {
        _tmp2 = micros();
        _cycle_time = _tmp2 - _tmp1;
        _tmp1 = _tmp2;

        _buffer[_i++] = _cycle_time;
    }

    _calculated = false;
}

void cycletimemeassure::calc_ct(void)
{
    if (!_calculated) {
        unsigned long tmp = 0;
        for (int i=0; i<_i; i++)
            tmp += _buffer[i];

        if (_i > 0) {
            _ct = static_cast<float>(tmp) / _i;  // float-Division
        } else {
            _ct = 0.0f;
        }

        _calculated = true;
    }
}

float cycletimemeassure::get_cycle_time(void)
{
    calc_ct();
    return _ct;
}

std::string cycletimemeassure::get_cycle_time(unsigned int precision)
{
    calc_ct();
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << _ct;
    return stream.str();
}