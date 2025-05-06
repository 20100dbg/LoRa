import serial
import threading
import time
import sys


def str_hex_to_bytes(txt):
    txt = txt.replace(' ', '')
    return bytes.fromhex(txt)

def bytes_to_hex(arr):
    return ' '.join(['{:02X}'.format(b) for b in arr])


def listener():
    while True:
        data = ser.read(ser.in_waiting)
        if data:
            print(f"hex : {bytes_to_hex(data)}")
            try:
                print(f"str : {data.decode()}")
            except Exception as e:
                pass
            ser.flush()

        time.sleep(0.5)



port = "/dev/ttyUSB0"
ser = serial.Serial(port=port, baudrate=115200, timeout=1)


t_receive = threading.Thread(target=listener)
t_receive.start()

while True:

    try:
        #string only
        #txt = input()

        #bytes
        data = sys.stdin.buffer.read()

        b = str_hex_to_bytes(txt)
        ser.write(b)
    except KeyboardInterrupt as e:
        break

ser.close()