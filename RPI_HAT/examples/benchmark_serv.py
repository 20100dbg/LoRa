import sx126x
import threading
import time

def calculate_checksum(data):
    CK_A, CK_B = 0, 0

    for i in range(len(data)):
        CK_A = CK_A + data[i]
        CK_B = CK_B + CK_A

    return (CK_A & 0xFF, CK_B & 0xFF)

def bytes_to_hex(arr):
    return ' '.join(['{:02X}'.format(b) for b in arr])

def listener():
    while is_running:
        data = lora.receive()
        if data:
            callback_lora(data)
        time.sleep(0.2)


def callback_lora(data):
    global last_msg_id, time_start, received_size

    packet_valid = True

    if data and len(data) >= 6:

        print(f"{bytes_to_hex(data[0:20])}...")
        if data[0:2] == b"\xB5\x62": # and data[-2:] == b"\xC6\x73":
            
            if time_start == 0:
                time_start = time.time()

            peer_id = data[2]
            msg_id = int.from_bytes(data[3:5])

            if msg_id != 0 and msg_id != last_msg_id + 1:
                print(f"\n[-] Missed {last_msg_id-1} !!!!\n")

            payload_size = data[5]
            payload = data[6:payload_size+6]

            if payload_size != len(payload):
                print(f"[-] bad payload size, expected {payload_size}, got {len(payload)}")

            if payload != b"A" * payload_size:
                print("[-] bad payload")
                packet_valid = False

            checksum = data[-2:]
            checksum2 = calculate_checksum(payload)

            if checksum[0] != checksum2[0] or checksum[1] != checksum2[1]:
                print(f"[-] bad checksum, expected {bytes_to_hex(checksum)} got {bytes_to_hex(list(checksum2))}")
                packet_valid = False

            if packet_valid:
                received_size += payload_size

                speed = received_size / (time.time() - time_start)
                print(f"Reçu : {msg_id} / vitesse : {speed}", end="\n")

            last_msg_id = msg_id

        else:
            print("bad footer")

        #if len(data) > 12:
        #    print(f"data left : {data[12:]}")

        #print(f"received {len(data)} : {bytes_to_hex(data)}")


global last_msg_id, time_start, received_size
last_msg_id, time_start, received_size = 0, 0, 0

#LoRa settings
air_data_rate=62.5
sub_packet_size=128
tx_power=10
expected_size = None
byte_index_size = 5

sync_word = b"\xB5\x62"

lora = sx126x.sx126x(port="/dev/ttyS0", baudrate=115200, debug=True)
lora.set_config(air_data_rate=air_data_rate, sub_packet_size=sub_packet_size, tx_power=tx_power)
#lora.set_serial_config(baudrate=115200)

lora.listen(callback=callback_lora, byte_index_size=byte_index_size, sync_word=sync_word)
is_running = True


while True:
    try:
        txt = input()
        lora.send_string(txt)
    except KeyboardInterrupt:
        is_running = False
        break

#t_receive.join()
lora.close()