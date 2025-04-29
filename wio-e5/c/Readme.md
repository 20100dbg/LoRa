### Download RIOT

git clone https://github.com/RIOT-OS/RIOT

### Install dependencies
sudo apt install gcc-arm-none-eabi openocd python3-pip

sudo -H pip install pyserial


Voir les boards prises en charge par RIOT
- https://doc.riot-os.org/group__boards.html
- Pour LoRa e5 : https://doc.riot-os.org/group__boards__lora-e5-dev.html
- contient les branchement, consignes pour flasher, etc


branchement LoRa e5 <-> ST-Link
- https://wiki.seeedstudio.com/LoRa_E5_mini/
- brancher SWCLK, SWDIO, GND, and RST
- brancher cable USB pour alim


### Disable write protection

Open STM32CubeProgrammer

Using this settings :
- port : SWD
- frequency : 4000
- mode :  under reset
- access port : 0
- reset mode : hardware reset
- speed : reliable
- shared : disabled

Click connect



Open left panel, "Option bytes" menu

- Look for "Read Out Protection" or "RDP"
- Set it on "AA"
- Apply


### Arduino IDE setup


##### Choose your board
Tools > Board: "XXXX" > STM32 MCU Based boards > LoRa boards

##### And
Tools > Board part number: "XXXX" > LoRa e5 mini


##### Open example sketch
File > Examples > 01. Basics > Blink

Upload and built-in LED should blink


*Just so you know*

Before the "LoRa boards" menu, we had to use "Generic STM32WL series" board and "Generic WLE5JCIx" board number, in order to compile. But serial didn't work (much ? at all ? can't remember)



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


If this doesn't work, try using the "Hot Plug" to connect to the board.



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

Connexion port série
- connecter la carte en USB uniquement
- on peut aussi se connecter au e5 via UART (RX/PB7 et TX/PB6)

Ne pas oublier LORA_DRIVER=sx126x_stm32wl : même si le driver est dans le makefile, si on ne le mets pas en paramètres, ça compile mais pas d'envoi/réception de message... Le debug a été long et douloureux.


