#ifndef sx126x_h
#define sx126x_h
#include "Arduino.h"
#include "conf.h"

class sx126x
{
  public:
    sx126x();
    
    void set_config(int addrh, int addrl, int netid,
                int serial_port_rate, int serial_parity_bit, int air_data_rate,
                int sub_packet_size, int channel_noise, int tx_power, 
                int channel, int enable_rssi, int transmission_mode, 
                int enable_repeater, int enable_lbt,
                int wor_control, int wor_cycle, int crypth, int cryptl);
    void read_config();
    void print_hex(char* buffer, int count);
    void send(char* buffer, int count);
    int receive(char* buffer);
    int getRSSI();

  private:
    #define lora Serial2 

    void set_mode(int mode);
    void config(char* data);
    void send_config(char* data, int count);

};

#endif