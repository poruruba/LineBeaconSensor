#ifndef _LIB_UDP_H_
#define _LIB_UDP_H_

#include <Arduino.h>

long udp_send(const char *p_message, const char *p_ipaddress, const uint16_t port);
long udp_broadcast(const char *p_message, const uint16_t port);

#endif
