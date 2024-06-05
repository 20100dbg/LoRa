#!/usr/bin/python

import sx126x
import threading
import time


#   serial_num
#       PiZero, Pi3B+, and Pi4B use "/dev/ttyS0"
#
#    Frequency is [850 to 930], or [410 to 493] MHz
#
#    address is 0 to 65535
#        under the same frequence,if set 65535,the node can receive 
#        messages from another node of address is 0 to 65534 and similarly,
#        the address 0 to 65534 of node can receive messages while 
#        the another note of address is 65535 sends.
#        otherwise two node must be same the address and frequence
#
#    The tramsmit power is {10, 13, 17, and 22} dBm
#
#    RSSI (receive signal strength indicator) is {True or False}
#        It will print the RSSI value when it receives each message
#

def myreceive():

    while running:
        data = node.receive()

        if data[0] != None:
             src_node = data[0]
             message = data[1]
             rssi_dbm = data[2]
             print("From", src_node, ':', message.decode('utf-8'))

        time.sleep(0.1)


def mysend(dst_node, message):
    node.addr_temp = node.addr
    node.set(node.freq,int(dst_node),node.power,node.rssi)
    node.send(message)
    time.sleep(0.2)
    node.set(node.freq,node.addr_temp,node.power,node.rssi)


#LORA init parameters
local_node = 100
power = 22
freq = 868


print(" _____________________")
print("|                     |")
print("|     LoRa Tester     |")
print("|_____________________|\n")

print("local node :", local_node)
print("power :", power)
print("freq :", freq)
print("      ____________\n\n")

print("Type 'XX your message', XX being your destination node")
print("Type 'exit' to close this program")

#init LORA
node = sx126x.sx126x(serial_num = "/dev/ttyS0",freq=freq,addr=local_node,power=power,rssi=True)

#start receive thread
running = True
t_receive = threading.Thread(target=myreceive)
t_receive.start()


#loop for sending messages
while True:
    message = input("> ")

    #if exit, kill thread and break loop
    if message == "exit":
        running = False
        t_receive.join()
        break

    #if message is not empty
    elif message:

        #multiples sanity checks
        tab = message.split(" ", 2)

        if len(tab) != 2:
            continue

        try:
            dst_node = int(tab[0])
        except:
            continue

        if dst_node == local_node:
             print("You can't send it to yourself mate")
             continue

        #finally sending
        print(f"Sending '{tab[1]}' to {dst_node}")
        mysend(dst_node, tab[1])

