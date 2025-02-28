#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial Serial2 = SoftwareSerial(PB7, PB6);
//Usart1(PB6/PB7)
//PB7     ------> USART1_RX
//PB6     ------> USART1_TX

void setup(void)
{
    pinMode(PB5, OUTPUT);
    Serial2.begin(115200);
}

void loop(void)
{
    Serial2.println("here");
    digitalWrite(PB5, (millis() / 500) % 2);
}