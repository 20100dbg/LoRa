import serial
import threading
import time

class LoRaE5(object):

    #
    # Init : the module is reading the chip current settings and accordingly fill local variables
    #

    def __init__(self, port="/dev/ttyS0", debug=False):
        
        #Variables
        self.transmitting = False
        self.debug = debug
        self.default_params = { "frequency": 866, "spreading_factor":12, "bandwith": 250, "tx_power": 22}
        self.params = self.default_params

        #Serial settings
        self.serial = self.open_serial(port)

        #enable TEST mode
        success, result = self.cmd_at("AT+MODE=TEST", "TEST", 0.5)

        read_params = self.read_config()
        
        if self.debug:
            print("[+] Reading registers")
            print(read_params)

        #Fill class variables with config currently applied to chip
        if read_params:
            self.set_config(frequency=read_params["frequency"], spreading_factor=read_params["spreading_factor"], 
                            bandwith=read_params["bandwith"], tx_power=read_params["tx_power"])
        else:
            print("[-] Error reading config")


    #
    # Serial
    #

    def open_serial(self, port, timeout=1):
        """ Open serial port """

        return serial.Serial(port=port, baudrate=9600, 
                                parity=serial.PARITY_NONE,
                                stopbits=serial.STOPBITS_ONE, 
                                bytesize=serial.EIGHTBITS, 
                                timeout=timeout)


    def simple_cmd_at(self, cmd):        
        self.serial.write(cmd)
        time.sleep(0.1)
        return self.serial.read(self.serial.in_waiting)


    def cmd_at(self, cmd, expected_result="", wait=0.5):
        success = True
        cmd = cmd.encode() + b"\n"

        self.serial.write(cmd)
        time.sleep(wait)
        result = self.serial.read(self.serial.in_waiting).decode()
        
        if expected_result not in result:
            success = False

        return success, result


    #
    # Bytes manipulation
    #

    def bytes_to_hex(self, b):
        """ Get bytes to proper hex notation """
        return " ".join(["{:02X}".format(x) for x in b])


    #
    # Send and receive data
    #

    def send_bytes(self, data):
        """ Send bytes message. Check if data is bytes """
        
        if not isinstance(data, bytes):
            print("[!] Data parameter must be bytes")
            return

        if self.debug:
            print(f"[+] SENDING {len(data)} bytes : {self.bytes_to_hex(data)}")

        self.transmitting = True
        self.cmd_at(f'AT+TEST=TXLRPKT,"{self.bytes_to_hex(data)}"')

        #Get back to receive mode
        self.cmd_at(f"AT+TEST=RXLRPKT")
        self.transmitting = False


    def send_string(self, data):
        """ Send string message. Check if data is string and .encode() it """

        if not isinstance(data, str):
            print("[!] Data parameter must be str")
            return

        ##### this
        self.send_bytes(data.encode())


        ##### or this
        """
        self.transmitting = True
        success, result = self.cmd_at(f'AT+TEST=TXLRSTR,"{data}"')

        if self.debug:
            print(f"[+] Sending {data}")
            print(f"[+] {success} / {result}")

        #Get back to receive mode
        success, result = self.cmd_at(f"AT+TEST=RXLRPKT")
        self.transmitting = False
        """


    def receive(self):
        """ Check if data is received """

        data = []
        rssi, snr = None, None

        #Check if we're not transmitting
        if not self.transmitting and self.serial.in_waiting:
            
            result = self.serial.read(self.serial.in_waiting).decode()

            #if data doesn't end with \n, we don't have the whole data. Let's wait and retry
            while result[len(result) - 1] != "\n":
                time.sleep(0.5)
                result += self.serial.read(self.serial.in_waiting).decode()

            lines_result = result.strip().split("\n")

            #Buffer is supposed to be two lines :
            #+TEST: LEN:18, RSSI:-36, SNR:12
            #+TEST: RX "41424344"

            #BUT lines can be mixed up. Let's check before assuming anything
            for line in lines_result:

                if line[:10] == "+TEST: LEN":

                    elements = line.split(",")
                    elements = [e.strip() for e in elements]
                    rssi = elements[1][5:]
                    snr = elements[2][4:]

                elif line[:9] == "+TEST: RX":
                    message = line.strip()[11:-1]
                    print(f"data : {message}")
                    data.append(bytes.fromhex(message))


            if self.debug:
                print(f"[+] RECEIVED")
                for d in data:
                    print(f"[+] {self.bytes_to_hex(d)}")

            #Only keeping last RSSI and SNR in the buffer
            return (rssi, snr), data

        return None

    #
    # Config
    # 

    def set_config(self, frequency=None, spreading_factor=None, bandwith=None, 
                        tx_power=None, write_config=True):
        
        if frequency: self.params["frequency"] = frequency
        if spreading_factor: self.params["spreading_factor"] = spreading_factor
        if bandwith: self.params["bandwith"] = bandwith
        if tx_power: self.params["tx_power"] = tx_power

        if write_config:
            self.write_config()
    

    def write_config(self):
        tx_preamble, rx_preamble = 8, 8
        config = f"AT+TEST=RFCFG,{self.params['frequency']},SF{self.params['spreading_factor']},{self.params['bandwith']},{tx_preamble},{rx_preamble},{self.params['tx_power']}"
        
        success, result = self.cmd_at(config, "")

        if self.debug:
            print(f"[+] Config sent {config}")
            print(f"[+] Config recv {result}")


    def read_config(self):
        
        success, result = self.cmd_at("AT+TEST=?")
        
        #if self.debug:
        #    print(f"[+] Reading CONF : {result}")

        idx = result.find("RFCFG F:") + 8
        elements = result[idx:].split(",")
        elements = [e.strip() for e in elements]

        frequency = int(elements[0]) // 1_000_000
        spreading_factor = int(elements[1][2:])
        bandwith = int(elements[2][2:-1])
        tx_preamble = int(elements[3][5])
        rx_preamble = int(elements[4][5])
        tx_power = int(elements[5][4:-3])
        
        #"tx_preamble": tx_preamble,"rx_preamble": rx_preamble,
        return {"frequency": frequency,"spreading_factor": spreading_factor,
                "bandwith": bandwith, "tx_power": tx_power}


    def show_config(self):
        return self.params

