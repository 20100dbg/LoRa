## LoRa e5 mini

This board as many names : LoRa e5 mini, Wio e5 mini, STM32WLE5JC

Référence : https://wiki.seeedstudio.com/LoRa_E5_mini/

The AT directory contains basic AT commands to try your board just after unboxing.
To fully use this board, you will need to reprogram it using one of the other directories :

The riot-os directory contains a program example suitable for the RIOT OS environment

The radiolib directory contains a program example using [Radiolib](https://github.com/jgromes/RadioLib) suitable for Arduino IDE with STM32 support added





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

If this doesn't work, try :
- mode : **hot plug**
- keep pressing the RST button
- click connect
- release the RST button



3) Open left panel, "Option bytes" menu

- Look for "Read Out Protection" or "RDP"
- Set it on "AA"
- Apply

4) Now you can re-program the board


### Arduino IDE setup

#### Choose your board
```
Tools > Board > STM32 MCU Based boards > LoRa boards
```
#### Then the board part
```
Tools > Board part number > LoRa e5 mini
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
