import argparse
import sys

# 03H
BAUDRATE = {'1200':0b00000000, '2400':0b00100000, '4800':0b01000000, '9600':0b01100000, '19200':0b10000000, '38400':0b10100000, '57600':0b11000000, '115200':0b11100000}
PORTMODE = { '8N1':0b00000000, '8O1':0b00001000, '8E1':0b00010000}
AIRRATE = { '0.3':0b00000000, '1.2':0b00000001, '2.4':0b00000010, '4.8':0b00000011, '9.6':0b00000100, '19.2':0b00000101, '38.4':0b00000110, '62.5':0b00000111}

# 04h - PacketSize(1) + ChannelNoise(2) + Reserved(3) + TxPower(4) (default 240, disabled, 22)
PACKETSIZE = {'240':0b00000000, '128':0b01000000, '64':0b10000000, '32':0b11000000}
CHANNELNOISE = {'disabled':0b00000000, 'enabled':0b00100000}
TXPOWER = {'22':0b00000000, '17':0b00000001, '13':0b00000010, '10':0b00000011}

# 06h - RSSILevel(1) + TransferMethod(2) + RelayMode(3) + LBT(4) + WORMode(5) + WORCycle(6)
RSSILEVEL = {'disabled':0b00000000, 'enabled':0b10000000}
TRANSFERMODE = {'transparent':0b00000000, 'fixed':0b01000000}
RELAYMODE = {'disabled':0b00000000, 'enabled':0b00100000}
LBT = {'disabled':0b00000000, 'enabled':0b00010000}
WORMODE = {'transmitter':0b00000000, 'receiver':0b00001000}
WORCYCLE = { '500':0b00000000, '1000':0b00000001, '1500':0b00000010, '2000':0b00000011, '2500':0b00000100, '3000': 0b00000101, '3500': 0b00000110, '4000': 0b00000111}


def bytes_pair_to_int(b1, b2):
    return (b1 << 8) + b2


def registers_to_variables(arr):

    high_addr = arr[0]
    low_addr = arr[1]
    netid = arr[2]
    bz = f'{arr[3]:0>8b}'
    baudrate = [k for k, v in BAUDRATE.items() if v == int(bz[:3], 2) << 5][0]
    portmode = [k for k, v in PORTMODE.items() if v == int(bz[3:5], 2) << 3][0]
    airrate = [k for k, v in AIRRATE.items() if v == int(bz[5:], 2)][0]
    bz = f'{arr[4]:0>8b}'
    packetsize = [k for k, v in PACKETSIZE.items() if v == int(bz[:2], 2) << 6][0]
    channelnoise = [k for k, v in CHANNELNOISE.items() if v == int(bz[2:3], 2) << 5][0]
    txpower = [k for k, v in TXPOWER.items() if v == int(bz[6:], 2)][0]
    controlchannel = arr[5]
    bz = f'{arr[6]:0>8b}'
    rssilevel = [k for k, v in RSSILEVEL.items() if v == int(bz[:1], 2) << 7][0]
    transfermode = [k for k, v in TRANSFERMODE.items() if v == int(bz[1:2], 2) << 6][0]
    relaymode = [k for k, v in RELAYMODE.items() if v == int(bz[2:3], 2) << 5][0]
    lbt = [k for k, v in LBT.items() if v == int(bz[3:4], 2) << 4][0]
    wormode = [k for k, v in WORMODE.items() if v == int(bz[4:5], 2) << 3][0]
    worcycle = [k for k, v in WORCYCLE.items() if v == int(bz[5:], 2)][0]
    keyhigh = arr[7]
    keylow = arr[8]

    return [high_addr, low_addr, netid, baudrate, portmode, airrate, \
            packetsize, channelnoise, txpower, controlchannel, rssilevel, \
            transfermode, relaymode, lbt, wormode, worcycle, keyhigh, keylow]



def manual_config(low_high = False):
    
    if low_high:
        try:
            x00h = int(input("Address High Byte - 0-255 (0): ") or 0)
            if x00h < 0 or x00h > 255:
                print("[*] Integer value must be between 0-255")
                raise OptionError()
        except Exception as e: raise e

        try:
            x01h = int(input("Address Low Byte - 0-255 (0): ") or 0)
            if x01h < 0 or x01h > 255:
                print("[*] Integer value must be between 0-255")
                raise OptionError()
        except Exception as e: raise e

    else:

        try:
            address = int(input("Address - 0-65535 (0): ") or 0)
            if address < 0 or address > 65535:
                print("[*] Integer value must be between 0-65535")
                raise OptionError()
        except Exception as e: raise e

        address = address.to_bytes(2, 'big')
        x00h = address[0]
        x01h = address[1]

    try:
        x02h = int(input("Network ID - 0-255 (0): ") or 0)
        if x02h < 0 or x02h > 255:
            print("[*] Integer value must be between 0-255")
            raise OptionError()
    except Exception as e: raise e

    print("Baud Rates\r\n==========")
    for k in BAUDRATE.keys(): print(k)
    x03h_baudrate = input("Enter BaudRate (9600): ") or '9600'
    if x03h_baudrate not in BAUDRATE:
        raise OptionError()
                                          
    print("PortMode\r\n==========")
    for k in PORTMODE.keys(): print(k)
    x03h_portmode = input("Enter PortMode (8N1): ") or '8N1'
    if x03h_portmode not in PORTMODE:
        raise OptionError()

    print("Air Rate\r\n==========")
    for k in AIRRATE.keys(): print(k)
    x03h_airrate = input("Enter Selection Number (2.4): ") or '2.4'
    if x03h_airrate not in AIRRATE:
        raise OptionError()

    print("Packet Size\r\n==========")
    for k in PACKETSIZE.keys(): print(k)
    x04h_packetsize = input("Enter Packet Size (240): ") or '240'
    if x04h_packetsize not in PACKETSIZE:
        raise OptionError()

    x04h_channelnoise = input("Enter Channel Noise state - enabled/disabled (disabled): ") or 'disabled'
    if x04h_channelnoise not in CHANNELNOISE:
        raise OptionError()

    print("Transmit Power\r\n==========")
    for k in TXPOWER.keys(): print(k)
    x04h_txpower = input("Enter Transmit Power (22): ") or '22'
    if x04h_txpower not in TXPOWER:
        raise OptionError()

    try:
        x05h = int(input("Enter Channel - 0-80 (18): ") or 18)
        if x05h < 0 or x05h > 80:
            print("[*] Integer value must be between 0-80")
            raise OptionError()
    except Exception as e: raise e

    x06h_rssi = input("Enter RSSI Level Feedback - enabled/disabled (disabled): ") or 'disabled'
    if x06h_rssi not in RSSILEVEL:
        raise OptionError()

    x06h_transfermode = input("Enter Transfer Mode - fixed/transparent (transparent): ") or 'transparent'
    if x06h_transfermode not in TRANSFERMODE:
        raise OptionError()

    x06h_relaymode = input("Enter Relay Mode - enabled/disabled (disabled): ") or 'disabled'
    if x06h_relaymode not in RELAYMODE:
        raise OptionError()

    x06h_lbt = input("Enter LBT Mode - enabled/disabled (disabled): ") or 'disabled'
    if x06h_lbt not in LBT:
        raise OptionError()
    
    x06h_wormode = input("Enter WOR Mode - receiver/transmitter (transmitter): ") or 'transmitter'
    if x06h_wormode not in WORMODE:
        raise OptionError()

    print("WOR Cycle\r\n==========")
    for k in WORCYCLE.keys(): print(k)
    x06h_worcycle = input("Enter WOR Cycle (2000): ") or '2000'
    if x06h_worcycle not in WORCYCLE:
        raise OptionError()


    if low_high:
        try:
            x07h = int(input("Enter Crypto Key High 0-255 - (0): ") or 0)
            if x07h < 0 or x07h > 255:
                print("[*] Integer value must be between 0-255")
                raise OptionError()
        except Exception as e: raise e

        try:
            x08h = int(input("Enter Crypto Key Low 0-255 - (0): ") or 0)
            if x08h < 0 or x08h > 255:
                print("[*] Integer value must be between 0-255")
                raise OptionError()
        except Exception as e: raise e

    else:

        try:
            crypt = int(input("Enter Crypto Key - 0-65535 (0): ") or 0)
            if crypt < 0 or crypt > 65535:
                print("[*] Integer value must be between 0-65535")
                raise OptionError()
        except Exception as e: raise e

        crypt = crypt.to_bytes(2, 'big')
        x07h = crypt[0]
        x08h = crypt[1]


    x03h = BAUDRATE[x03h_baudrate] + PORTMODE[x03h_portmode] + AIRRATE[x03h_airrate]
    x04h = PACKETSIZE[x04h_packetsize] + CHANNELNOISE[x04h_channelnoise] + TXPOWER[x04h_txpower]
    x06h = RSSILEVEL[x06h_rssi] + TRANSFERMODE[x06h_transfermode] + RELAYMODE[x06h_relaymode] + \
    LBT[x06h_lbt] + WORMODE[x06h_wormode] + WORCYCLE[x06h_worcycle]
    
    return [x00h, x01h, x02h, x03h, x04h, x05h, x06h, x07h, x08h]


def show_config_str(arr):
    print('High Address :', arr[0])
    print('Low Address :', arr[1])
    print('Address (high + low) :', bytes_pair_to_int(arr[0], arr[1]))
    print('Network ID :', arr[2])
    print('Baudrate  :', arr[3])
    print('Port Mode :', arr[4])
    print('Air Rate :', arr[5])
    print('Packet Size :', arr[6])
    print('Channel Noise :', arr[7])
    print('Transmit Power :', arr[8])
    print('Control Channel :', arr[9])
    print('RSSI Level :', arr[10])
    print('Transfer Mode :', arr[11])
    print('Relay Mode :', arr[12])
    print('LBT :', arr[13])
    print('WOR Mode :', arr[14])
    print('WOR Cycle :', arr[15])
    print('Crypto High Byte :', arr[16])
    print('Crypto Low Byte :', arr[17])
    print('Crypto (high + low) :', bytes_pair_to_int(arr[16], arr[17]))


def show_config_hex(arr):
    #cmd = bytearray(b'\xc0\x00\x09')
    cmd = bytearray(b'')

    cmd.append(arr[0])
    cmd.append(arr[1])
    cmd.append(arr[2])
    cmd.append(arr[3])
    cmd.append(arr[4])
    cmd.append(arr[5])
    cmd.append(arr[6])
    cmd.append(arr[7])
    cmd.append(arr[8])

    return cmd


def bytes_to_str(arr):
    return ' '.join(['{:02X}'.format(b) for b in arr])   


def str_hex_to_bytes(txt):
    txt = txt.replace(' ', '')
    return bytes.fromhex(txt)


parser = argparse.ArgumentParser(description='LoRa config tool')
group = parser.add_mutually_exclusive_group(required=True)
parser.add_argument('-l', '--low-high', default=False, action='store_true', help='Allow to set individually low and high registers')
group.add_argument('-m', '--manual', default=False, action='store_true', help='Enter config manually')
group.add_argument('-x', '--hex', nargs=argparse.REMAINDER, metavar='', help='Enable config as hex, format : 01 A2 3B 45')

if len(sys.argv) == 1:
    parser.print_help()
    exit(0)

args = parser.parse_args()


if args.manual:
    arr_registers = manual_config(args.low_high)
    cmd = show_config_hex(arr_registers)
    print()
    print("Config in hex :", bytes_to_str(cmd))
elif args.hex:

    if isinstance(args.hex, list):
        args.hex = ''.join(args.hex)

    cmd = str_hex_to_bytes(args.hex)

    if cmd[0:3] == b'\xC1\x00\x09':
        cmd = cmd[3:]

    config = registers_to_variables(cmd)
    show_config_str(config)
