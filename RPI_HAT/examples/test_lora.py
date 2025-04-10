import sx126x
import threading
import time
import argparse

def listener():
    while is_running:
        data = lora.receive()
        if data:
            print(f"> {bytes_to_hex(data)}")
            print(f"> {data}")

            if is_bot:
                lora.send_bytes(data)

        time.sleep(0.1)


def close():
    print("Closing...")
    t_receive.join()


def bytes_to_hex(arr):
    return ' '.join(['{:02X}'.format(b) for b in arr])

def str_hex_to_bytes(txt):
    txt = txt.replace(' ', '')
    return bytes.fromhex(txt)

def msg_usage(name=None):
    return f'''
TODO
'''

parser = argparse.ArgumentParser(description='Testing tool for LoRa') #, usage=msg_usage('%(prog)s'))
group1 = parser.add_argument_group('Basics')
group1.add_argument('-c', '--channel', metavar='', type=int, help='0 - 80')
group1.add_argument('-n', '--network', metavar='', type=int, help='0 - 255')
group1.add_argument('-a', '--air_data_rate', metavar='', type=float, choices=[0.3,1.2,2.4,4.8,9.6,19.2,38.4,62.5], help='0.3,1.2,2.4,4.8,9.6,19.2,38.4,62.5')
group1.add_argument('-p', '--tx_power', metavar='', type=int, choices=[22,17,13,10], help='22,17,13,10')

group2 = parser.add_argument_group('Operating modes')
group2.add_argument('-x', '--hex', default=False, action='store_true', help='Send hex instead strings. Format : 02 f3 2C')
group2.add_argument('-b', '--bot', default=False, action='store_true', help='Start as bot that sends back everything it receives')

group5 = parser.add_argument_group('Advanced')
group5.add_argument('-r', '--enable_rssi', action='store_true', help='Append RSSI byte at the end of every received message')
group5.add_argument('-cn', '--channel_noise', action='store_true', help='Enable get_channel_noise() method, which return current channel noise and last message RSSI')
group5.add_argument('-k', '--key', metavar='', type=int, help='Crypto key 0 - 65535')
group1.add_argument('-s', '--sub_packet_size', metavar='', type=int, choices=[240,128,64,32], help='240,128,64,32')

group5.add_argument('-d', '--debug', action='store_true', help='Enable Debug')

args = parser.parse_args()
#params = vars(args)

is_running = True
is_bot = args.bot
is_hex = args.hex

if args.key:
    tmp = args.key.to_bytes(2, "big")
    args.crypth = tmp[0]
    args.cryptl = tmp[1]
else:
    args.crypth = 0
    args.cryptl = 0


lora = sx126x.sx126x(port="/dev/ttyAMA0", debug=args.debug)

lora.set_config(channel=args.channel, network=args.network, air_data_rate=args.air_data_rate, 
                tx_power=args.tx_power, channel_noise=args.channel_noise, 
                enable_rssi=args.enable_rssi, sub_packet_size=args.sub_packet_size,
                crypth=args.crypth, cryptl=args.cryptl)


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
        if is_hex:
            bmsg = str_hex_to_bytes(msg)
            lora.send_bytes(bmsg)
        else:
            lora.send_string(msg)

is_running = False
close()
lora.close()
