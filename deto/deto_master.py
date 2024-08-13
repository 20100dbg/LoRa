import sx126x
import time
import threading


def listener():
    while isRunning:
        data = lora.receive()
        if data:
            print("received : ", data)
        time.sleep(0.1)


def send(addr, msgid):
    addr = addr.to_bytes(1, 'big')
    m = msg[msgid].to_bytes(1, 'big')
    lora.sendraw(addr + m)



msg = {"PING": 0, "PONG": 1, "ARMED_ON": 2, "ACK_ARMED_ON": 3, "ARMED_OFF": 4, "ACK_ARMED_OFF": 5, "FIRE": 6, "ACK_FIRE" : 7 }

#MSG
#ADRESSE (1) + MSG (1)

buttonActive = False
startButton = time.time()

#initialize lora
lora = sx126x.sx126x(channel=18,address=0,network=0)


isRunning = True
t_receive = threading.Thread(target=listener)
t_receive.start()


print("msg : PING, ARMED_ON, ARMED_OFF, FIRE")

while True:
    addr = int(input("slave : "))
    msgid = input("msg : ")

    send(addr, msgid)
