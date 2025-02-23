import serial
import time

class at(object):
    def __init__(self, port="/dev/ttyS0", debug=False):
        self.debug = debug
        self.wait = 0.1
        self.serial = serial.Serial(port=port, baudrate=9600, 
                                parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, 
                                bytesize=serial.EIGHTBITS, timeout=1)
        
    def simple_send_at(self, cmd):        
        serial.write(cmd)
        time.sleep(0.1)
        return self.serial.read(self.serial.in_waiting)


    def send_at(self, cmd, expected_result, wait=0.1):
        success = True
        serial.write(cmd)
        time.sleep(wait)
        result = self.serial.read(self.serial.in_waiting)
        
        if expected_result not in result:
            success = False

        return success, result


at_obj = at(port="/dev/ttyS0", debug=False)
success, result = at_obj.send_at("AT", "OK")
print(f"{success} / {result}")

msg = "salut"
success, result = at_obj.send_at(f"AT+MSG {msg}", "OK")
print(f"{success} / {result}")

success, result = at_obj.send_at(f"AT+DR", "")
print(f"{success} / {result}")

#config
#AT+DR=DR0-DR15
#AT+DR=EU868
#AT+DR=SF7-SF12
#AT+DR=BW125K
#AT+CH=0-80

success, result = at_obj.send_at(f"AT+DR=DR0", "")
print(f"{success} / {result}")
