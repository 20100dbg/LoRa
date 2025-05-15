#include <sx126x.h>
sx126x lora;

//SYNC (2) + FROM (1) + TO (1) + SIZE (1) + MSG


String list_msg[] = {"En place", "Zone RAS", "Demande de releve",
                    "VHL en approche", "Survol drone",
                    "Demande identification"};
int list_msg_size = 6;

char sync_word[] = {0xb5, 0x62};
int IDX_PAYLOAD_SIZE = 4;
int HEADERS_SIZE = 5;
int local_addr = 1;
int to = 1;
int nb_modules = 3;


void send(int to, String msg) {

  int buf_size = HEADERS_SIZE + msg.length();
  char buffer[buf_size];
  buffer[0] = sync_word[0];
  buffer[1] = sync_word[1];
  buffer[2] = local_addr;
  buffer[3] = to;
  buffer[4] = msg.length();
  
  for (int i = 0; i < msg.length(); i++) {
    buffer[5+i] = msg[i];
  }

  lora.send(buffer, buf_size);
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
  int from = buffer[2];
  int to = buffer[3];
  int size = buffer[4];

  if (to == local_addr) {
    if (random(0,20) == 7) {
      long idx_msg = random(0, list_msg_size);
      send(to, list_msg[idx_msg]);

    }
  }
}


//Receive data as packet. Will look for SYNC word and a field containing payload size
//Needs to know where is payload size, and headers size (= size of all non-payload data)
int receive_packet() {
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
      if (count == IDX_PAYLOAD_SIZE) {
        payload_size = x;
      }

      //Record data in buffer
      buffer[count++] = x;

      //if packet is complete, send to handle_packet()
      if (payload_size > -1 && 
          count == payload_size + HEADERS_SIZE + lora.get_enable_rssi())
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
  randomSeed(analogRead(0));

  lora.set_debug(true);
  lora.begin();

  lora.set_channel(18);
  lora.set_air_data_rate(2.4);
  lora.set_tx_power(22);
  lora.set_address(65535);
  lora.set_network(0);
  lora.set_channel_noise(false);

  bool result = lora.save_config();
  Serial.print("save_config : ");
  Serial.println(result);
  lora.print_config();
}


void loop() 
{
  if (true) { //random(0,20) == 7) {
    long idx_msg = random(0, list_msg_size);

    send(to, list_msg[idx_msg]);
    delay(50);
  }

  receive_packet();
  delay(1000);
}