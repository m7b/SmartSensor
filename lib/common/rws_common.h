#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include <string>

#define RWS_VERSION "2024-07-29-002"

unsigned long get_duration_ms(unsigned long start);
unsigned long get_duration_us(unsigned long start);
//constexpr unsigned long get_id(void);

std::string payload_to_string(uint8_t* payload, unsigned int length);

#endif // COMMON_H