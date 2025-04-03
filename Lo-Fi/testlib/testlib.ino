#include <sx126x.h>
sx126x lora;

char sync_word[] = {0xb5, 0x62};

void print_rssi()
{
  struct s_rssi rssi = lora.get_rssi();
  Serial.println(); 
  Serial.print("current_noise : ");
  Serial.println(rssi.current_noise);
  Serial.print("rssi_last_receive : ");
  Serial.println(rssi.rssi_last_receive);
}

int find_sync(char* buffer, int count_buffer, char* sync_word, int count_sync)
{
  for (int i = 0, j = 0; i < count_buffer; i++)
  {
    if (buffer[i] == sync_word[j]) {
      if (++j == count_sync)
        return i - count_sync;
    }
  }
  return -1;
}

void handle_packet(char* buffer, int count)
{
  Serial.println();

  for (int i = 0; i < count; i++) {
      Serial.print(buffer[i]);
  }
}

int receive() {
  int count = 0;
  int idx_payload_size = 4;
  int headers_size = 5;
  int payload_size = -1;
  int idx_sync = 0;
  int count_sync = 2;
  bool sync_ok = false;

  char buffer[240];
  
  while (serial_lora.available())
  {
    int x = serial_lora.read();
    
    if (!sync_ok)
    {
      if (count == idx_payload_size)
        payload_size = x;

      buffer[count++] = x;

      if (payload_size > -1 && count == payload_size + headers_size)
      {
        handle_packet(buffer, count);
      }
    }
    else
    {
      if (x == sync_word[idx_sync])
      {
        buffer[count++] = x;

        if (++idx_sync == count_sync)
          sync_ok = true;
      }
      else {
        idx_sync = 0;
        count = 0;
      }
    }

    delay(1);
  }


  return 0;
}

void setup()
{
  lora.begin();
  delay(10);

  lora.print_config();
  lora.set_channel(18);
  lora.set_air_data_rate(62.5);
  lora.set_tx_power(10);
  lora.set_address(65535);
  lora.set_network(0);
  lora.print_config();
}

void loop() 
{
  char msg[] = "hello xxxx dede";  
  //char msg[] = {0xb5, 0x62, 0x01, 0x02, 0x03};  
  lora.send(msg, sizeof(msg) - 1);
  delay(10);


  char buffer[512];
  int count = lora.receive(buffer);
  int idx_start = find_sync(buffer, count, sync_word, 2);

  if (idx_start >= 0) {
    //x
  }  

  delay(200);
}