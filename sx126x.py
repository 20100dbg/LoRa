import RPi.GPIO as GPIO
import time
import serial
import os

class sx126x():

    SERIAL_PORT_RATE = {'1200' : 0b00000000, '2400' : 0b00100000, '4800' : 0b01000000, '9600' : 0b01100000, '19200' : 0b10000000, '38400' : 0b10100000, '57600' : 0b11000000, '115200' : 0b11100000 }
    SERIAL_PARITY_BIT = {'8N1' : 0b00000000, '8O1' : 0b00001000, '8E1' : 0b00010000 } #serial.PARITY_NONE - 8N1, serial.PARITY_ODD - 8O1, serial.PARITY_EVEN - 8E1    
    AIR_DATA_RATE = {'0.3' : 0b00000000, '1.2' : 0b00000001, '2.4' : 0b00000010, '4.8' : 0b00000011, '9.6' : 0b00000100, '19.2' : 0b00000101, '38.4' : 0b00000110, '62.5' : 0b00000111 }
    SUB_PACKET_SIZE = {'240' : 0b00000000, '128' : 0b01000000, '64' : 0b10000000, '32' : 0b11000000 }
    CHANNEL_NOISE = {'off' : 0b00000000, 'on' : 0b00100000 }
    TX_POWER = {'22' : 0b00000000, '17' : 0b00000001, '13' : 0b00000010, '10' : 0b00000011 }
    ENABLE_RSSI = {'off' : 0b00000000, 'on' : 0b10000000 }
    TRANSMISSION_MODE = {'fixedPoint' : 0b01000000, 'transparent' : 0b000000000 }
    ENABLE_REPEATER = {'off' : 0b00000000, 'on' : 0b00100000 }
    ENABLE_LBT = {'off' : 0b00000000, 'on' : 0b00010000 }
    WOR_CONTROL = {'receiver' : 0b00001000, 'transmitter' : 0b00000000 }
    WOR_CYCLE = {'500' : 0b00000000, '1000' : 0b00000001, '1500' : 0b00000010, '2000' : 0b00000011, '2500' : 0b00000100, '3000' : 0b00000101, '3500' : 0b00000110, '4000' : 0b00000111 }


    def __init__(self, address = 100, network=0, channel=18, txPower='22', enableRSSI=False,
                    airDataRate='2.4', repeater='none', packetSize='128', debug=False):

        self.debug = debug
        self.logicalAddress = address

        #set default parameters
        self.port = '/dev/ttyS0'
        self.serialPortRate = '9600'
        self.timeout = 1
        self.serialParityBit = serial.PARITY_NONE
        self.loraParityBit = self.convertSerialParity(self.serialParityBit)
        
        self.channel = int(channel) #0 - 80
        self.crypt = 0 #0 - 65535
        self.network = int(network) #0 - 255

        self.airDataRate =airDataRate
        self.subPacketSize ='128'
        self.channelNoise ='off'
        self.txPower = str(txPower)
        self.enableRSSI = 'on' if enableRSSI else 'off'

        if repeater == "server":
            self.transmissionMode = 'fixedPoint'
            self.address = address
            self.enableRepeater = 'on'
        elif repeater == "client":
            self.transmissionMode = 'fixedPoint'
            self.address = address
            self.enableRepeater = 'off'
        else: #none
            self.transmissionMode = 'transparent'
            self.address = 65535
            self.enableRepeater = 'off'

        self.enableLBT = 'off'
        self.WORcontrol = 'transmitter'
        self.WORcycle = '2000'
        
        
        self.M0 = 22
        self.M1 = 27

        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
        GPIO.setup(self.M0, GPIO.OUT)
        GPIO.setup(self.M1, GPIO.OUT)

        #self.gpio_mode("let's rock")
        self.writeConfig()


    def show_config(self):
        print(f'Channel {self.channel}, address {self.logicalAddress}, network {self.network}')
        print(f'mode {self.transmissionMode}, real address {self.address}, repeater {self.enableRepeater}')


    def gpio_mode(self, mode):
        if mode == 'conf':
            GPIO.output(self.M0, False)
            GPIO.output(self.M1, True)
        elif mode == 'wor':
            GPIO.output(self.M0, True)
            GPIO.output(self.M1, False)
        elif mode == 'sleep':
            GPIO.output(self.M0, True)
            GPIO.output(self.M1, True)
        else:
            GPIO.output(self.M0, False)
            GPIO.output(self.M1, False)

        time.sleep(.3)


    def openSerial(self):
        ser = serial.Serial(port=self.port, baudrate=self.serialPortRate, \
                    parity=self.serialParityBit, stopbits=serial.STOPBITS_ONE,
                    bytesize=serial.EIGHTBITS, timeout=self.timeout)

        return ser


    def sendraw(self, data):
        ser = self.openSerial()

        ser.write(data)
        time.sleep(.1)
        #ret = ser.readlines()
        ser.close()


    def sendmsg(self, data):
        if self.transmissionMode == 'fixedPoint':
            data = self.logicalAddress.to_bytes(2, 'big') + self.network.to_bytes(1, 'big') + data.encode()
        else:
            data = self.logicalAddress.to_bytes(2, 'big') + data.encode()

        if self.debug:
            print('[+] sending :', data)
        
        self.sendraw(data)


    def receive(self):
        ser = self.openSerial()

        data = ser.read_until(expected='')

        if not data:
            data = None
        else:
            if self.debug:
                print('[+] received',len(data),'data :', data)

        ser.close()
        return data


    def convertSerialParity(self,parity):
        if parity == serial.PARITY_NONE:
            return "8N1"
        elif parity == serial.PARITY_ODD:
            return "8O1"
        elif parity == serial.PARITY_EVEN:
            return "8E1"
        else:
            return "8N1"


    def getRSSI(self):
        
        if self.channelNoise == 'off':
            return

        ser = self.openSerial()
        
        ser.write(b'\xC0\xC1\xC2\xC3\x00\x02')
        ret = ser.read_until()
        print(ret)
        currentNoise = ret[3]
        lastReceive = ret[4]

        ser.close()
        return currentNoise, lastReceive



    def setConfig(self, port=None, serialPortRate=None, timeout=None, serialParityBit=None,
            channel=None,crypt=None,address=None, network=None,airDataRate=None,subPacketSize=None,
            channelNoise=None,txPower=None,enableRSSI=None,transmissionMode=None,enableRepeater=None,
            enableLBT=None,WORcontrol=None,WORcycle=None):

        self.port = port or self.port
        self.serialPortRate = serialPortRate or self.serialPortRate
        self.timeout = timeout or self.timeout
        self.serialParityBit = serialParityBit or self.serialParityBit
        self.loraParityBit = self.convertSerialParity(self.serialParityBit)
        
        self.channel = channel or self.channel
        self.crypt = crypt or self.crypt
        self.address = address or self.address
        self.network = network or self.network

        self.airDataRate = airDataRate or self.airDataRate
        self.subPacketSize = subPacketSize or self.subPacketSize
        self.channelNoise = channelNoise or self.channelNoise
        self.txPower = txPower or self.txPower
        self.enableRSSI = enableRSSI or self.enableRSSI
        self.transmissionMode = transmissionMode or self.transmissionMode
        self.enableRepeater = enableRepeater or self.enableRepeater
        self.enableLBT = enableLBT or self.enableLBT
        self.WORcontrol = WORcontrol or self.WORcontrol
        self.WORcycle = WORcycle or self.WORcycle

        self.writeConfig()


    def writeConfig(self):

        #factory reset
        #C0 00 09 12 34 00 61

        RESERVE = 0b00000000

        #C0 = config, 00 = start address, 09 : length
        config = bytearray(b'\xC0\x00\x09')

        address_tmp = self.address.to_bytes(2, 'big')
        config.append(address_tmp[0])
        config.append(address_tmp[1])

        #config.append(int(addrl))

        config.append(self.network)

        config.append(int(hex(self.SERIAL_PORT_RATE[self.serialPortRate] + \
                              self.SERIAL_PARITY_BIT[self.loraParityBit] + \
                              self.AIR_DATA_RATE[self.airDataRate]), 16))


        config.append(int(hex(self.SUB_PACKET_SIZE[self.subPacketSize] + \
                              self.CHANNEL_NOISE[self.channelNoise] + \
                              RESERVE + \
                              self.TX_POWER[self.txPower]), 16))
        
        config.append(self.channel)

        config.append(int(hex(self.ENABLE_RSSI[self.enableRSSI] + 
                              self.TRANSMISSION_MODE[self.transmissionMode] + \
                              self.ENABLE_LBT[self.enableLBT] + \
                              self.WOR_CONTROL[self.WORcontrol] + \
                              self.WOR_CYCLE[self.WORcycle]), 16))

        crypt_tmp = self.crypt.to_bytes(2, 'big')
        config.append(crypt_tmp[0])
        config.append(crypt_tmp[1])
        
        #print("[+] Sending new Configuration Register", flush=True)
        #print(' '.join(['{:02X}'.format(x) for x in config]), flush=True)

        self.gpio_mode('conf')
        ser = self.openSerial()
        
        ser.write(bytes(config))
        time.sleep(.1)
        ret = ser.read_until()
        
        if ret == b'\xff\xff\xff':
            print("ERREUR CONFIG")

        ser.close()
        self.gpio_mode('')
