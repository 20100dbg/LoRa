#include "sx126x.h"
//#define mySerial Serial2

sx126x::sx126x() {
  _debug = true;
}

void sx126x::begin() {

  Serial.begin(115200);
  while (!Serial) {}

  serial_lora.begin(9600, SERIAL_8N1, 18, 17); //9600 mandatory

  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  set_mode(MODE_TRANS);
  
  //very important to read here
  delay(10);
  while (serial_lora.available()) serial_lora.read();

  //read current settings
  char result[12];
  read_registers(result);

  //fill local vars
  //set_config(write_registers=false);
  
}


void sx126x::send(char* buffer, int count) {
  serial_lora.write(buffer, count);
  //serial_lora.write((byte*)&data, sizeof(data));
  delay(10);
}

int sx126x::receive(char* buffer) {
  int count = 0;
  while (serial_lora.available()) {
    buffer[count++] = serial_lora.read();
    delay(1);
  }
  return count;
}

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

void sx126x::set_config(int addrh, int addrl, int network,
                float air_data_rate, int sub_packet_size, 
                bool channel_noise, int tx_power, int channel, 
                bool enable_rssi, bool transmission_mode, 
                bool enable_repeater, bool enable_lbt,
                int wor_control, int wor_cycle, int crypth, int cryptl,
                bool write_registers)
{
  //write_serial();
}

void sx126x::read_registers(char* result)
{
  char data[] = {0xC1, 0x00, 0x09};
  write_serial(data, result, sizeof(data));
  
  int addrh = result[3];
  int addrl = result[4];
  int network = result[5];

  int serial_port_rate=0;
  int serial_parity_bit=0;  
  float air_data_rate=0;

  int sub_packet_size=0;
  bool channel_noise=false;
  int tx_power=0;

  int channel = result[8];

  bool enable_rssi = false;
  bool transmission_mode = false;
  bool enable_repeater = false;
  bool enable_lbt = false;
  bool wor_control = false;
  int wor_cycle = 0;

  int crypth = result[10];
  int cryptl = result[11];

  for (int i = 0; i < 8; i++) if ((result[6] & 0b11100000) == SERIAL_PORT_RATE[i][1]) serial_port_rate = SERIAL_PORT_RATE[i][0];
  for (int i = 0; i < 3; i++) if ((result[6] & 0b00011000) == SERIAL_PARITY_BIT[i][1]) serial_parity_bit = SERIAL_PARITY_BIT[i][0];  
  for (int i = 0; i < 8; i++) if ((result[6] & 0b00000111) == AIR_DATA_RATE[i][1]) air_data_rate = AIR_DATA_RATE[i][0];

  for (int i = 0; i < 4; i++) if ((result[7] & 0b11000000) == SUB_PACKET_SIZE[i][1]) sub_packet_size = SUB_PACKET_SIZE[i][0];
  for (int i = 0; i < 2; i++) if ((result[7] & 0b00100000) == CHANNEL_NOISE[i][1]) channel_noise = (CHANNEL_NOISE[i][0] == 1);  
  for (int i = 0; i < 4; i++) if ((result[7] & 0b00000011) == TX_POWER[i][1]) tx_power = TX_POWER[i][0];

  for (int i = 0; i < 2; i++) if ((result[9] & 0b10000000) == ENABLE_RSSI[i][1]) enable_rssi = (ENABLE_RSSI[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b01000000) == TRANSMISSION_MODE[i][1]) transmission_mode = (TRANSMISSION_MODE[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b00100000) == ENABLE_REPEATER[i][1]) enable_repeater = (ENABLE_REPEATER[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b00010000) == ENABLE_LBT[i][1]) enable_lbt = (ENABLE_LBT[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b00001000) == WOR_CONTROL[i][1]) wor_control = (WOR_CONTROL[i][0] == 1);
  for (int i = 0; i < 8; i++) if ((result[9] & 0b00000111) == WOR_CYCLE[i][1]) wor_cycle = WOR_CYCLE[i][0];

  Serial.print("\nserial_port_rate ");
  Serial.print(serial_port_rate);
  Serial.print("\nserial_parity_bit ");
  Serial.print(serial_parity_bit);
  Serial.print("\nair_data_rate ");
  Serial.print(air_data_rate);

  Serial.print("\nsub_packet_size ");
  Serial.print(sub_packet_size);
  Serial.print("\nchannel_noise ");
  Serial.print(channel_noise);
  Serial.print("\ntx_power ");
  Serial.print(tx_power);

  Serial.print("\nenable_rssi ");
  Serial.print(enable_rssi);
  Serial.print("\ntransmission_mode ");
  Serial.print(transmission_mode);
  Serial.print("\nenable_repeater ");
  Serial.print(enable_repeater);
  Serial.print("\nenable_lbt ");
  Serial.print(enable_lbt);
  Serial.print("\nwor_control ");
  Serial.print(wor_control);
  Serial.print("\nwor_cycle ");
  Serial.print(wor_cycle);

  
  Serial.println("\n\nread config");
  print_hex(result, 12);
}

void sx126x::write_registers()
{
  char settings[12] = {0xC0, 0x00, 0x09};
  int reserve = 0;
/*
  settings[3] = addrh;
  settings[4] = addrl;
  settings[5] = network;
  settings[6] = serial_port_rate + serial_parity_bit + air_data_rate;
  settings[7] = sub_packet_size + channel_noise + reserve + tx_power;
  settings[8] = channel;
  settings[9] = enable_rssi + transmission_mode + enable_repeater + 
                enable_lbt + wor_control + wor_cycle;
  settings[10] = crypth;
  settings[11] = cryptl;
*/
  char result[12];
  write_serial(settings, result, sizeof(settings));
}

void sx126x::write_serial(char *data, char *result, int count)
{
  set_mode(MODE_CONF);
  delay(10);

  if (_debug) {
    Serial.println("Writing : ");
    print_hex(data, count);
  }
  
  serial_lora.write(data, count);
  delay(50);

  count = receive(result);

  if (_debug) {
    Serial.println("Reading : ");
    print_hex(result, count);
  }

  set_mode(MODE_TRANS);
}

void sx126x::print_hex(char* buffer, int count) {
  char txt[2];

  for (int i = 0; i < count; ++i) {
    sprintf(txt, "%X", buffer[i]);
    Serial.print(txt);
    Serial.print(" ");
  }
  Serial.println();
}


int sx126x::get_rssi() {
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
