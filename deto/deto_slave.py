import sx126x
import time
import RPi.GPIO as GPIO
from signal import signal, SIGINT

def cleanclose():
    GPIO.cleanup()
    exit(0)

def handler(signal_received, frame):
    cleanclose()

def activer_relais():
    GPIO.output(pin_relais, GPIO.LOW)
    time.sleep(1)
    GPIO.output(pin_relais, GPIO.HIGH)

def armer(armer):
    return True


def envoyer(adresse, msgid):
    addr = adresse.to_bytes(1, 'big')
    m = msg[msgid].to_bytes(1, 'big')
    lora.sendraw(addr + m)


def listener():
    while True:
        data = lora.receive()

        if data and len(data) == 2:

            addr = int.from_bytes(data[0:1])
            msgid = int.from_bytes(data[1:2])

            if msg["PING"] == msgid:
                envoyer(addresse_master, msg["PONG"])
            elif msg["ARMER_ON"] == msgid:
                armer(True)
                envoyer(addresse_master, msg["ACK_ARMER_ON"])
            elif msg["ARMER_OFF"] == msgid:
                armer(False)
                envoyer(addresse_master, msg["ACK_ARMER_OFF"])
            elif msg["FEU"] == msgid:
                activer_relais()
                envoyer(addresse_master, msg["ACK_FEU"])

        time.sleep(0.1)


signal(SIGINT, handler)


msg = {"PING": 0, "PONG": 1, "ARMER_ON": 2, "ACK_ARMER_ON": 3, "ARMER_OFF": 4, "ACK_ARMER_OFF": 5, "FEU": 6, "ACK_FEU" : 7 }
adresse = 1
addresse_master = 0


pin_relais = 4

GPIO.setmode(GPIO.BCM)
GPIO.setup(pin_relais, GPIO.OUT, initial=GPIO.HIGH)

#initialize lora
lora = sx126x.sx126x(channel=18,address=adresse,network=0)

listener()