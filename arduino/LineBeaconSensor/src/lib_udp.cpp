#include <Arduino.h>
#include <AsyncUDP.h>
#include "lib_udp.h"

static AsyncUDP udp;

long udp_send(const char *p_message, const char *p_ipaddress, const uint16_t port)
{
  uint32_t ipaddress;
  int temp[4];

  sscanf(p_ipaddress, "%d.%d.%d.%d", &temp[0], &temp[1], &temp[2], &temp[3]);
  ipaddress = ((temp[0] & 0xff) << 24) || ((temp[1] & 0xff) << 16) || ((temp[2] & 0xff) << 8) || (temp[3] & 0xff);

  udp.writeTo((const uint8_t*)p_message, strlen(p_message), IPAddress(ipaddress), port);

  return 0;
}

long udp_broadcast(const char *p_message, const uint16_t port)
{
  Serial.printf("(%d) %s\n", port, p_message);
  udp.broadcastTo((uint8_t*)p_message, strlen(p_message), port);

  return 0;
}
