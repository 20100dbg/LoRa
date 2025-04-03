#include <RadioLib.h>


#define LORA_MISO 8   // confirmed
#define LORA_SCK 7    // confirmed
#define LORA_MOSI 9  // confirmed
#define LORA_CS 41    //NSS
#define LORA_DIO2 38
#define LORA_DIO1 39     // irq
#define LORA_RESET 42
#define LORA_BUSY 40

int state = RADIOLIB_ERR_NONE;


SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RESET, LORA_BUSY);
//SX1262 radio = new Module(10, 2, 3, 9);

void action_done(void) {
  //here
}


void setup() {
  
  Serial.begin(9600);

  // initialize SX1268 with default settings
  Serial.print(F("[SX1262] Initializing ... "));
  state = radio.begin();
  if (state != RADIOLIB_ERR_NONE) Serial.println(state);
  delay(500);

  // set the function that will be called
  // when new packet is received
  radio.setDio1Action(action_done);

  if (radio.setFrequency(868) == RADIOLIB_ERR_INVALID_FREQUENCY) {
    Serial.println(F("Selected frequency is invalid for this module!"));
  }

  if (radio.setBandwidth(250.0) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
    Serial.println(F("Selected bandwidth is invalid for this module!"));
  }

  if (radio.setSpreadingFactor(10) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
    Serial.println(F("Selected spreading factor is invalid for this module!"));
  }

  if (radio.setCodingRate(6) == RADIOLIB_ERR_INVALID_CODING_RATE) {
    Serial.println(F("Selected coding rate is invalid for this module!"));
  }

  if (radio.setOutputPower(10) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
    Serial.println(F("Selected output power is invalid for this module!"));
  }

  if (radio.setPreambleLength(8) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
    Serial.println(F("Selected preamble length is invalid for this module!"));
  }

  if (radio.setCRC(false) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
    Serial.println(F("Selected CRC is invalid for this module!"));
  }
  delay(500);
}

void loop() {

    Serial.print(F("[SX1262] Sending first packet ... "));
    state = radio.startTransmit("Hello World!");
    if (state != RADIOLIB_ERR_NONE) Serial.println(state);
    delay(500);

    Serial.print(F("[SX1262] Starting to listen ... "));
    state = radio.startReceive();
    if (state != RADIOLIB_ERR_NONE) Serial.println(state);

    String str;
    state = radio.readData(str);

    if (state == RADIOLIB_ERR_NONE) {
      Serial.println(str);
      Serial.println(radio.getRSSI());
      Serial.println(radio.getSNR());
    }
    else Serial.println(state);

    delay(1000);
}
