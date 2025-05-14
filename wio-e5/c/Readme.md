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


### To program the LoRa e5 board, we NEED a ST-LINK


### Pinout LoRa e5 <-> ST-Link
- Check out the board pinout : https://wiki.seeedstudio.com/LoRa_E5_mini/
- connect SWCLK, SWDIO, GND, and RST pins
- connect USB-C cable for power supply and serial interface


### Disable write protection

1) Open STM32CubeProgrammer

2) Using this settings :
- port : SWD
- frequency : 4000
- mode :  under reset
- access port : 0
- reset mode : hardware reset
- speed : reliable
- shared : disabled

And click connect.



3) Open left panel, "Option bytes" menu

- Look for "Read Out Protection" or "RDP"
- Set it on "AA"
- Apply

4) Now you can re-program the board


### Arduino IDE setup

#### Choose your board
```
Tools > Board: "XXXX" > STM32 MCU Based boards > LoRa boards
```
#### Then the board part
```
Tools > Board part number: "XXXX" > LoRa e5 mini
```

#### Open example sketch
```
File > Examples > 01. Basics > Blink
```

Upload and built-in LED should blink


### Just so you know

Before the "LoRa boards" menu, we had to use "Generic STM32WL series" board and "Generic WLE5JCIx" board number, in order to compile.
But serial didn't work (much ? at all ? can't remember)



### Check SUBGHZ register

**Very important to use LoRa RF**

After first program is written, the SUBGHZ register is overwritten and set to 0, disabling the RF capabilities.

In order to re-activate it, we need to :

- Connect LoRa e5 as usual with ST-Link and USB-C cable
- Use STM32CubeProgrammer to connect
- In left panel, open the "Registers" menu
- Set the device as "STM32WL5x_CM4" 
- Select "FLASH" option in the select list
- In the registers list, unfold "SFR"
- Set "1" for the "SUBGHSPISD" setting
- Apply


If this doesn't work, try again using the "Hot Plug" to connect to the board.



### At this point, take out the RST pin

Maybe we could remove it earlier, not sure.


### Flash a program

We can either use Arduino IDE and related libraries or RIOT OS.

I recommend RIOT OS.
RIOT OS uses SEMTECH provided LoRa driver (on this repo : https://github.com/Lora-net/sx126x_driver)


Open a terminal, browse to the target software

For example : 
- RIOT/examples/basic/lora
- RIOT/tests/drivers/sx126x

Connect the LoRa-e5 to both ST-LINK and USB-C

##### Flash
sudo BOARD=lora-e5-dev LORA_DRIVER=sx126x_stm32wl make flash

##### Open serial communication
sudo BOARD=lora-e5-dev LORA_DRIVER=sx126x_stm32wl make term

##### Start debug using gdb
sudo BOARD=lora-e5-dev LORA_DRIVER=sx126x_stm32wl make debug


### Additionnal notes

#### Serial interface is available though USB-C cable or UART pins (RX/PB7 and TX/PB6)

#### Always add `LORA_DRIVER=sx126x_stm32wl` in compile command to enable send/receive data

#### By default on RIOT, compilation warnings are errors. To ignore warnings Add `WERROR=0` when flashing

#### Hide "This is RIOT !" welcome message :
- edit RIOT/core/lib/init.c
- comment LOG_INFO(CONFIG_BOOT_MSG_STRING);

