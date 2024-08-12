import sx126x
import time
import RPi.GPIO as GPIO
from signal import signal, SIGINT

def cleanclose():
    GPIO.cleanup()
    exit(0)

def handler(signal_received, frame):
    cleanclose()

def get_etat(pin):
    return not GPIO.input(pin)

def envoyer(adresse, msgid):
    addr = adresse.to_bytes(1, 'big')
    m = msg[msgid].to_bytes(1, 'big')
    lora.sendraw(addr + m)


msg = {"PING": 0, "PONG": 1, "ARMER_ON": 2, "ACK_ARMER_ON": 3, "ARMER_OFF": 4, "ACK_ARMER_OFF": 5, "FEU": 6, "ACK_FEU" : 7 }

#MSG
#ADRESSE (1) + MSG (1)

signal(SIGINT, handler)

pin_bouton = 21

GPIO.setmode(GPIO.BCM)
GPIO.setup(pin_bouton, GPIO.IN, pull_up_down=GPIO.PUD_UP)

boutonActif = False
debutBouton = time.time()

#initialize lora
lora = sx126x.sx126x(channel=18,address=0,network=0)

adresse_slave = 1

while True:

    if not boutonActif and get_etat(pin_bouton):
        debutBouton = time.time()
        boutonActif = True

    if boutonActif and not get_etat(pin_bouton):
        boutonActif = False

    if boutonActif and (time.time() - debutBouton) > 2:
        envoyer(adresse_slave, msg["FEU"])
        boutonActif = False


    time.sleep(0.1)