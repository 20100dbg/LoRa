#ifndef sx126x_h
#define sx126x_h
#include "Arduino.h"
#include "conf.h"

class sx126x
{
  public:
    sx126x();
    void begin();
    
    bool set_address(int address);
    bool set_network(int network);
    bool set_air_data_rate(float air_data_rate);
    bool set_tx_power(int tx_power);
    bool set_sub_packet_size(int sub_packet_size);
    bool set_channel(int channel);
    bool set_channel_noise(bool channel_noise);
    bool set_enable_rssi(bool enable_rssi);
    bool set_crypt_key(int key);
    
    bool get_channel_noise();
    bool get_enable_rssi();

    void print_hex(char* buffer, int count);
    void send(char* buffer, int count);
    int receive(char* buffer);
    s_rssi get_rssi();
    void print_config();

  private:
    #define serial_lora Serial2 

    void set_mode(int mode);
    void config(char* data);
    void send_config(char* data, int count);
    s_param read_registers(char* result);
    void write_registers();
    int write_serial(char *data, int count, char *result);

    s_param params;
    bool _debug;
};

#endif