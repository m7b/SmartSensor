#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include <string>

#define FIRMWARE_VERSION_DATE_TIME FIRMWARE_VERSION ", " __DATE__ ", "  __TIME__

unsigned long get_duration_ms(unsigned long start);
unsigned long get_duration_us(unsigned long start);
//constexpr unsigned long get_id(void);

std::string payload_to_string(uint8_t* payload, unsigned int length);

#endif // COMMON_H