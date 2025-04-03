#include "sx126x.h"

//todo
//params.serial_port_rate = 9600;

sx126x::sx126x() {
  _debug = true;
}

void sx126x::begin() {

  Serial.begin(115200);
  while (!Serial) {}

  params.serial_port_rate = 115200;
  serial_lora.begin(params.serial_port_rate, SERIAL_8N1, 18, 17);

  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  //set_mode(MODE_TRANS);
  
  //very important to read here
  delay(10);
  while (serial_lora.available()) serial_lora.read();

  //read current settings
  char result[12];
  //struct s_param read_params;
  params = read_registers(result);
  
  if (_debug) {
    print_config();
  }
}

void sx126x::send(char* buffer, int count) {

  if (_debug) {
    Serial.print("[+] Sending : ");
    print_hex(buffer, count);
  }

  serial_lora.write(buffer, count);
  delay(10);
}

int sx126x::receive(char* buffer) {
  int count = 0;
  while (serial_lora.available()) {
    buffer[count++] = serial_lora.read();
    delay(1);
  }

  if (_debug) {
    Serial.print("[+] Received : ");
    print_hex(buffer, count);
  }

  return count;
}

void sx126x::set_mode(int mode) {
  
  if (_debug) {
    Serial.print("[+] Set mode : ");
    if (mode == MODE_CONF) Serial.println("CONF");
    if (mode == MODE_TRANS) Serial.println("TRANS");
    if (mode == MODE_WOR) Serial.println("WOR");
    if (mode == MODE_SLEEP) Serial.println("SLEEP");
  }

  if (mode == MODE_CONF) {
    digitalWrite(M0, LOW);
    digitalWrite(M1, HIGH);
  }
  else if (mode == MODE_WOR) {
    digitalWrite(M0, HIGH);
    digitalWrite(M1, LOW);
  }
  else if (mode == MODE_SLEEP) {
    digitalWrite(M0, HIGH);
    digitalWrite(M1, HIGH);
  }
  else {
    digitalWrite(M0, LOW);
    digitalWrite(M1, LOW);
  }
  delay(100);
}


bool sx126x::set_address(int address)
{
  if (address < 0 || address > 65535)
    return false;

  params.addrh = address >> 8;
  params.addrl = address & 255;
  write_registers();
  return true;
}


bool sx126x::set_network(int network)
{
  if (network < 0 || network > 255)
    return false;

  params.network = network;
  write_registers();
  return true;
}

bool sx126x::set_air_data_rate(float air_data_rate)
{
  if (air_data_rate != 0.3 && air_data_rate != 1.2 && air_data_rate != 2.4 &&
    air_data_rate != 4.8 && air_data_rate != 9.6 && air_data_rate != 19.2 && 
    air_data_rate != 38.4 && air_data_rate != 62.5)
    return false;

  params.air_data_rate = air_data_rate;
  write_registers();
  return true;
}

bool sx126x::set_tx_power(int tx_power)
{
  if (tx_power != 10 && tx_power != 13 && tx_power != 17 && tx_power != 22)
    return false;

  params.tx_power = tx_power;
  write_registers();
  return true;
}

bool sx126x::set_sub_packet_size(int sub_packet_size)
{
  if (sub_packet_size != 240 && sub_packet_size != 128 &&
      sub_packet_size != 64 && sub_packet_size != 32)
    return false;

  params.sub_packet_size = sub_packet_size;
  write_registers();
  return true;
}

bool sx126x::set_channel(int channel)
{
 if (channel < 0 || channel > 80)
    return false;

  params.channel = channel;
  write_registers();
  return true;
}

bool sx126x::set_channel_noise(bool channel_noise)
{
  params.channel_noise = channel_noise;
  write_registers();
  return true;
}

bool sx126x::set_rssi(bool enable_rssi)
{
  params.enable_rssi = enable_rssi;
  write_registers();
  return true;
}

bool sx126x::set_crypt_key(int key)
{
 if (key < 0 || key > 65535)
    return false;

  params.crypth = key >> 8;
  params.cryptl = key & 255;
  write_registers();
  return true;
}


s_param sx126x::read_registers(char* result)
{
  char data[] = {0xC1, 0x00, 0x09};
  write_serial(data, result, 3);
  struct s_param read_params;

  read_params.addrh = result[3];
  read_params.addrl = result[4];
  read_params.network = result[5];
  read_params.channel = result[8];
  read_params.crypth = result[10];
  read_params.cryptl = result[11];

  for (int i = 0; i < 8; i++) if ((result[6] & 0b11100000) == SERIAL_PORT_RATE[i][1]) read_params.serial_port_rate = SERIAL_PORT_RATE[i][0];
  for (int i = 0; i < 3; i++) if ((result[6] & 0b00011000) == SERIAL_PARITY_BIT[i][1]) read_params.serial_parity_bit = SERIAL_PARITY_BIT[i][0];  
  for (int i = 0; i < 8; i++) if ((result[6] & 0b00000111) == AIR_DATA_RATE[i][1]) read_params.air_data_rate = ((float)AIR_DATA_RATE[i][0] / 10);
  for (int i = 0; i < 4; i++) if ((result[7] & 0b11000000) == SUB_PACKET_SIZE[i][1]) read_params.sub_packet_size = SUB_PACKET_SIZE[i][0];
  for (int i = 0; i < 2; i++) if ((result[7] & 0b00100000) == CHANNEL_NOISE[i][1]) read_params.channel_noise = (CHANNEL_NOISE[i][0] == 1);  
  for (int i = 0; i < 4; i++) if ((result[7] & 0b00000011) == TX_POWER[i][1]) read_params.tx_power = TX_POWER[i][0];
  for (int i = 0; i < 2; i++) if ((result[9] & 0b10000000) == ENABLE_RSSI[i][1]) read_params.enable_rssi = (ENABLE_RSSI[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b01000000) == TRANSMISSION_MODE[i][1]) read_params.transmission_mode = (TRANSMISSION_MODE[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b00100000) == ENABLE_REPEATER[i][1]) read_params.enable_repeater = (ENABLE_REPEATER[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b00010000) == ENABLE_LBT[i][1]) read_params.enable_lbt = (ENABLE_LBT[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b00001000) == WOR_CONTROL[i][1]) read_params.wor_control = (WOR_CONTROL[i][0] == 1);
  for (int i = 0; i < 8; i++) if ((result[9] & 0b00000111) == WOR_CYCLE[i][1]) read_params.wor_cycle = WOR_CYCLE[i][0];

  return read_params;
}


void sx126x::write_registers()
{
  char settings[12] = {0xC0, 0x00, 0x09};
  int reserve = 0;

  struct s_param write_params;

  for (int i = 0; i < 8; i++) if (params.serial_port_rate == SERIAL_PORT_RATE[i][0]) write_params.serial_port_rate = SERIAL_PORT_RATE[i][1];
  for (int i = 0; i < 3; i++) if (params.serial_parity_bit == SERIAL_PARITY_BIT[i][0]) write_params.serial_parity_bit = SERIAL_PARITY_BIT[i][1];  
  for (int i = 0; i < 8; i++) if (params.air_data_rate == AIR_DATA_RATE[i][0]) write_params.air_data_rate = AIR_DATA_RATE[i][1];
  for (int i = 0; i < 4; i++) if (params.sub_packet_size == SUB_PACKET_SIZE[i][0]) write_params.sub_packet_size = SUB_PACKET_SIZE[i][1];
  for (int i = 0; i < 2; i++) if (params.channel_noise == CHANNEL_NOISE[i][0]) write_params.channel_noise = (CHANNEL_NOISE[i][1] == 1);  
  for (int i = 0; i < 4; i++) if (params.tx_power == TX_POWER[i][0]) write_params.tx_power = TX_POWER[i][1];
  for (int i = 0; i < 2; i++) if (params.enable_rssi == ENABLE_RSSI[i][0]) write_params.enable_rssi = (ENABLE_RSSI[i][1] == 1);
  for (int i = 0; i < 2; i++) if (params.transmission_mode == TRANSMISSION_MODE[i][0]) write_params.transmission_mode = (TRANSMISSION_MODE[i][1] == 1);
  for (int i = 0; i < 2; i++) if (params.enable_repeater == ENABLE_REPEATER[i][0]) write_params.enable_repeater = (ENABLE_REPEATER[i][1] == 1);
  for (int i = 0; i < 2; i++) if (params.enable_lbt == ENABLE_LBT[i][0]) write_params.enable_lbt = (ENABLE_LBT[i][1] == 1);
  for (int i = 0; i < 2; i++) if (params.wor_control == WOR_CONTROL[i][0]) write_params.wor_control = (WOR_CONTROL[i][1] == 1);
  for (int i = 0; i < 8; i++) if (params.wor_cycle == WOR_CYCLE[i][0]) write_params.wor_cycle = WOR_CYCLE[i][1];

  settings[3] = write_params.addrh;
  settings[4] = write_params.addrl;
  settings[5] = write_params.network;
  settings[6] = write_params.serial_port_rate + write_params.serial_parity_bit + 
                write_params.air_data_rate;
  settings[7] = write_params.sub_packet_size + write_params.channel_noise + 
                reserve + write_params.tx_power;
  settings[8] = write_params.channel;
  settings[9] = write_params.enable_rssi + write_params.transmission_mode + 
                write_params.enable_repeater + write_params.enable_lbt + 
                write_params.wor_control + write_params.wor_cycle;
  settings[10] = write_params.crypth;
  settings[11] = write_params.cryptl;

  char result[12];
  write_serial(settings, result, 12);

  if (result[0] == 0xff && result[1] == 0xff && result[2] == 0xff) {
    Serial.println("[-] ERROR WRITING CONFIG");
  }

}

void sx126x::write_serial(char *data, char *result, int count)
{
  set_mode(MODE_CONF);

  if (_debug) {
    Serial.print("Writing : ");
    Serial.print(count);
    Serial.println();

    Serial.print(data[0], HEX);
    Serial.print("-");
    Serial.print(data[1], HEX);
    Serial.print("-");
    Serial.print(data[2], HEX);

    //print_hex(data, count);
    Serial.println();
  }
  
  serial_lora.write(data, count);
  delay(100);

  count = receive(result);

  if (_debug) {
    Serial.print("Reading : ");
    Serial.print(count);
    Serial.println();

    print_hex(result, count);
    Serial.println();
  }

  set_mode(MODE_TRANS);
}

void sx126x::print_hex(char* buffer, int count) {
  
  for (int i = 0; i < count; i++) {
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}


s_rssi sx126x::get_rssi() {
  char data[] = {0xC0, 0xC1, 0xC2, 0xC3, 0x00, 0x02};

  //No conf mode : chip needs to be in transmitting or WOR mode
  send(data, 6);

  char buffer[5];
  int count = receive(buffer);
  
  struct s_rssi rssi;
  rssi.current_noise = buffer[3];
  rssi.rssi_last_receive = buffer[4];

  return rssi;
}



void sx126x::print_config() {
 
  Serial.print("\nserial_port_rate ");
  Serial.print(params.serial_port_rate);
  Serial.print("\nserial_parity_bit ");
  Serial.print(params.serial_parity_bit);
  Serial.print("\nair_data_rate ");
  Serial.print(params.air_data_rate);

  Serial.print("\nsub_packet_size ");
  Serial.print(params.sub_packet_size);
  Serial.print("\nchannel_noise ");
  Serial.print(params.channel_noise);
  Serial.print("\ntx_power ");
  Serial.print(params.tx_power);

  Serial.print("\nenable_rssi ");
  Serial.print(params.enable_rssi);
  Serial.print("\ntransmission_mode ");
  Serial.print(params.transmission_mode);
  Serial.print("\nenable_repeater ");
  Serial.print(params.enable_repeater);
  Serial.print("\nenable_lbt ");
  Serial.print(params.enable_lbt);
  Serial.print("\nwor_control ");
  Serial.print(params.wor_control);
  Serial.print("\nwor_cycle ");
  Serial.print(params.wor_cycle);
  Serial.println();
}