import sx126x
import threading
import time

def calculate_checksum(data):
    CK_A, CK_B = 0, 0

    for i in range(len(data)):
        CK_A = CK_A + data[i]
        CK_B = CK_B + CK_A

    return (CK_A & 0xFF, CK_B & 0xFF)


def listener():
    while is_running:
        data = lora.receive()
        if data:
            print(data)
        time.sleep(0.2)


#LoRa settings
air_data_rate=62.5
sub_packet_size=240
tx_power=10
delay = 0.1
peer_id = 0
header_size = 6
footer_size = 2

lora = sx126x.sx126x(debug=True)
lora.set_config(air_data_rate=air_data_rate, sub_packet_size=sub_packet_size, tx_power=tx_power)
is_running = True

t_receive = threading.Thread(target=listener)
t_receive.start()


for msg_id in range(1000):
    payload_size = sub_packet_size - header_size - footer_size
    payload = "A".encode() * payload_size
    checksum = calculate_checksum(payload)

    msg = b"\xB5\x62"
    msg += peer_id.to_bytes(1, "big")
    msg += msg_id.to_bytes(2, "big")
    msg += payload_size.to_bytes(1, "big")
    msg += payload
    msg += checksum[0].to_bytes(1, "big")
    msg += checksum[1].to_bytes(1, "big")

    lora.send_bytes(msg)

    print(f"\rEnvoi {msg_id}", end="")
    time.sleep(delay)


t_receive.join()
lora.close()