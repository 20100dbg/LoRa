import sx126x
import threading
import time

#MSG STRUCTURE
#FROM (2bytes) - DATA


def main():

    #initialize lora
    global lora

    #lora = sx126x.sx126x(address=100, network=0, channel=18, txPower='22', enableRSSI='off'):
    lora = sx126x.sx126x(channel=18,address=100,network=0,txPower=10)

    #start receive thread
    global isRunning
    isRunning = True

    global t_receive
    t_receive = threading.Thread(target=listener)
    t_receive.start()


    #loop for sending messages
    while True:
        txt = input("> ")

        #if exit, kill thread and break loop
        if txt == "exit":
            isRunning = False
            t_receive.join()
            break
        elif txt:
            handleSend(txt)


def listener():
    while isRunning:
        data = lora.receive()
        if data:
            handleReceive(data)
        time.sleep(0.1)

#
def handleReceive(data):

    #print("received :", data)

    data_src = int.from_bytes(data[0:2])
    end_buffer = len(data)
    rssi = 0

    if lora.enableRSSI == 'on':
        rssi = -1 * (256 - int.from_bytes(data[-1:]))
        end_buffer = -1
        print(f'FROM {data_src} ({rssi}) : {data[2:end_buffer].decode()}')
    else:
        print(f'FROM {data_src} : {data[2:end_buffer].decode()}')



def handleSend(txt):

    tab = txt.split()
    lora.sendmsg(txt)


if __name__ == '__main__':
    main()
