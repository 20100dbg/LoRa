import argparse
import sx126x
import threading
import time

#MSG STRUCTURE
#FROM (2bytes) - DATA

def main():

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
            lora.sendmsg(txt)


def listener():
    while isRunning:
        data = lora.receive()
        if data:
            handleReceive(data)
        time.sleep(0.1)


def handleReceive(data):
    data_src = int.from_bytes(data[0:2])
    end_buffer = len(data)
    rssi = 0

    if lora.enableRSSI == 'on':
        rssi = -1 * (256 - int.from_bytes(data[-1:]))
        end_buffer = -1
        print(f'FROM {data_src} ({rssi}) : {data[2:end_buffer].decode()}')
    else:
        print(f'FROM {data_src} : {data[2:end_buffer].decode()}')



if __name__ == '__main__':

    print()
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('-c', '--channel', metavar='', default=18, type=int, help='0 - 80')
    parser.add_argument('-a', '--address', metavar='', default=10, type=int, help='0 - 65534')
    parser.add_argument('-n', '--network', metavar='', default=1, type=int, help='0 - 255')
    parser.add_argument('-p', '--power', metavar='', default='22', choices=[22,17,13,10], help='22,17,13,10')
    parser.add_argument('-r', '--rssi', default=False, action='store_true', help='Enable RSSI')
    parser.add_argument('-d', '--datarate', metavar='', default='2.4', choices=[0.3,1.2,2.4,4.8,9.6,19.2,38.4,62.5], help='0.3,1.2,2.4,4.8,9.6,19.2,38.4,62.5')
    parser.add_argument('-x', '--repeater', metavar='', default='none', choices=['none', 'client', 'server'], help='none, client, server')
    parser.add_argument('-s', '--packet-size', metavar='', default=128, choices=[240,128,64,32], help='240,128,64,32')
    parser.add_argument('-z', '--debug', default=False, action='store_true', help='Enable Debug')
    args = parser.parse_args()

    #initialize lora
    global lora

    # Utilisation classique
    #lora = sx126x.sx126x(channel=18,address=100,network=0,txPower=10)
    
    lora = sx126x.sx126x(channel=args.channel, address=args.address, network=args.network,
                    txPower=args.power, enableRSSI=args.rssi, airDataRate=args.datarate,
                    repeater=args.repeater, packetSize=args.packet_size, debug=args.debug)

    lora.show_config()

    main()
