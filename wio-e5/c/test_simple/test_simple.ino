#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial lora = SoftwareSerial(PB7, PB6);
//Usart1(PB6/PB7)
//PB7     ------> USART1_RX
//PB6     ------> USART1_TX

const int BUFFER_SIZE = 20;
unsigned char buf[BUFFER_SIZE];

void setup() {
  lora.begin(9600);
  delay(10);
}

void loop() {

/*
  int count = 0;
  while (lora.available())
  {
    buf[count++] = lora.read();
  }

  delay(500);

  for (int i=0; i < count; i++)
  {
    lora.write(buf[i]);
  }
*/
  lora.write("helLoRa");
  delay(500);
}
