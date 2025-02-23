import sx126x
import threading
import time
import argparse

def listener():
    while is_running:
        data = lora.receive()
        if data:
            print(f"> {bytes_to_str(data)}")
            print(f"> {data}")

            if is_bot:
                lora.send_bytes(data)

        time.sleep(0.1)


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
TODO
'''

parser = argparse.ArgumentParser(description='Testing tool for LoRa') #, usage=msg_usage('%(prog)s'))
group1 = parser.add_argument_group('Basics')
group1.add_argument('-c', '--channel', metavar='', type=int, help='0 - 80')
group1.add_argument('-n', '--network', metavar='', type=int, help='0 - 255')
group1.add_argument('-a', '--logical_address', metavar='', type=int, help='0 - 65535')
group1.add_argument('-ah', '--addrh', metavar='', type=int, help='0 - 255')
group1.add_argument('-al', '--addrl', metavar='', type=int, help='0 - 255')

group2 = parser.add_argument_group('Operating modes')
group2.add_argument('-x', '--hex', default=False, action='store_true', help='Send hex instead strings. Format : 02 f3 2C')
group2.add_argument('-b', '--bot', default=False, action='store_true', help='Start as bot that sends back everything it receives')

group3 = parser.add_argument_group('Performance settings')
group3.add_argument('-adr', '--air_data_rate', metavar='', type=float, choices=[0.3,1.2,2.4,4.8,9.6,19.2,38.4,62.5], help='0.3,1.2,2.4,4.8,9.6,19.2,38.4,62.5')
group3.add_argument('-p', '--tx_power', metavar='', type=int, choices=[22,17,13,10], help='22,17,13,10')
group3.add_argument('-s', '--sub_packet_size', metavar='', type=int, choices=[240,128,64,32], help='240,128,64,32')

"""
group4 = parser.add_argument_group('Repeater')
group4.add_argument('-r', '--enable_repeater', action='store_true', help='')
group4.add_argument('-rm', '--repeater', metavar='', choices=['none', 'client', 'server'], help='none, client, server')
group4.add_argument('-n1', '--netid1', metavar='', type=int, help='0 - 255 Left Network ID')
group4.add_argument('-n2', '--netid2', metavar='', type=int, help='0 - 255 Right Network ID')
"""

group4 = parser.add_argument_group('Repeater')
group4.add_argument('-r', '--enable_repeater', action='store_true', help='')

group5 = parser.add_argument_group('Advanced')
group5.add_argument('-rssi', '--enable_rssi', action='store_true', help='Append RSSI byte at the end of every received message')
group5.add_argument('-cn', '--channel_noise', action='store_true', help='Enable get_channel_noise() method, which return current channel noise and last message RSSI')
group5.add_argument('-t', '--transmission_mode', metavar='', choices=['fixed', 'transparent'], help='')
group5.add_argument('-lbt', '--enable_lbt', action='store_true', help='Enable Listen Before Transmit')
group5.add_argument('-w', '--wor_control', metavar='', choices=['transmitter', 'receiver'], help='')
group5.add_argument('-wc', '--wor_cycle', metavar='', choices=[500, 1000, 1500, 2000, 2500, 3000, 3500, 4000], help='')
group5.add_argument('-ch', '--crypth', metavar='', type=int, help='Crypto key 0 - 255')
group5.add_argument('-cl', '--cryptl', metavar='', type=int, help='Crypto key 0 - 255')
group5.add_argument('-k', '--key', metavar='', type=int, help='Crypto key 0 - 65535')

group5.add_argument('-d', '--debug', action='store_true', help='Enable Debug')

args = parser.parse_args()

global is_running
is_running = True
is_bot = args.bot
is_hex = args.hex

lora = sx126x.sx126x(port="/dev/ttyS0", debug=args.debug)
params = vars(args)

lora.set_config(addrh=params['addrh'], addrl=params['addrl'], network=params['network'], 
                air_data_rate=params['air_data_rate'], sub_packet_size=params['sub_packet_size'],
                channel_noise=params['channel_noise'], tx_power=params['tx_power'], 
                channel=params['channel'], enable_rssi=params['enable_rssi'], 
                transmission_mode=params['transmission_mode'], enable_repeater=params['enable_repeater'], 
                enable_lbt=params['enable_lbt'], wor_control=params['wor_control'], 
                wor_cycle=params['wor_cycle'], crypth=params['crypth'], cryptl=params['cryptl'])

if args.enable_repeater:
    lora.set_repeater(args.repeater, args.netid1, args.netid2)


t_receive = threading.Thread(target=listener)
t_receive.start()

if debug:
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

close()
lora.close()
