import sx126x
import time
import RPi.GPIO as GPIO
from signal import signal, SIGINT

def handler(signal_received, frame):
    GPIO.cleanup()
    exit(0)

def activer_relais():
    GPIO.output(pin_relais, GPIO.LOW)
    time.sleep(1)
    GPIO.output(pin_relais, GPIO.HIGH)

def setArmed(state):
    isArmed = state

def send(addr, msgkey):
    addr = addr.to_bytes(1, 'big')
    msgkey = msgkey.to_bytes(1, 'big')
    lora.sendraw(addr + msgkey)

def getmsgkey(msgvalue):
    idx = list(msg.values()).index(msgvalue)
    return list(msg.keys())[idx]

def listener():
    isArmed = False

    while True:
        data = lora.receive()

        if data and len(data) == 2:

            addr = int.from_bytes(data[0:1])
            msgvalue = int.from_bytes(data[1:2])

            print("to :", addr, " - MSG :", getmsgkey(msgvalue))

            if addr != addr_local:
                continue

            if msg["PING"] == msgvalue:
                send(addr_master, msg["PONG"])
            elif msg["ARMED_ON"] == msgvalue:
                isArmed = True
                send(addr_master, msg["ACK_ARMED_ON"])
            elif msg["ARMED_OFF"] == msgvalue:
                isArmed = False
                send(addr_master, msg["ACK_ARMED_OFF"])
            elif msg["FIRE"] == msgvalue:
                if isArmed:
                    activer_relais()
                    send(addr_master, msg["ACK_FIRE"])

        time.sleep(0.1)


signal(SIGINT, handler)

global msg
msg = {"PING": 0, "PONG": 1, "ARMED_ON": 2, "ACK_ARMED_ON": 3, "ARMED_OFF": 4, "ACK_ARMED_OFF": 5, "FIRE": 6, "ACK_FIRE" : 7 }

addr_local = 2
addr_master = 0
pin_relais = 4

GPIO.setmode(GPIO.BCM)
GPIO.setup(pin_relais, GPIO.OUT, initial=GPIO.HIGH)

#initialize lora
lora = sx126x.sx126x(channel=18,address=addr_local,network=0)

listener()
