#ifndef CYCLETIMEMEASURE_H
#define CYCLETIMEMEASURE_H

#include <rws_common.h>
#include <iomanip>
#include <sstream>

class CycleTimeMeasure {
public:
    CycleTimeMeasure(int buffer_size);
    ~CycleTimeMeasure();

    void takeMeasurePoint();
    float getCycleTime();
    std::string getCycleTime(unsigned int precision);

private:
    void calcCycleTime();

    unsigned long* _buffer;
    int _buffer_size;
    int _index;
    bool _filled;
    unsigned long _tmp1, _tmp2, _cycle_time;
    float _ct;
    bool _calculated;
};


#endif // CYCLETIMEMEASURE_H