#include "conf.h"
//#define lora Serial2

class sx126x
{
  private:
    lora Serial2;

    void set_mode(int mode);
    
  public:
    void setup();
};
 
// Définition des méthodes.
void sx126x::setup()
{
  Serial.begin(115200);
  lora.begin(9600, SERIAL_8N1, 18, 17); //9600 mandatory

  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  set_mode(MODE_TRANS);
  
  //very important to read here
  delay(10);
  while (lora.available()) lora.read();
}

///////////


void set_mode(int mode) {
  
  if (mode == MODE_CONF) {
    digitalWrite(M0, LOW);
    digitalWrite(M1, HIGH);
  }
  else if (mode == MODE_WOR) {
    digitalWrite(M0, HIGH);
    digitalWrite(M1, LOW);
  }
  else if (mode == MODE_CONF) {
    digitalWrite(M0, HIGH);
    digitalWrite(M1, HIGH);
  }
  else {
    digitalWrite(M0, LOW);
    digitalWrite(M1, LOW);
  }
  delay(50);
}

void config(byte* data) {
  set_mode(MODE_CONF);
  
  lora.write((byte*)&data, sizeof(data));
  delay(10);

  set_mode(MODE_TRANS);
}


byte* set_config(int addrh, int addrl, int netid,
                int serial_port_rate, int serial_parity_bit, int air_data_rate,
                int sub_packet_size, int channel_noise, int tx_power, 
                int channel, int enable_rssi, int transmission_mode, 
                int enable_repeater, int enable_lbt,
                int wor_control, int wor_cycle, int crypth, int cryptl) {

  byte settings[12] = {0xC0, 0x00, 0x09};
  int reserve = 0;

  settings[3] = addrh;
  settings[4] = addrl;
  settings[5] = netid;
  settings[6] = serial_port_rate + serial_parity_bit + air_data_rate;
  settings[7] = sub_packet_size + channel_noise + reserve + tx_power;
  settings[8] = channel;
  settings[9] = enable_rssi + transmission_mode + enable_repeater + 
                enable_lbt + wor_control + wor_cycle;
  settings[10] = crypth;
  settings[11] = cryptl;

  return settings;
}

void read_config()
{
  char data[] = {0xC1, 0x00, 0x09};
  send_config(data, sizeof(data));
}


void send_config(char *data, int count)
{
  set_mode(MODE_CONF);
  delay(100);

  Serial.println("Writing : ");
  print_hex(data, count);
  lora.write(data, count);
  delay(100);

  char buffer[200];
  count = receive(&buffer);

  Serial.println("Reading : ");
  print_hex(buffer, count);

  set_mode(MODE_TRANS);
}


void print_hex(char* buffer, int count) {
  char txt[2];

  for (int i = 0; i < count; ++i)
  {
    sprintf(txt, "%X", buffer[i]);
    Serial.print(txt);
    Serial.print(" ");
  }
  Serial.println();
}

//send bytes & send string ?
void send(char* buffer, int count) {

  //this
  lora.write(buffer, count);

  //or that
  lora.write((byte*)&data, sizeof(data));

  //always
  delay(10);
}

int receive(char* buffer) {

  int count = 0;
  char buffer[200];
  
  while (lora.available())
  {
    buffer[count++] = lora.read();
    delay(1);
  }

  return count;
}

int getRSSI() {
  char data[] = {0xC0, 0xC1, 0xC2, 0xC3, 0x00, 0x02};

  //write
  send(&data, 6);

  int count = receive();

  if (count == 5) {
    current_noise = data[3];
    rssi_last_receive = data[4];
  }
  
  Serial.println("current_noise : %d", current_noise);
  Serial.println("rssi_last_receive : %d", rssi_last_receive);
}