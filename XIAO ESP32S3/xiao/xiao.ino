#include <Arduino.h>
#include <RadioLib.h>

#define LORA_SCK 7    // confirmed
#define LORA_MISO 8   // confirmed
#define LORA_MOSI 9  // confirmed

#define LORA_DIO2 38
#define LORA_DIO1 39     // irq
#define LORA_BUSY 40
#define LORA_CS 41    //NSS
#define LORA_RESET 42

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RESET, LORA_BUSY);
//SX1262 radio = new Module(10, 2, 3, 9);


static const int max_size = 256;
byte serial_buffer[max_size];


volatile bool receivedFlag = false;
void setFlag(void) {
  receivedFlag = true;
}


void setup() {
  
  Serial.begin(115200);
  Serial.setTimeout(100);

  float freq = 868.175; //150-960mhz
  float bw = 125.0; //125,250,500
  uint8_t sf = 7; //7,8,9,10,11,12
  uint8_t cr = 5; //5,6,7,8
  uint16_t preambleLength = 10;
  uint8_t syncWord = RADIOLIB_SX126X_SYNC_WORD_PRIVATE;
  int8_t power = 10; //22, 17, 14, 10

  radio.begin(freq, bw, sf, cr, syncWord, power, preambleLength);
  radio.setFrequency(freq);
  
  radio.setDio1Action(setFlag);
  radio.startReceive();

  Serial.print("ready");
}


void loop()
{

  if(receivedFlag) {
    receivedFlag = false;
    int count = radio.getPacketLength(true);

    if (count > 0) {
      byte received_buffer[count];
      int state = radio.readData(received_buffer, count);

      if (state == RADIOLIB_ERR_NONE) {
        Serial.write(received_buffer, count);
      }
    }
  }

  int count = Serial.readBytes(serial_buffer, max_size);
  
  if (count > 0) {

    radio.clearDio1Action();

    byte send_buffer[count];
    memcpy(send_buffer, serial_buffer, count);
    radio.transmit(send_buffer, count);
    delay(50);
    
    radio.setDio1Action(setFlag);
    radio.startReceive();

  }
}

