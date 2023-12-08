#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include "lib_ir.h"

static IRsend *g_irsend = NULL;
static IRrecv *g_irrecv = NULL;

long ir_initialize(uint16_t recvpin, uint16_t sendpin)
{
  g_irsend = new IRsend(sendpin);
  g_irrecv = new IRrecv(recvpin);

  g_irsend->begin();
  g_irrecv->enableIRIn();

  return 0;
}

long ir_receive(decode_results *p_results){
  if (g_irrecv->decode(p_results)){
    Serial.printf("received: decode_type=%d\n", p_results->decode_type);
    if( p_results->decode_type == decode_type_t::NEC && !p_results->repeat){
      g_irrecv->resume();
      return 0;
    }
    g_irrecv->resume();
  }

  return -1;
}

long ir_send(const uint16_t *p_raw, int len)
{
  g_irsend->sendRaw(p_raw, len, 38);
  g_irrecv->resume();

  return 0;
}