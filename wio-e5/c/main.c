#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net/lora.h"
#include "net/netdev/lora.h"
#include "sx126x_params.h"
#include "sx126x_netdev.h"
#include "ztimer.h"
#include "stdio_base.h"

#define SX126X_MSG_QUEUE        (8U)
#define SX126X_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)
#define SX126X_MSG_TYPE_ISR     (0x3456)
#define SX126X_MAX_PAYLOAD_LEN  (128U)


static char stack[SX126X_STACKSIZE];
static kernel_pid_t _recv_pid;
static char message[SX126X_MAX_PAYLOAD_LEN];
static sx126x_t sx126x;


struct s_config {
    uint32_t frequency;
    uint16_t bandwith;
    uint8_t spreading_factor;
    uint8_t coding_rate;
    bool integrity_check;
    uint16_t preamble;
    uint8_t payload_length;
    int8_t tx_power;
};



//Called every time we receive a LoRa packet
static void receive_callback(netdev_t *netdev, char *message, unsigned int len, 
                            int rssi, int snr, long unsigned int toa)
{

    int type = message[0];

    if (type == 1) {
        message[0] = 2;
        send(netdev, message, 4);
    }
    else {
        stdio_write(message, len);
    }

}


static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;
        msg.type = SX126X_MSG_TYPE_ISR;
        msg_send(&msg, _recv_pid);
    }
    else if (event == NETDEV_EVENT_RX_COMPLETE)
    {
        size_t len = dev->driver->recv(dev, NULL, 0, 0);
        netdev_lora_rx_info_t packet_info;
        dev->driver->recv(dev, message, len, &packet_info);

        receive_callback(dev, message, len, packet_info.rssi, packet_info.snr,
                         sx126x_get_lora_time_on_air_in_ms(&sx126x.pkt_params,
                                                           &sx126x.mod_params));

        netopt_state_t state = NETOPT_STATE_RX;
        dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));
    }
}

void *_recv_thread(void *arg)
{
    netdev_t *netdev = arg;
    static msg_t _msg_queue[SX126X_MSG_QUEUE];
    msg_init_queue(_msg_queue, SX126X_MSG_QUEUE);

    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == SX126X_MSG_TYPE_ISR) {
            netdev->driver->isr(netdev);
        }
    }
}


//Write settings from a s_config struct (no value check)
static int write_config(netdev_t *netdev, struct s_config config)
{

    netdev->driver->set(netdev, NETOPT_CHANNEL_FREQUENCY, &config.frequency, sizeof(uint32_t));

    uint8_t bw_val = 0;
    switch (config.bandwith) {
        case 125: bw_val = LORA_BW_125_KHZ; break;
        case 250: bw_val = LORA_BW_250_KHZ; break;
        case 500: bw_val = LORA_BW_500_KHZ; break;
        default: bw_val = LORA_BW_125_KHZ; break;
    }

    netdev->driver->set(netdev, NETOPT_BANDWIDTH, &bw_val, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_SPREADING_FACTOR, &config.spreading_factor, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_CODING_RATE, &config.coding_rate, sizeof(uint8_t));
    netopt_enable_t enable = config.integrity_check ? NETOPT_ENABLE : NETOPT_DISABLE;
    netdev->driver->set(netdev, NETOPT_INTEGRITY_CHECK, &enable, sizeof(netopt_enable_t));
    netdev->driver->set(netdev, NETOPT_PREAMBLE_LENGTH, &config.preamble, sizeof(uint16_t));
    netdev->driver->set(netdev, NETOPT_PDU_SIZE, &config.payload_length, sizeof(uint8_t));
    netdev->driver->set(netdev, NETOPT_TX_POWER, &config.tx_power, sizeof(int8_t));

    return 0;
}

//Send LoRa packet
int send(netdev_t *netdev, char *data, int count)
{
    //désactive l'écoute pour permettre la transmission
    netopt_state_t state = NETOPT_STATE_STANDBY;
    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));
    ztimer_sleep(ZTIMER_MSEC, 50);

    iolist_t iolist = {
        .iol_base = data,
        .iol_len = count
    };

    if (netdev->driver->send(netdev, &iolist) == -ENOTSUP) {
        return -1;
    }

    ztimer_sleep(ZTIMER_MSEC, 50);
    state = NETOPT_STATE_IDLE;
    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));
    return 0;
}




//Read current LoRa settings into a s_config struct
static int read_config(netdev_t *netdev, struct s_config *config)
{
    uint8_t bw;
    netdev->driver->get(netdev, NETOPT_BANDWIDTH, &bw, sizeof(uint8_t));
    netopt_enable_t crc;
    netdev->driver->get(netdev, NETOPT_INTEGRITY_CHECK, &crc, sizeof(netopt_enable_t));
    netdev->driver->get(netdev, NETOPT_CHANNEL_FREQUENCY, &config->frequency, sizeof(uint32_t));
    netdev->driver->get(netdev, NETOPT_SPREADING_FACTOR, &config->spreading_factor, sizeof(uint8_t));
    netdev->driver->get(netdev, NETOPT_CODING_RATE, &config->coding_rate, sizeof(uint8_t));
    netdev->driver->get(netdev, NETOPT_PREAMBLE_LENGTH, &config->preamble, sizeof(uint16_t));
    netdev->driver->get(netdev, NETOPT_PDU_SIZE, &config->payload_length, sizeof(uint8_t));
    netdev->driver->get(netdev, NETOPT_TX_POWER, &config->tx_power, sizeof(int8_t));

    switch (bw) {
    case LORA_BW_125_KHZ: config->bandwith = 125; break;
    case LORA_BW_250_KHZ: config->bandwith = 250; break;
    case LORA_BW_500_KHZ: config->bandwith = 500; break;
    default: config->bandwith = 0; break;
    }
    config->integrity_check = (crc == NETOPT_ENABLE);
    return 0;
}


//Print settings inside a s_config struct
static void print_config(struct s_config config)
{
    printf("---------------\n");
    printf("frequency=%ld\n", config.frequency);
    printf("bandwith=%d\n", config.bandwith);
    printf("spreading_factor=%d\n", config.spreading_factor);
    printf("coding_rate=%d\n", config.coding_rate);
    printf("integrity_check=%s\n", config.integrity_check ? "enabled":"disabled");
    printf("preamble=%d\n", config.preamble);
    printf("payload_length=%d\n", config.payload_length);
    printf("tx_power=%d\n", config.tx_power);
    printf("---------------\n");
}



int main(void)
{
    stdio_init();

    sx126x_setup(&sx126x, &sx126x_params[0], 0);
    netdev_t *netdev = &sx126x.netdev;
    netdev->driver = &sx126x_driver;

    netdev->event_callback = _event_cb;

    if (netdev->driver->init(netdev) < 0) {
        return 1;
    }

    struct s_config config;

    if (true) {
        config.frequency = 410000000; 
        config.bandwith = 250;
        config.spreading_factor = 7;
        config.coding_rate = 1;
        config.integrity_check = true;
        config.preamble = 10;
        config.payload_length = 32;
        config.tx_power = 22;

        write_config(netdev, config);        
        //read_config(netdev, &config);
        //print_config(config);
    }
    

    //enable listening
    netopt_state_t state = NETOPT_STATE_IDLE;
    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));

    //start listening thread
    _recv_pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1, 0, _recv_thread, netdev, "recv_thread");

    if (_recv_pid <= KERNEL_PID_UNDEF) {
        return 1;
    }

    size_t max_size = 256;
    char buffer[max_size];

    size_t current_size = 0;
    uint32_t timestamp = 0;



    while (1) {

        //read stdin and send it with LoRa

        while (stdio_available()) {
            char c;
            ssize_t count = stdio_read(&c, 1);
            buffer[current_size] = c;
            current_size += count;
            timestamp = ztimer_now(ZTIMER_MSEC);
        }


        if (current_size > 0 && ztimer_now(ZTIMER_MSEC) - timestamp > 10) {
            send(netdev, buffer, current_size);
            current_size = 0;
        }

    }

    return 0;
}
