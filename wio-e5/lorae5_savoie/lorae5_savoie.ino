#include <Arduino.h>
#include "lorae5.h"
#include "config_application.h"
#include "config_board.h"

uint8_t sizePayloadUp = 4;
uint8_t sizePayloadDown = 0;

uint8_t payloadUp[20] = {0x00, 0x01, 0x02, 0x03};
uint8_t payloadDown[20]  ={0};

LORAE5 lorae5(devEUI, appEUI, appKey, devAddr, nwkSKey, appSKey);

/***********************************************************************/
/* Please see README page on https://github.com/SylvainMontagny/LoRaE5 */
/***********************************************************************/

void setup() {
  lorae5.setup_hardware(&Debug_Serial, &LoRa_Serial);
  lorae5.printInfo();

   delay(500);
}

void loop() {
  // Send data
  lorae5.sendData(payloadUp, sizePayloadUp);
  
  // Check downlink on RX1-RX2 and call processDownlink() 
  if (lorae5.awaitForDownlinkClass_A(payloadDown, &sizePayloadDown) == RET_DOWNLINK){
    processDownlink();
  };
  
  // Check downlink on RXC and call processDownlink() 
  if (lorae5.awaitForDownlinkClass_C(payloadDown, &sizePayloadDown) == RET_DOWNLINK){
    processDownlink();
  };

  delay(500);
}

void processDownlink() {
  // You have received "sizePayloadDown" bytes stored in the table "payloadDown"

  for (int i = 0; i < sizeof(payloadDown); i++)
  {
    Debug_Serial.println(payloadDown[i]);
  }
  
} 