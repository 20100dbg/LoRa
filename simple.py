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
lora = sx126x.sx126x(channel=18,address=100,network=0)

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
        lora.sendraw(txt.encode())
