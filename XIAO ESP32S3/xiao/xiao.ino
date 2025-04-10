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

  state = radio.begin();
  if (state != RADIOLIB_ERR_NONE) Serial.println(state);
  delay(10);

  radio.setDio1Action(action_done);

  radio.setFrequency(868);
  radio.setBandwidth(250.0);
  radio.setSpreadingFactor(2);
  radio.setCodingRate(2);
  radio.setOutputPower(10);
  radio.setPreambleLength(8);
  radio.setCRC(false);
  
  delay(10);
}

void loop()
{
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
