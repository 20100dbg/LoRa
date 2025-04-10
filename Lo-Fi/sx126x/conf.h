const int M0 = 47;
const int M1 = 48;

const int MODE_TRANS = 0;
const int MODE_CONF = 1;
const int MODE_WOR = 2;
const int MODE_SLEEP = 3;


const int SERIAL_PORT_RATE[][2] = {{1200, 0}, {2400, 0b00100000}, {4800, 0b01000000}, {9600, 0b01100000}, {19200, 0b10000000}, {38400, 0b10100000}, {57600, 0b11000000}, {115200, 0b11100000}};
//8N1 = 0, 8O1 = 1, 8E1 = 2
const int SERIAL_PARITY_BIT[][2] = {{0, 0}, {1, 0b00001000}, {2, 0b00010000}};
const int AIR_DATA_RATE[][2] = {{3, 0}, {12, 0b00000001}, {24, 0b00000010}, {48, 0b00000011}, {96, 0b00000100}, {192, 0b00000101}, {384, 0b00000110}, {625, 0b00000111}};
const int SUB_PACKET_SIZE[][2] = {{240, 0}, {128, 0b01000000}, {64, 0b10000000}, {32, 0b11000000}};
const int CHANNEL_NOISE[][2] = {{0, 0}, {1, 0b00100000}};
const int TX_POWER[][2] = {{22, 0}, {17, 0b00000001}, {13, 0b00000010}, {10, 0b00000011}};
const int ENABLE_RSSI[][2] = {{0, 0}, {1, 0b10000000}};
//transparent = 0, fixed = 1
const int TRANSMISSION_MODE[][2] = {{0, 0}, {1, 0b01000000}};
const int ENABLE_REPEATER[][2] = {{0, 0}, {1, 0b00100000}};
const int ENABLE_LBT[][2] = {{0, 0}, {1, 0b00010000}};
//receiver = 0, transmitter = 1
const int WOR_CONTROL[][2] = {{0, 0}, {1, 0b00001000}};
const int WOR_CYCLE[][2] = {{500, 0}, {1000, 0b00000001}, {1500, 0b00000010}, {2000, 0b00000011}, {2500, 0b00000100}, {3000, 0b00000101}, {3500, 0b00000110}, {4000, 0b00000111}};


struct s_param
{
    int serial_port_rate;
    int serial_parity_bit;
    int addrh;
    int addrl;
    int network;
    int air_data_rate;
    int sub_packet_size;
    bool channel_noise;
    int tx_power;
    int channel;
    bool enable_rssi;
    bool transmission_mode;
    bool enable_repeater;
    bool enable_lbt;
    int wor_control;
    int wor_cycle;
    int cryptl;
    int crypth;
};

struct s_rssi
{
    int current_noise;
    int rssi_last_receive;
};