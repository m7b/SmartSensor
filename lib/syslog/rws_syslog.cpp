#include "rws_syslog.h"

/**
 * @brief Construct a new smartsensor syslog::smartsensor syslog object
 *
 */
rws_syslog::rws_syslog(const char* server, uint16_t port, const char* deviceHostname, const char* appName, uint16_t priDefault, uint8_t protocol)
: Syslog(syslogUDP, server, port, deviceHostname, appName, priDefault)
{
}


/**
 * @brief Destroy the smartsensor syslog::smartsensor syslog object
 *
 */
rws_syslog::~rws_syslog()
{
}