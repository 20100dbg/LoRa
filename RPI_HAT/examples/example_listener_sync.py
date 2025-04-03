import sx126x
import threading
import time

def bytes_to_str(arr):
    return ' '.join(['{:02X}'.format(b) for b in arr])


def find_sync(data, sync_word, idx_start=0):
    if not data:
        return -1

    for idx in range(idx_start, len(data)):
        if data[idx:idx+len(sync_word)] == sync_word:
            return idx

    return -1


def listen_loop(callback, idx_payload_size, headers_size, sync_word):
    """ Check if data is received """
    
    buffer_receive = b''
    last_receive = 0
    
    while True:
        data = lora.receive()

        if data:
            buffer_receive += data
            last_receive = time.time()

        while True:
            idx_start = find_sync(buffer_receive, sync_word)

            if idx_start >= 0:
                buffer_receive = buffer_receive[idx_start:]

                if len(buffer_receive) > idx_payload_size:
                    expected_size = buffer_receive[idx_payload_size]
                    expected_size += headers_size


                    if len(buffer_receive) >= expected_size:
                        print(f"expected_size {expected_size}")
                        callback(buffer_receive[idx_start:expected_size])
                        buffer_receive = buffer_receive[expected_size:]
                        
                        if buffer_receive:
                            print(f"data left {bytes_to_str(buffer_receive)}")
                    else:
                        break
                else:
                    break
            else:
                break


        #buffer timeout to avoid misalignment of packets
        if time.time() - last_receive > 1:
            if True:
                if buffer_receive:
                    print(f"[-] timeout, lost data : {buffer_receive}")
            buffer_receive = b''

        #time.sleep(0.05) #minimal sleep time
        time.sleep(0.1)



def handle_packet(data):
    print(f"got packet {len(data)} - {bytes_to_str(data[0:10])}...{bytes_to_str(data[-4:])}")


sync_word = b"\xB5\x62"
idx_payload_size = 5
headers_size = 8

lora = sx126x.sx126x(port="/dev/ttyS0", baudrate=115200, debug=False)
#lora.set_config(air_data_rate=62.5, sub_packet_size=128, tx_power=10)

lora.set_config(air_data_rate=62.5, sub_packet_size=128, tx_power=10,
    network=0, channel=18, channel_noise=False, enable_rssi=False, 
    transmission_mode="transparent", enable_repeater=False,
    wor_control="receiver")

print(f"config {lora.params}")

is_running = True
t_receive = threading.Thread(target=listen_loop, args=[handle_packet, idx_payload_size, headers_size, sync_word])
t_receive.start()



try:
    txt = input()
except KeyboardInterrupt:
    is_running = False

t_receive.join()
lora.close()