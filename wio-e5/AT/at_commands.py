import threading
import serial
import time

def listener():
    while True:
        nb = serial.in_waiting

        if nb > 0:
            x = serial.read(nb)
            print(x.decode())
        time.sleep(0.1)


def cmd_at(cmd, expected_result="", wait=0.8):
    success = True
    cmd = cmd.encode() + b"\n"
    serial.write(cmd)
    time.sleep(wait)
    nb = serial.in_waiting
    result = serial.read(nb).decode()

    if expected_result not in result:
        success = False

    return nb, success, result


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


serial = serial.Serial(port=serial_port, baudrate=baudrate, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=1)

while True:

    cmd = input(">")
    nb, success, result = cmd_at(cmd)
    print(f"{nb} / {success} / {result}")

