
const int M0 = 47;
const int M1 = 48;
const int MODE_TRANS = 0;
const int MODE_CONF = 1;
const int MODE_WOR = 2;
const int MODE_SLEEP = 3;

const int SERIAL_PORT_RATE_1200 = 0;
const int SERIAL_PORT_RATE_2400 = 0b00100000;
const int SERIAL_PORT_RATE_4800 = 0b01000000;
const int SERIAL_PORT_RATE_9600 = 0b01100000;
const int SERIAL_PORT_RATE_19200 = 0b10000000;
const int SERIAL_PORT_RATE_38400 = 0b10100000;
const int SERIAL_PORT_RATE_57600 = 0b11000000;
const int SERIAL_PORT_RATE_115200 = 0b11100000;

const int SERIAL_PARITY_BIT_8N1 = 0;
const int SERIAL_PARITY_BIT_8O1 = 0b00001000;
const int SERIAL_PARITY_BIT_8E1 = 0b00010000;

const int AIR_DATA_RATE_0_3 = 0;
const int AIR_DATA_RATE_1_2 = 0b00000001;
const int AIR_DATA_RATE_2_4 = 0b00000010;
const int AIR_DATA_RATE_4_8 = 0b00000011;
const int AIR_DATA_RATE_9_6 = 0b00000100;
const int AIR_DATA_RATE_19_2 = 0b00000101;
const int AIR_DATA_RATE_38_4 = 0b00000110;
const int AIR_DATA_RATE_62_5 = 0b00000111;

const int SUB_PACKET_SIZE_240 = 0;
const int SUB_PACKET_SIZE_128 = 0b01000000;
const int SUB_PACKET_SIZE_64 = 0b10000000;
const int SUB_PACKET_SIZE_32 = 0b11000000;

const int CHANNEL_NOISE_DISABLED = 0;
const int CHANNEL_NOISE_ENABLED = 0b00100000;

const int TX_POWER_22 = 0;
const int TX_POWER_17 = 0b00000001;
const int TX_POWER_13 = 0b00000010;
const int TX_POWER_10 = 0b00000011;

const int ENABLE_RSSI_DISABLED = 0;
const int ENABLE_RSSI_ENABLED = 0b10000000;

const int TRANSMISSION_MODE_FIXED = 0b01000000;
const int TRANSMISSION_MODE_TRANSPARENT = 0;

const int ENABLE_REPEATER_DISABLED = 0;
const int ENABLE_REPEATER_ENABLED = 0b00100000;

const int ENABLE_LBT_DISABLED = 0;
const int ENABLE_LBT_ENABLED = 0b00010000;

const int WOR_CONTROL_TRANSMITTER = 0b00001000;
const int WOR_CONTROL_RECEIVER = 0;

const int WOR_CYCLE_500 = 0;
const int WOR_CYCLE_1000 = 0b00000001;
const int WOR_CYCLE_1500 = 0b00000010;
const int WOR_CYCLE_2000 = 0b00000011;
const int WOR_CYCLE_2500 = 0b00000100;
const int WOR_CYCLE_3000 = 0b00000101;
const int WOR_CYCLE_3500 = 0b00000110;
const int WOR_CYCLE_4000 = 0b00000111;
