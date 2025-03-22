import os
import sx126x
import threading
import time
from enum import Enum

class MsgType(Enum):
    HELLO = 1
    SEND = 2
    NACK = 3

HEADER_SIZE = 3

def extract_header(data):
    """ Extract header fields from message """
    
    return (data[0],                  #msg_type - 1
            bytes_to_int(data[1:5]))  #msg_id - 4

def int_to_bytes(x, bytes_count=1):
    return x.to_bytes(bytes_count, 'big')

def bytes_to_int(b):
    return int.from_bytes(b)

def bytes_to_str(arr):
    return ' '.join(['{:02X}'.format(b) for b in arr])


def callback_lora(data):

    msg_type, msg_id = extract_header(data)


    if msg_type == MsgType.HELLO.value:
        filesize = bytes_to_int(data[5:9])
        filename = data[9:].decode()

    elif msg_type == MsgType.SEND.value:

        payload_size = data[5]

        if payload_size != len(data[6:]):
            print("[-] Payload size error")

        with open(filename, "ab") as f:

            if msg_id != 0 and last_msg_id != msg_id - 1:
                missing_msg_id.append(msg_id - 1)

                #write placeholder
                f.write(b'\x00' * payload_size)
            else:
                f.write(data[6:payload_size + 6])


        last_msg_id = msg_id



#TYPE MSGID PAYLOAD

#HELLO - filesize 4 - filename
#HELLO - taille payload - nb envoi - filename


#SEND - PAYLOAD_SIZE 1 - DATA
#NACK - NB MISSING 4 - MSGID

sender = True
filename = "xxx"
filesize = os.path.getsize(filename)

#filesize max = 256^4 = 4 294 967 296


last_msg_id = 0
missing_msg_id = []

air_data_rate = 62.5 #0.3 1.2 2.4 4.8 9.6 19.2 38.4 62.5
channel = 18
sub_packet_size = 240 #240 128 64 32
payload_size = sub_packet_size - 6

msg_id = 0

lora = sx126x.sx126x(port="/dev/ttyS0", debug=False)
lora.set_config(air_data_rate=air_data_rate, sub_packet_size=sub_packet_size, tx_power=10, channel=channel)

if sender:

    #send HELLO
    msg = b''
    msg += MsgType.HELLO.value
    msg += 0
    msg += int_to_bytes(filesize, 4)
    msg += filename.encode()

    lora.send_bytes(msg)

    with open(filename, "rb") as f:
        while True:
            data = f.read(payload_size)
            real_payload_size = len(data)

            msg = b''
            msg += MsgType.SEND.value
            msg += int_to_bytes(msg_id, 4)
            msg += int_to_bytes(real_payload_size)
            msg += data
            msg_id += 1

            lora.send_bytes(msg)
            time.sleep(0.05)

#receiver
else:
    lora.listen(callback, expected_size=None,)



while True:
    time.sleep(1)