// for SX126x series or LLCC68
#include <SX126x.h>
SX126x LoRa;

char message[] = "HeLoRa World!";
uint8_t nBytes = sizeof(message);
uint8_t counter = 0;

const int BUFFER_SIZE = 25;
unsigned char received[BUFFER_SIZE];


void setup() {
  Serial.begin(115200);

  if (!LoRa.begin()) {
    Serial.println("Problem");
  }
  Serial.println("init ok");
}

void loop() {
  
  unsigned char montest[] = {0xC1, 0x00, 0x09};
  
  Serial.println("send config...");
  LoRa.beginPacket();
  LoRa.write(montest, sizeof(montest));
  LoRa.endPacket();
  LoRa.wait();

  delay(100);
  Serial.println("waiting response...");


  LoRa.request();
  LoRa.wait();

  int idx = 0;
  while (LoRa.available() > 1){
    received[idx++] = LoRa.read();
  }

  Serial.println("Response : ");
  char txt[40];
  sprintf(txt, "%X ", received);
  Serial.write(txt);

  /*
  LoRa.beginPacket();
  LoRa.write(message, nBytes);
  LoRa.write(counter++);
  LoRa.endPacket();
  LoRa.wait();
  */
  delay(1000);
}