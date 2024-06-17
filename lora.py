import sx126x
import threading
import time

#MSG STRUCTURE
#TYPE (TXT/FILE) (1bytes) - FROM (2bytes) - DATA

#FILE: 
#DATA = FILENAME (8bytes) - DATA


def main():

    #initialize lora
    global lora
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

    data_type = data[0]
    data_src = int.from_bytes(data[1:3])
    end_buffer = len(data)
    rssi = 0

    if self.enableRSSI == 'on':
        rssi = -1 * (256 - int.from_bytes(data[-1:]))
        end_buffer = -1

    if data_type == lora.MSG_TYPE['TXT']:
        print(f'FROM {data_src} ({rssi}) : {data[3:end_buffer].decode()}')


    elif data_type == lora.MSG_TYPE['FILE']:
        filename = data[3:11]
        buffer = data[11:end_buffer]
        
        with open(filename, 'ab') as f:
            f.write(buffer)



def handleSend(txt):

    tab = txt.split()

    if tab[0] == 'file' or tab[0] == 'send':
        filename = tab[1]
        lora.sendfile(filename)
    
    else:
        lora.sendmsg(txt)


if __name__ == '__main__':
    main()
