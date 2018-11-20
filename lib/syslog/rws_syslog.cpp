#include "rws_syslog.h"
#include "../../include/settings/smartsensor_settings.h"

/**
 * @brief Construct a new smartsensor syslog::smartsensor syslog object
 * 
 */
smartsensor_syslog::smartsensor_syslog()
: Syslog(syslogUDP, SYSLOG_SERVER, SYSLOG_PORT, DEVICE_HOSTNAME, APP_NAME, LOG_KERN)
{
}

/**
 * @brief Destroy the smartsensor syslog::smartsensor syslog object
 * 
 */
smartsensor_syslog::~smartsensor_syslog()
{
}