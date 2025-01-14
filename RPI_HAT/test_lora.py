import sx126x
import threading
import time
import argparse
import sys


def listener():
    while is_running:
        data = lora.receive()
        if data:
            print(f"> {bytes_to_str(data)}")
            print(f"> {data}")

            if is_bot:
                lora.send_bytes(data)

        time.sleep(0.01)


def close():
    global is_running
    print("Closing...")
    is_running = False;
    t_receive.join()


def bytes_to_str(arr):
    return ' '.join(['{:02X}'.format(b) for b in arr])

def str_hex_to_bytes(txt):
    txt = txt.replace(' ', '')
    return bytes.fromhex(txt)

def msg_usage(name=None):
    return f'''
Basic : 
    {name} -c 18 -a 10 -n 1
Specify performance settings : 
    {name} -c 18 -a 10 -n 1 -p 10 -d 9.6 -s 128
Repeater stuff :
    {name} -c 18 -a 10 -n 1 -x client
'''

parser = argparse.ArgumentParser(description='Testing tool for LoRa') #, usage=msg_usage('%(prog)s'))
group1 = parser.add_argument_group('Basics')
group1.add_argument('-c', '--channel', metavar='', default=18, type=int, help='0 - 80')
group1.add_argument('-a', '--address', metavar='', default=10, type=int, help='0 - 65535')
group1.add_argument('-n', '--network', metavar='', default=1, type=int, help='0 - 255')

group2 = parser.add_argument_group('Operating modes')
group2.add_argument('-x', '--hex', default=False, action='store_true', help='Send hex instead strings. Format : 02 f3 2C')
group2.add_argument('-b', '--bot', default=False, action='store_true', help='Start as bot that sends back everything it receives')

group3 = parser.add_argument_group('Performance settings')
group3.add_argument('-p', '--power', metavar='', default=22, choices=[22,17,13,10], help='22,17,13,10')
group3.add_argument('-dr', '--datarate', metavar='', default=2.4, choices=[0.3,1.2,2.4,4.8,9.6,19.2,38.4,62.5], help='0.3,1.2,2.4,4.8,9.6,19.2,38.4,62.5')
group3.add_argument('-s', '--packet-size', metavar='', default=128, choices=[240,128,64,32], help='240,128,64,32')

group4 = parser.add_argument_group('Repeater')
group4.add_argument('-r', '--repeater', metavar='', default='none', choices=['none', 'client', 'server'], help='none, client, server')
group4.add_argument('-n1', '--netid1', metavar='', type=int, help='0 - 255 Left Network ID')
group4.add_argument('-n2', '--netid2', metavar='', type=int, help='0 - 255 Right Network ID')

group5 = parser.add_argument_group('Info & Debug')
group5.add_argument('-d', '--debug', default=False, action='store_true', help='Enable Debug')
group5.add_argument('-rssi', '--rssi', default=False, action='store_true', help='Enable RSSI')
group5.add_argument('-k', '--key', metavar='', default=0, type=int, help='Crypto key')
args = parser.parse_args()

global is_running
is_running = True
is_bot = args.bot

lora = sx126x.sx126x(channel=args.channel, address=args.address, network=args.network,
                tx_power=args.power, air_data_rate=args.datarate, sub_packet_size=args.packet_size, 
                repeater=args.repeater, debug=args.debug, enable_rssi=args.rssi,
                key=args.key, netid1=args.netid1, netid2=args.netid2)


t_receive = threading.Thread(target=listener)
t_receive.start()

if args.debug:
    print()
    lora.show_config()
print()


while is_running:

    try:
        msg = input()
    except KeyboardInterrupt:
        break

    if msg == 'exit':
        break
    elif msg:
        if args.hex:
            bmsg = str_hex_to_bytes(msg)
            lora.send_bytes(bmsg)
        else:
            lora.send_string(msg)

close()
lora.close()