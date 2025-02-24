#include <sx126x.h>
sx126x lora;

void setup()
{
  lora.begin();
  delay(10);

  lora.print_config();
}

void loop() 
{
  char msg[16] = "hello xxxx dede";
  
  lora.send(msg, sizeof(msg) - 1);
  delay(10);

  char buffer[200];
  int count = lora.receive(buffer);
  
  
  if (count > 0) {
    Serial.println();
    
    for (int i = 0; i < count; i++) {
      Serial.print(buffer[i]);
    }

    struct s_rssi rssi = lora.get_rssi();
    Serial.println(); 
    Serial.print("current_noise : ");
    Serial.println(rssi.current_noise);
    Serial.print("rssi_last_receive : ");
    Serial.println(rssi.rssi_last_receive);
  }

  delay(5000);

}