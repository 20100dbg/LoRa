## Installation

We will need to :
- Setup hardware
- Setup OS config
- Setup a python environnement

### Setup

#### Hardware

- Connect LoRa to Rpi using GPIO pins
- Take out both black jumpers (M1 and M0)
- Change both yellow jumpers from position 1 to position 2

You should have this result :
![LoRa set up](https://github.com/20100dbg/LoRa/blob/master/RPI_HAT/hat_setup.jpg?raw=true)


### OS

#### Enable serial port
Start Raspberry Pi config utility :
```
sudo raspi-config
```
- Browse to Interface options -> Serial Port
- Disable shell over serial
- Enable serial port hardware 

And then reboot
```
reboot
```

### Python environnement

#### Clone this repo
```
git clone https://github.com/20100dbg/lora
cd lora
```

#### Install python libraries
```
python -m venv .venv
source .venv/bin/activate

pip install pyserial
pip install lgpio
pip install gpiozero
```

#### Let's play
```
python simple.py

Or

python test_lora.py
```


### Repository content

- sx126x.py : the library. Mandatory to play with LoRa
- config_tool.py : a tool to translate register configuration to and from human readable values
- simple.py : just enough code to send and receive data over LoRa
- test_lora.py : more complete test app that allows to send/receive string or bytes, set up parameters and such


### References

[k3rn3Lp4n1cK's SX126X-LoRa-PI-HAT-Waveshare](https://github.com/k3rn3Lp4n1cK/SX126X-LoRa-PI-HAT-Waveshare)
[MithunHub's LoRa repo](https://github.com/MithunHub/LoRa/)
[E22-900T22S User Manual](https://www.cdebyte.com/pdf-down.aspx?id=1463)
[Waveshare user guide](https://www.waveshare.com/wiki/SX1262_915M_LoRa_HAT)


### Additionnal notes about LoRa

About adresses :
Using transparent transmssion mode (default), your local address MUST match your recipient one : if A is sending a message to B, A must have B's address and network (at least temporarly)
We can use special address 65535 that allow broadcast (send to everyone) and monitoring (listen from everyone). A broadcast message is sent to any address and any network.

So unless I have a particular need, I setup every node with 65535 address, and add a logical address in my homemade data header.

Using fixed point transmission mode :
- Each node has a unique address
- Nodes must be on the same network, or send message through repeater
- Nodes can use the same channel or different ones.
- Each message MUST start with three bytes describing your recipient as : ADDRH+ADDRL+CHANNEL
- If your recipient is on another channel, your node is hopping briefly on recipient's channel to send the message 


About RSSI (Received Signal Strength Indication) :
There is two modes
- RSSI ambient noise (aka Channel noise) (register 04) : gives _only on demand_ current channel noise and/or last message's signal level
- RSSI (register 06) : the chip append _automatically_ to each message received a byte with RSSI value

We can activate none, one, or both modes.
Note : real_rssi = -1 * (256 - RSSI byte value)


About repeater mode
- Need nodes to use fixed point modes (maybe not mandatory but recommended)
- Nodes need to share the same address and different networks
- On clients, network must be same value than channel
- Since we are in fixed point mode, each message start with three bytes : ADDRH+ADDRL+CHANNEL
- Repeater must set its ADDRH as a first network and ADDRL as a second network. No need to set a network on repeater.
- Repeater can only listen one channel at time, so choose it wisely
- Unless you switch channel regularly, message will travel in one way only


File transfer : It seems really difficult without a TCP like protocol

Packets MAY be arbitrarly split by the chip