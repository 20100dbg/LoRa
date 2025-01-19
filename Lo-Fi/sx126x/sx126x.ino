#include "sx126x.h"
//#define mySerial Serial2
//HardwareSerial
//SoftwareSerial

sx126x::sx126x() {

  //#define lora Serial2
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


void sx126x::set_mode(int mode) {
  
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

void sx126x::config(char* data) {
  set_mode(MODE_CONF);
  
  lora.write((byte*)&data, sizeof(data));
  delay(10);

  set_mode(MODE_TRANS);
}


void sx126x::set_config(int addrh, int addrl, int netid,
                int serial_port_rate, int serial_parity_bit, int air_data_rate,
                int sub_packet_size, int channel_noise, int tx_power, 
                int channel, int enable_rssi, int transmission_mode, 
                int enable_repeater, int enable_lbt,
                int wor_control, int wor_cycle, int crypth, int cryptl) {

  char settings[12] = {0xC0, 0x00, 0x09};
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

  //write registers
}

void sx126x::read_config()
{
  char data[] = {0xC1, 0x00, 0x09};
  send_config(data, sizeof(data));
}


void sx126x::send_config(char *data, int count)
{
  set_mode(MODE_CONF);
  delay(100);

  Serial.println("Writing : ");
  print_hex(data, count);
  lora.write(data, count);
  delay(100);

  char buffer[200];
  count = receive(buffer);

  Serial.println("Reading : ");
  print_hex(buffer, count);

  set_mode(MODE_TRANS);
}


void sx126x::print_hex(char* buffer, int count) {
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
void sx126x::send(char* buffer, int count) {

  //this
  lora.write(buffer, count);

  //or that
  //lora.write((byte*)&data, sizeof(data));

  //always
  delay(10);
}

int sx126x::receive(char* buffer) {

  int count = 0;

  //char buffer[200];
  //char* buffer = malloc(200 * sizeof(char));
  
  while (lora.available())
  {
    buffer[count++] = lora.read();
    delay(1);
  }

  return count;
}

int sx126x::getRSSI() {
  char data[] = {0xC0, 0xC1, 0xC2, 0xC3, 0x00, 0x02};

  //write
  send(data, 6);
  char buffer[200];

  int count = receive(buffer);
  int current_noise = -1, rssi_last_receive = -1;
  if (count == 5) {
    current_noise = buffer[3];
    rssi_last_receive = buffer[4];
  }
  
  Serial.print("\ncurrent_noise :");
  Serial.print(current_noise);
  Serial.print("\nrssi_last_receive :");
  Serial.print(rssi_last_receive);
}
