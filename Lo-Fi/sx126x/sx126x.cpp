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
  struct s_param read_params;
  read_params = read_registers(result);

  //fill local vars
  set_config(read_params.addrh, read_params.addrl, read_params.network,
              read_params.air_data_rate, read_params.sub_packet_size, 
              read_params.channel_noise, read_params.tx_power, 
              read_params.channel, read_params.enable_rssi,
              read_params.transmission_mode, read_params.enable_repeater,
              read_params.enable_lbt, read_params.wor_control,
              read_params.wor_cycle, read_params.crypth,
              read_params.cryptl, false);
  
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
  delay(10);
}

void sx126x::set_config(int addrh, int addrl, int network,
                float air_data_rate, int sub_packet_size, 
                bool channel_noise, int tx_power, int channel, 
                bool enable_rssi, bool transmission_mode, 
                bool enable_repeater, bool enable_lbt,
                int wor_control, int wor_cycle, int crypth,
                int cryptl, bool write)
{

  if (addrh != NULL) params.addrh;
  if (addrl != NULL) params.addrl;
  if (network != NULL) params.network;
  if (air_data_rate != NULL) params.air_data_rate;
  if (sub_packet_size != NULL) params.sub_packet_size;
  if (channel_noise != NULL) params.channel_noise;
  if (tx_power != NULL) params.tx_power;
  if (channel != NULL) params.channel;
  if (enable_rssi != NULL) params.enable_rssi;
  if (transmission_mode != NULL) params.transmission_mode;
  if (enable_repeater != NULL) params.enable_repeater;
  if (enable_lbt != NULL) params.enable_lbt;
  if (wor_control != NULL) params.wor_cycle;
  if (crypth != NULL) params.crypth;
  if (cryptl != NULL) params.cryptl;
  
  /*
  if (logical_address != NULL) {

  }
  */


  if (write) {
    write_registers();
  }
}

s_param sx126x::read_registers(char* result)
{
  char data[] = {0xC1, 0x00, 0x09};
  write_serial(data, result, sizeof(data));
  struct s_param read_params;

  read_params.addrh = result[3];
  read_params.addrl = result[4];
  read_params.network = result[5];

  int serial_port_rate=0;
  int serial_parity_bit=0;
  //float air_data_rate=0;

  //int sub_packet_size=0;
  //bool channel_noise=false;
  //int tx_power=0;

  read_params.channel = result[8];

  //bool enable_rssi = false;
  //bool transmission_mode = false;
  //bool enable_repeater = false;
  //bool enable_lbt = false;
  //bool wor_control = false;
  //int wor_cycle = 0;

  read_params.crypth = result[10];
  read_params.cryptl = result[11];

  for (int i = 0; i < 8; i++) if ((result[6] & 0b11100000) == SERIAL_PORT_RATE[i][1]) serial_port_rate = SERIAL_PORT_RATE[i][0];
  for (int i = 0; i < 3; i++) if ((result[6] & 0b00011000) == SERIAL_PARITY_BIT[i][1]) serial_parity_bit = SERIAL_PARITY_BIT[i][0];  
  for (int i = 0; i < 8; i++) if ((result[6] & 0b00000111) == AIR_DATA_RATE[i][1]) read_params.air_data_rate = AIR_DATA_RATE[i][0];
  for (int i = 0; i < 4; i++) if ((result[7] & 0b11000000) == SUB_PACKET_SIZE[i][1]) read_params.sub_packet_size = SUB_PACKET_SIZE[i][0];
  for (int i = 0; i < 2; i++) if ((result[7] & 0b00100000) == CHANNEL_NOISE[i][1]) read_params.channel_noise = (CHANNEL_NOISE[i][0] == 1);  
  for (int i = 0; i < 4; i++) if ((result[7] & 0b00000011) == TX_POWER[i][1]) read_params.tx_power = TX_POWER[i][0];
  for (int i = 0; i < 2; i++) if ((result[9] & 0b10000000) == ENABLE_RSSI[i][1]) read_params.enable_rssi = (ENABLE_RSSI[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b01000000) == TRANSMISSION_MODE[i][1]) read_params.transmission_mode = (TRANSMISSION_MODE[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b00100000) == ENABLE_REPEATER[i][1]) read_params.enable_repeater = (ENABLE_REPEATER[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b00010000) == ENABLE_LBT[i][1]) read_params.enable_lbt = (ENABLE_LBT[i][0] == 1);
  for (int i = 0; i < 2; i++) if ((result[9] & 0b00001000) == WOR_CONTROL[i][1]) read_params.wor_control = (WOR_CONTROL[i][0] == 1);
  for (int i = 0; i < 8; i++) if ((result[9] & 0b00000111) == WOR_CYCLE[i][1]) read_params.wor_cycle = WOR_CYCLE[i][0];

  Serial.println("\n\nread config");
  print_hex(result, 12);
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
  write_serial(settings, result, sizeof(settings));

  if (result[0] == 0xff && result[1] == 0xff && result[2] == 0xff) {
    //erreur
  }

}

void sx126x::write_serial(char *data, char *result, int count)
{
  set_mode(MODE_CONF);

  if (_debug) {
    Serial.println("Writing : ");
    print_hex(data, count);
  }
  
  serial_lora.write(data, count);
  delay(20);

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


s_rssi sx126x::get_rssi() {
  char data[] = {0xC0, 0xC1, 0xC2, 0xC3, 0x00, 0x02};

  //write
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
}