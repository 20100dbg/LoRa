#!/usr/bin/env python3

"""
This is an example of a as simple as possible LoRa app.
It allows to send and receive text over LoRa.
"""

import sx126x
import threading
import time

def listener():
    while is_running:
        data = lora.receive()
        if data:
            print("> " + data.decode())
        time.sleep(0.1)


#initialize lora using default settings
lora = sx126x.sx126x()
is_running = True

#start receive thread
t_receive = threading.Thread(target=listener)
t_receive.start()


#loop for sending messages
while True:
    try:
        txt = input()
    except KeyboardInterrupt:
        is_running = False
        break

    #if exit, kill thread and break loop
    if txt == "exit":
        break

    elif txt:
        lora.send_string(txt)

t_receive.join()
lora.close()