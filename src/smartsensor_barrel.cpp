#include "smartsensor_barrel.h"
#include "settings/smartsensor_settings.h"

/**
 * @brief Construct a new smartsensor barrel::smartsensor barrel object
 * 
 */
smartsensor_barrel::smartsensor_barrel(smartsensor_ntp *ntp, smartsensor_pubsubclient *mqtt, smartsensor_syslog *syslog)
: NewPing(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE_CM), ntp(ntp), mqtt(mqtt), syslog(syslog)
{
    this->max_fill_level_cm = MAX_FILL_LEVEL_CM;
}

/**
 * @brief Destroy the smartsensor barrel::smartsensor barrel object
 * 
 */
smartsensor_barrel::~smartsensor_barrel()
{
}


bool smartsensor_barrel::do_measure(void)
{
    bool rc = false;
    
    unsigned long meassured_value;

    meassured_value = ping_cm();
    if ((meassured_value <= 0) || (meassured_value > max_fill_level_cm))
        return rc;
    
    rc = true;
   
    sFillLevel tmp;
    
    tmp.sensor_cm = meassured_value;
    tmp.cm        = max_fill_level_cm - meassured_value;
    tmp.percent   = (max_fill_level_cm / 100) * tmp.cm;
    tmp.utc       = ntp->getEpochTime();

    fill_level = tmp;

    return rc;
}


bool smartsensor_barrel::do_publish(void)
{
    bool rc = true;

    //Publish fill level
    rc = rc & do_publish_mqtt();
    rc = rc & do_publish_syslog();
    rc = rc & do_publish_serial();

    return rc;
}

/**
 * @brief 
 * 
 * @return sFillLevel 
 */
sFillLevel smartsensor_barrel::get_fill_level(void)
{
    return fill_level;
}


unsigned long smartsensor_barrel::get_fill_level_cm(void)
{
    return fill_level.cm;
}

float smartsensor_barrel::get_fill_level_percent(void)
{
    return fill_level.percent;
}

std::string smartsensor_barrel::get_local_datetime(void)
{
    return ntp->get_local_datetime(fill_level.utc);
}


        
bool smartsensor_barrel::do_publish_mqtt(void)
{
    bool rc = true;

    //Publish fill level
    rc = rc & mqtt->publish(LEVEL_SENSOR_CM_TOPIC, fill_level.sensor_cm);
    rc = rc & mqtt->publish(LEVEL_CM_TOPIC,        fill_level.cm);
    rc = rc & mqtt->publish(LEVEL_PERCENT_TOPIC,   fill_level.percent);
    rc = rc & mqtt->publish(LEVEL_TIMESTAMP_TOPIC, fill_level.utc);

    return rc;
}

bool smartsensor_barrel::do_publish_syslog(void)
{
    bool rc;

    rc = syslog->logf(LOG_INFO, "Zeitstempel: %s; Füllstand: %dcm; Prozentual: %.2f%%",
        get_local_datetime().c_str(),
        (int)fill_level.cm,
        fill_level.percent);

    return rc;
}
bool smartsensor_barrel::do_publish_serial(void)
{
    bool rc = false;

    int len = Serial.printf("%s - Füllstand: %dcm; Prozentual: %.2f\r\n",
        get_local_datetime().c_str(),
        (int)fill_level.cm,
        fill_level.percent);

    if (len > 0)
        rc = true;
    
    return rc;
}