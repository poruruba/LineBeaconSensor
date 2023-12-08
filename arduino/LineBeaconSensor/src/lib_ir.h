#ifndef _LIB_IR_H_
#define _LIB_IR_H_

#include <Arduino.h>
#include <IRRecv.h>

long ir_initialize(uint16_t recvpin, uint16_t sendpin);
long ir_receive(decode_results *p_results);
long ir_send(const uint16_t *p_raw, int len);

#endif