#include "smartsensor_barrel.h"
#include "settings/smartsensor_settings.h"

/**
 * @brief Construct a new smartsensor barrel::smartsensor barrel object
 * 
 */
smartsensor_barrel::smartsensor_barrel()
: NewPing(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE_CM)
{
    this->max_fill_level_cm = MAX_FILL_LEVEL_CM;
}

/**
 * @brief Construct a new smartsensor barrel::smartsensor barrel object
 * 
 * @param max_fill_level_cm 
 */
smartsensor_barrel::smartsensor_barrel(unsigned long max_fill_level_cm)
: NewPing(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE_CM)
{
    this->max_fill_level_cm = max_fill_level_cm;
}

/**
 * @brief Destroy the smartsensor barrel::smartsensor barrel object
 * 
 */
smartsensor_barrel::~smartsensor_barrel()
{
}

/**
 * @brief Get calculated fill level in cm
 * 
 * @return unsigned long 
 */
unsigned long smartsensor_barrel::get_fill_level_cm(void)
{
    unsigned long value_cm;

    value_cm = ping_cm();
    if (value_cm <= 0)
        return value_cm;
    
    return max_fill_level_cm - value_cm;
}