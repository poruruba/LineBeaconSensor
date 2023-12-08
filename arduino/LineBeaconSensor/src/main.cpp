#include <M5StickC.h>
#include <ArduinoJson.h>
#include "wifi_utils.h"
#include "lib_linebeacon.h"
#include "lib_udp.h"
#include "lib_ir.h"

#define PIN_IR_SEND 32
#define PIN_IR_RECV 33
#define PIN_BRIGHTNESS 36

uint8_t linebeacon_hwid[5] = { 0x01, 0x02, 0x03, 0x04, 0x05 };

#define SEND_UDP_PORT 12346
#define LOOP_INTERVAL   5000

#define JSON_CAPACITY 1024
StaticJsonDocument<JSON_CAPACITY> jsonDoc;
decode_results results;

char *payload_make(JsonDocument& jsonDoc);
void payload_free(char *p_data);

void set_uint16b(uint8_t *p_buffer, uint16_t value){
  p_buffer[0] = (uint8_t)((value >> 8) & 0xff);
  p_buffer[1] = (uint8_t)(value & 0xff);
}

void setup() {
  // put your setup code here, to run once:
  long ret;
  M5.begin(true, true, true);
  Serial.println("setup start");

  wifi_try_connect(true);

  ret = linebeacon_initialize(linebeacon_hwid);
  if( ret != 0 ){
    Serial.printf("Error: linebeacon_initialize\n");
    while(1);
  }
  ret = linebeacon_start();
  if( ret != 0 ){
    Serial.printf("Error: linebeacon_start\n");
    while(1);
  }

  pinMode(PIN_BRIGHTNESS, ANALOG);
  ir_initialize(PIN_IR_RECV, PIN_IR_SEND);

  Serial.println("setup end");
}

void loop() {
  // put your main code here, to run repeatedly:
  long ret;
  M5.update();

  ret = ir_receive(&results);
  if( ret == 0 ){
    jsonDoc.clear();
    jsonDoc["type"] = "ir";
    jsonDoc["decode_type"] = results.decode_type;
    jsonDoc["value_high"] = (uint32_t)((results.value >> 32) & 0xffffffff);
    jsonDoc["value_low"] = (uint32_t)((results.value >> 0) & 0xffffffff);

    char *p_data = payload_make(jsonDoc);
    if( p_data != NULL ){
      Serial.println("udp_print");
      ret = udp_broadcast(p_data, SEND_UDP_PORT);
      Serial.printf("ret=%d\n", ret);

      payload_free(p_data);
      p_data = NULL;
    }
  }

  static uint8_t btn_isPressed_prev = 0x00;
  uint8_t btn_isPressed = 0x00;
  uint8_t btn_wasPressed = 0x00; 
  if( M5.BtnA.wasPressed() ){
    Serial.println("BtnA wasPressed");
    btn_wasPressed |= 0x01;
  }
  if( M5.BtnA.isPressed() ){
    btn_isPressed |= 0x01;
  }

  if( btn_wasPressed || (btn_isPressed != btn_isPressed_prev) ){
    jsonDoc.clear();
    jsonDoc["type"] = "button";
    jsonDoc["isPressed"] = btn_isPressed;
    jsonDoc["wasPressed"] = btn_wasPressed;

    char *p_data = payload_make(jsonDoc);
    if( p_data != NULL ){
      Serial.println("udp_print");
      ret = udp_broadcast(p_data, SEND_UDP_PORT);
      Serial.printf("ret=%d\n", ret);

      payload_free(p_data);
      p_data = NULL;
    }

    btn_isPressed_prev = btn_isPressed;
  }

  static unsigned long start_tim = millis();
  unsigned long end_tim = millis();
  if( (end_tim - start_tim) < LOOP_INTERVAL )
    return;
  start_tim = end_tim;

  uint16_t brightness = analogRead(PIN_BRIGHTNESS);
  Serial.printf("brightness=%d\n", brightness);

  jsonDoc.clear();
  jsonDoc["type"] = "sensor";
  jsonDoc["brightness"] = brightness;

  char *p_data = payload_make(jsonDoc);
  if( p_data != NULL ){
    Serial.println("udp_print");
    ret = udp_broadcast(p_data, SEND_UDP_PORT);
    Serial.printf("ret=%d\n", ret);

    payload_free(p_data);
    p_data = NULL;
  }

  delay(1);
}

char *payload_make(JsonDocument& jsonDoc){
  int size = measureJson(jsonDoc);
  char *p_data = (char*)malloc(size + 1);
  if( p_data == NULL ){
    Serial.printf("malloc error");
    return NULL;
  }
  int size2 = serializeJson(jsonDoc, p_data, size);
  if( size2 <= 0 ){
    Serial.printf("serializeJson error");
    free(p_data);
    return NULL;
  }
  p_data[size2] = '\0';

  return p_data;
}

void payload_free(char *p_data){
  free(p_data);
}
