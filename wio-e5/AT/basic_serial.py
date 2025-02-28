import serial
import time
import serial.tools.list_ports


#List serial ports
raw_ports = serial.tools.list_ports.comports()
port_names = []
id = 0

print("Available serial ports :")
for port, desc, hwid in raw_ports:
    print(f"[{id}] : {port} - {desc}")
    port_names.append(port)
    id += 1

print()
serial_port = int(input("Serial port : "))
serial_port = port_names[serial_port]
baudrate = int(input("Baudrate : "))

print(f"Connecting to {serial_port}")

#Init serial
serial = serial.Serial(port=serial_port, baudrate=baudrate, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=1)


while True:
    cmd = input(">")
    serial.write(cmd.encode() + b'\n')
    time.sleep(0.8)
    print(serial.read(serial.in_waiting))