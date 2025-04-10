#include <sx126x.h>
sx126x lora;

char sync_word[] = {0xb5, 0x62};
int IDX_PAYLOAD_SIZE;
int HEADERS_SIZE;


void print_rssi()
{
  struct s_rssi rssi = lora.get_rssi();
  char buf[40];
  sprintf(buf, "Current noise : %d - Last RSSI : %d", rssi.current_noise, rssi.rssi_last_receive);
  Serial.println(buf);
}

//Search for sync word in buffer and return index of sync position, or -1 if not found
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
  lora.print_hex(buffer, count);
  print_rssi();
}    


//Receive data as packet. Will look for SYNC word and a field containing payload size
//Needs to know where is payload size, and headers size (= size of all non-payload data)
int receive_packet(int idx_payload_size, int headers_size) {
  int count = 0;
  int payload_size = -1;
  int idx_sync = 0;
  int count_sync = sizeof(sync_word); //2
  bool sync_ok = false;

  char buffer[240];
 
  while (serial_lora.available())
  {
    int x = serial_lora.read();
    
    //SYNC word is found
    if (sync_ok)
    {
      //is this byte the payload size ?
      if (count == idx_payload_size) {
        payload_size = x;
      }

      //Record data in buffer
      buffer[count++] = x;

      //if packet is complete, send to handle_packet()
      if (payload_size > -1 && 
          count == payload_size + headers_size + lora.get_enable_rssi())
      {
        handle_packet(buffer, count);
        sync_ok = false;
        count = 0;
        idx_sync = 0;
        payload_size = -1;
      }
    }
    else {
      //look for SYNC word

      if (x == sync_word[idx_sync++]) {
        buffer[count++] = x;
        
        if (idx_sync == count_sync) {
          sync_ok = true;
        }
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
  IDX_PAYLOAD_SIZE = 2;
  HEADERS_SIZE = 3;

  lora.begin();
  delay(10);

  /*
  lora.print_config();
  Serial.println("______________");
  lora.set_channel(18);
  lora.set_air_data_rate(62.5);
  lora.set_tx_power(10);
  lora.set_address(65535);
  lora.set_network(0);
  */
  lora.set_channel_noise(true);

  //lora.set_enable_rssi(true);
  lora.print_config();
}


void loop() 
{
  char msg[] = "hello xxxx dede";  
  //char msg[] = {0xb5, 0x62, 0x01, 0x02, 0x03};  
  //lora.send(msg, sizeof(msg) - 1);
  //delay(10);


  receive_packet(IDX_PAYLOAD_SIZE, HEADERS_SIZE);

  delay(200);
}