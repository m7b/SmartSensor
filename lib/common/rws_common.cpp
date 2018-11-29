#include "rws_common.h"

unsigned long get_duration_ms(unsigned long start)
{
    unsigned long act = millis();
    unsigned long max = 0xffffffff;  // <= 4294967,295 seconds

    if (start > act)
        return  max - start + act;

    return act - start;
}


unsigned long get_duration_us(unsigned long start)
{
    unsigned long act = micros();
    unsigned long max = 0xffffffff;  // <= 4294,967295 seconds

    if (start > act)
        return  max - start + act;

    return act - start;
}

/*
constexpr unsigned long get_id(void)
{
    static unsigned long id = 0;
    return id++;
}
*/


std::string payload_to_string(uint8_t* payload, unsigned int length)
{
    return std::string();
}