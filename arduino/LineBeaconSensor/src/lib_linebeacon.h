#ifndef _LIB_LINEBEACON_H_
#define _LIB_LINEBEACON_H_

#include <Arduino.h>

#if defined(ARDUINO_M5Stick_C)
#define LINEBEACON_DEVICE_NAME  "M5StickC"
#elif defined(ARDUINO_M5Stack_ATOM)
#define LINEBEACON_DEVICE_NAME  "M5Atom"
#elif defined(ARDUINO_ESP32C3_DEV)
#define LINEBEACON_DEVICE_NAME  "M5Stamp"
#else
#define LINEBEACON_DEVICE_NAME  "ESP32"
#endif

#define LINEBEACON_DEFAULT_TX -56

long linebeacon_initialize(const uint8_t *p_hwid);
long linebeacon_set_device_message(const char *p_device_message, int8_t tx);
long linebeacon_set_device_message2(const uint8_t *p_device_message, uint8_t len, int8_t tx);
long linebeacon_start(void);
long linebeacon_stop(void);

#endif
