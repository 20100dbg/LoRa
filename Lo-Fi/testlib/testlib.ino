#include <sx126x.h>
sx126x lora;

void setup()
{
  lora.begin();
  /*
  Serial.begin(115200);
  while (!Serial) {}
  Serial.print("here");
  */
  delay(10);
  //read_config();
}

void loop() 
{  
  delay(10);
}