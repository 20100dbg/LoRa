### Download RIOT
```
git clone https://github.com/RIOT-OS/RIOT
```

### Install dependencies
```
sudo apt install gcc-arm-none-eabi openocd python3-pip

sudo -H pip install pyserial
```

### RIOT documentation
- Supported boards : https://doc.riot-os.org/group__boards.html
- LoRa e5 specifics : https://doc.riot-os.org/group__boards__lora-e5-dev.html



### Flash a program

We can either use Arduino IDE and related libraries or RIOT OS.

I recommend RIOT OS.
RIOT OS uses SEMTECH provided LoRa driver (on this repo : https://github.com/Lora-net/sx126x_driver)


Open a terminal, browse to the target software

For example : 
- RIOT/examples/basic/lora
- RIOT/tests/drivers/sx126x

Connect the LoRa-e5 to both ST-LINK (without RST pin) and USB-C

##### Flash
```
sudo BOARD=lora-e5-dev LORA_DRIVER=sx126x_stm32wl make flash
```
##### Open serial communication
```
sudo BOARD=lora-e5-dev LORA_DRIVER=sx126x_stm32wl make term
```

##### Start debug using gdb
```
sudo BOARD=lora-e5-dev LORA_DRIVER=sx126x_stm32wl make debug
```

### Additionnal notes

#### Serial interface is available though USB-C cable or UART pins (RX/PB7 and TX/PB6)

#### Always add `LORA_DRIVER=sx126x_stm32wl` in compile command to enable send/receive data

#### By default on RIOT, compilation warnings are errors. To ignore warnings Add `WERROR=0` when flashing

#### Hide "This is RIOT !" welcome message :
- edit RIOT/core/lib/init.c
- comment LOG_INFO(CONFIG_BOOT_MSG_STRING);

