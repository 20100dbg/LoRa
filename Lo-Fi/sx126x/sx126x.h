#ifndef sx126x_h
#define sx126x_h
#include "Arduino.h"
#include "conf.h"

class sx126x
{
  public:
    sx126x();
    void begin();
    void set_config(int addrh = NULL, int addrl = NULL, int network = NULL,
                float air_data_rate = NULL, int sub_packet_size = NULL, 
                bool channel_noise = NULL, int tx_power = NULL, int channel = NULL, 
                bool enable_rssi = NULL, bool transmission_mode = NULL, 
                bool enable_repeater = NULL, bool enable_lbt = NULL,
                int wor_control = NULL, int wor_cycle = NULL, int crypth = NULL,
                int cryptl = NULL, bool write = true);
    
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
    void write_serial(char *data, char *result, int count);

    s_param params;
    bool _debug;
};

#endif