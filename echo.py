import sx126x
import threading
import time
import RPi.GPIO as GPIO
import sys

def light_led(pin, delay=0.1):
    if mode_client:
        GPIO.output(pin, True)
        time.sleep(delay)
        GPIO.output(pin, False)


def reverse(msg):
    return msg[::-1]


def listener():

    while isRunning:
        data = lora.receive()

        if data and len(data) >= 2:

            msg_type = data[0]
            msg = data[1:]

            if msg_type == 1:
                sender(b'\x02' + reverse(msg))

            elif msg_type == 2:
                if msg_check == reverse(msg):
                    light_led(pin_receive)
                else:
                    light_led(pin_error)
            else:
                light_led(pin_error)

            print(f"RECEIVED : {data}")

        time.sleep(0.1)


def sender(msg, ping = True):

    print(f"sending {msg}")
    lora.sendraw(msg)
    light_led(pin_send)    

# 01 : ping
# 02 : pong

mode_client = False

if len(sys.argv) > 1 and sys.argv[1] == 'client':
    mode_client = True

msg_check = b'\x01\x02\x03\x04\x05'
sleep_time = 10

#LED
pin_send = 17
pin_receive = 22
pin_error = 10

if mode_client:
    #GPIO.cleanup()
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(pin_send, GPIO.OUT, initial=GPIO.LOW)
    GPIO.setup(pin_receive, GPIO.OUT, initial=GPIO.LOW)
    GPIO.setup(pin_error, GPIO.OUT, initial=GPIO.LOW)


#initialize lora
lora = sx126x.sx126x(channel=18,address=100,network=0)

#start receive thread
isRunning = True
t_receive = threading.Thread(target=listener)
t_receive.start()


if mode_client:

    #loop for sending messages
    while True:

        #in case we need to end this loop someday
        if False:
            isRunning = False
            t_receive.join()
            break

        msg_check = input().encode()

        sender(b'\x01' + msg_check)
        time.sleep(sleep_time)
