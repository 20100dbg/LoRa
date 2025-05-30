import LoRaE5

#lora = LoRaE5(port="/dev/ttyS0", debug=False)
lora = LoRaE5(port="COM11", debug=True)

print(lora.show_config())

#lora.set_config(frequency=433, spreading_factor=12, bandwith=250, tx_power=14)

while True:
    
    lora.send_string("hello")

    result = lora.receive()
    
    if result:
        print(f"received : {result}")

    time.sleep(5)

