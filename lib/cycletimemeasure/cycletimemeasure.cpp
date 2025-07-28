#include "cycletimemeasure.h"

CycleTimeMeasure::CycleTimeMeasure(int buffer_size)
    : _buffer_size(buffer_size), _index(0), _filled(false),
      _tmp1(0), _tmp2(0), _cycle_time(0), _ct(0.0f), _calculated(false)
{
    _buffer = new unsigned long[_buffer_size]();
}

CycleTimeMeasure::~CycleTimeMeasure()
{
    delete[] _buffer;
}

void CycleTimeMeasure::takeMeasurePoint()
{
    unsigned long now = micros();

    if (_tmp1 == 0) {
        _tmp1 = now;
        return;
    }

    _tmp2 = now;
    _cycle_time = _tmp2 - _tmp1;
    _tmp1 = _tmp2;

    _buffer[_index] = _cycle_time;
    _index = (_index + 1) % _buffer_size;

    if (_index == 0) _filled = true;
    _calculated = false;
}

void CycleTimeMeasure::calcCycleTime()
{
    if (_calculated) return;

    unsigned long sum = 0;
    int count = _filled ? _buffer_size : _index;

    for (int i = 0; i < count; ++i) {
        sum += _buffer[i];
    }

    _ct = count > 0 ? static_cast<float>(sum) / count : 0.0f;
    _calculated = true;
}

float CycleTimeMeasure::getCycleTime()
{
    calcCycleTime();
    return _ct;
}

std::string CycleTimeMeasure::getCycleTime(unsigned int precision)
{
    calcCycleTime();
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << _ct;
    return stream.str();
}