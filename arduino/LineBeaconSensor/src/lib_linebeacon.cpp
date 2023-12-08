#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include "lib_linebeacon.h"

#define UUID_SERVICE_SHORT  0xFE6F
#define UUID_SERVICE BLEUUID((uint16_t)UUID_SERVICE_SHORT)

static uint8_t LINE_BEACON_HWID[5]; // LINE BEACON HWID
static BLEAdvertising *pAdvertising;
static bool is_running = false;

long linebeacon_set_device_message(const char *p_device_message, int8_t tx)
{
  uint8_t message_len = strlen(p_device_message);
  if( message_len < 1 || message_len > 13 )
    return -1;

  BLEAdvertisementData advertisementData = BLEAdvertisementData();
  advertisementData.setFlags(0x06);
  
  std::string strServiceData = "";

  strServiceData += (char)0x03;
  strServiceData += (char)0x03;
  strServiceData += (char)0x6f;
  strServiceData += (char)0xfe;

  strServiceData += (char)(10 + message_len);
  strServiceData += (char)0x16;
  strServiceData += (char)(UUID_SERVICE_SHORT & 0xff);
  strServiceData += (char)((UUID_SERVICE_SHORT >> 8) & 0xff);
  strServiceData += (char)0x02; // LINE Simple Beacon FrameのFrame Type
  strServiceData += LINE_BEACON_HWID[0]; // LINE Simple Beacon FrameのHWID(5B)
  strServiceData += LINE_BEACON_HWID[1];
  strServiceData += LINE_BEACON_HWID[2];
  strServiceData += LINE_BEACON_HWID[3];
  strServiceData += LINE_BEACON_HWID[4];
  strServiceData += (char)tx; // LINE Simple Beacon FrameのMeasured TxPower

  // LINE Simple Beacon FrameのDevice Message(1-13)
  for( int i = 0 ; i < message_len ; i++ )
    strServiceData += p_device_message[i];

  advertisementData.addData(strServiceData);
  pAdvertising->setAdvertisementData(advertisementData);

  return 0;
}

long linebeacon_set_device_message2(const uint8_t *p_device_message, uint8_t message_len, int8_t tx)
{
  if( message_len < 1 || message_len > 13 )
    return -1;

  BLEAdvertisementData advertisementData = BLEAdvertisementData();
  advertisementData.setFlags(0x06);
  
  std::string strServiceData = "";

  strServiceData += (char)0x03;
  strServiceData += (char)0x03;
  strServiceData += (char)0x6f;
  strServiceData += (char)0xfe;

  strServiceData += (char)(10 + message_len);
  strServiceData += (char)0x16;
  strServiceData += (char)(UUID_SERVICE_SHORT & 0xff);
  strServiceData += (char)((UUID_SERVICE_SHORT >> 8) & 0xff);
  strServiceData += (char)0x02; // LINE Simple Beacon FrameのFrame Type
  strServiceData += LINE_BEACON_HWID[0]; // LINE Simple Beacon FrameのHWID(5B)
  strServiceData += LINE_BEACON_HWID[1];
  strServiceData += LINE_BEACON_HWID[2];
  strServiceData += LINE_BEACON_HWID[3];
  strServiceData += LINE_BEACON_HWID[4];
  strServiceData += (char)tx; // LINE Simple Beacon FrameのMeasured TxPower

  // LINE Simple Beacon FrameのDevice Message(1-13)
  for( int i = 0 ; i < message_len ; i++ )
    strServiceData += p_device_message[i];

  advertisementData.addData(strServiceData);
  pAdvertising->setAdvertisementData(advertisementData);

  return 0;
}

long linebeacon_initialize(const uint8_t *p_hwid)
{
  BLEDevice::init(LINEBEACON_DEVICE_NAME);

  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(UUID_SERVICE);
  pService->start();

  pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(UUID_SERVICE);

  memmove(LINE_BEACON_HWID, p_hwid, sizeof(LINE_BEACON_HWID));

  uint8_t tmp = 0;
  linebeacon_set_device_message2(&tmp, 1, LINEBEACON_DEFAULT_TX);

  return 0;
}

long linebeacon_start(void)
{
  if( !is_running ){
    is_running = true;
    pAdvertising->start();
  }

  return 0;
}

long linebeacon_stop(void)
{
  if( is_running ){
    pAdvertising->stop();
    is_running = false;
  }

  return 0;
}

