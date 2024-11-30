import sx126x
import threading
import time
import RPi.GPIO as GPIO
from signal import signal, SIGINT

def handler(signal_received, frame):
    global isRunning
    print("Caught Ctrl+C")
    isRunning = False
    t_receive.join()
    exit(0)

def listener():
    while isRunning:
        data = lora.receive()
        if data:
            print(f"RECEIVED : {data}")

        time.sleep(0.1)

def strhextobytes(txt):
    txt = txt.replace(' ', '')
    return bytes.fromhex(txt)


signal(SIGINT, handler)

#initialize lora
channel = 18
address = 1
network = 0
debug = False

lora = sx126x.sx126x(channel=channel, address=address, network=network,
                txPower='22', airDataRate='9.6', packetSize='128', 
                repeater='none', debug=debug, enableRSSI=False,
                key=0, netid1=None, netid2=None)

print()
lora.show_config()

#start receive thread
isRunning = True
t_receive = threading.Thread(target=listener)
t_receive.start()

print('Input hex : 02 f3 2C')

while isRunning:

    msg = input(">")
    if msg == 'exit':
        break
    elif msg:
        lora.sendraw(strhextobytes(msg))
