#!/usr/bin/env python3

"""
This is an example of a as simple as possible LoRa app.
It allows to send and receive text over LoRa.
"""

import sx126x
import threading
import time

def listener():
    while isRunning:
        data = lora.receive()
        if data:
            print("> " + data.decode())
        time.sleep(0.1)


#initialize lora
#lora = sx126x.init_config(channel=18,address=100,network=0)
lora = sx126x.init()

#start receive thread
isRunning = True
t_receive = threading.Thread(target=listener)
t_receive.start()

#loop for sending messages
while True:
    txt = input()

    #if exit, kill thread and break loop
    if txt == "exit":
        isRunning = False
        t_receive.join()
        break

    elif txt:
        lora.send_string(txt)
