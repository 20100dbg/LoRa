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

            if lora.transmissionMode == 'transparent':
                lora.sendmsg(txt)
            else:
                tab = txt.split('/')
                if len(tab) == 3:
                    lora.sendmsg(tab[2], int(tab[0]), int(tab[1]))
                else:
                    print("Format : ADDR/NETID/MSG")


def listener():
    while isRunning:
        data = lora.receive()
        if data:
            handleReceive(data)
        #time.sleep(0.1)


def handleReceive(data):

    if lora.transmissionMode == 'transparent':
        addr = int.from_bytes(data[0:2])
        netid = int.from_bytes(data[2:3])
        msg = data[3:]
        msg_from = f'FROM {addr}/{netid}'
    else:
        msg = data
        msg_from = 'Received'
    
    if lora.enableRSSI == 'on':
        rssi = -1 * (256 - int.from_bytes(data[-1:]))
        msg = data[:-1]
        print(f'RSSI : {rssi}')

    print(f'{msg_from} : {msg.decode()}')


def msg_usage(name=None):
    return f'''
Basic : 
    {name} -c 18 -a 10 -n 1
Specify performance settings : 
    {name} -c 18 -a 10 -n 1 -p 10 -d 9.6 -s 128
Repeater stuff :
    {name} -c 18 -a 10 -n 1 -x client
'''

if __name__ == '__main__':

    print()
    parser = argparse.ArgumentParser(description='Testing tool for LoRa') #, usage=msg_usage('%(prog)s'))
    group1 = parser.add_argument_group('Basics')
    group1.add_argument('-c', '--channel', metavar='', default=18, type=int, help='0 - 80')
    group1.add_argument('-a', '--address', metavar='', default=10, type=int, help='0 - 65534')
    group1.add_argument('-n', '--network', metavar='', default=1, type=int, help='0 - 255')
    
    group2 = parser.add_argument_group('Performance settings')
    group2.add_argument('-p', '--power', metavar='', default='22', choices=[22,17,13,10], help='22,17,13,10')
    group2.add_argument('-d', '--datarate', metavar='', default='2.4', choices=[0.3,1.2,2.4,4.8,9.6,19.2,38.4,62.5], help='0.3,1.2,2.4,4.8,9.6,19.2,38.4,62.5')
    group2.add_argument('-s', '--packet-size', metavar='', default=128, choices=[240,128,64,32], help='240,128,64,32')
    
    group3 = parser.add_argument_group('Repeater')
    group3.add_argument('-x', '--repeater', metavar='', default='none', choices=['none', 'client', 'server'], help='none, client, server')
    group3.add_argument('-1', '--netid1', metavar='', type=int, help='0 - 255 Left Network ID')
    group3.add_argument('-2', '--netid2', metavar='', type=int, help='0 - 255 Right Network ID')
    
    group4 = parser.add_argument_group('Info & Debug')
    group4.add_argument('-z', '--debug', default=False, action='store_true', help='Enable Debug')
    group4.add_argument('-r', '--rssi', default=False, action='store_true', help='Enable RSSI')
    group4.add_argument('-k', '--key', metavar='', default=0, type=int, help='Crypto key')
    args = parser.parse_args()

    #print(msg_usage('%(prog)s'))

    #initialize lora
    global lora

    # Utilisation classique
    #lora = sx126x.sx126x(channel=18,address=100,network=0,txPower=10)
    
    lora = sx126x.sx126x(channel=args.channel, address=args.address, network=args.network,
                    txPower=args.power, airDataRate=args.datarate, packetSize=args.packet_size, 
                    repeater=args.repeater, debug=args.debug, enableRSSI=args.rssi,
                    key=args.key, netid1=args.netid1, netid2=args.netid2)

    print()
    lora.show_config()
    print()
    print("Address/Network/Message")
    
    main()
