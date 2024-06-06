# Finally a working guide to LoRa

### Setup hardware

#### Setup OS
- Using Rpi 4 model B
- Install OS using Raspberry Pi Imager
- Using Raspberry Pi OS LITE 64bit
- Don't forget to setup login / password, and enable SSH

#### Setup LoRA HAT
- Connect LoRa to Rpi using GPIO pins
- Take out both black jumpers (M1 and M0)
- Change both yellow jumpers from position 1 to position 2

You should have this result :
![LoRa set up](https://github.com/20100dbg/lora/blob/master/lora.jpg?raw=true)


#### Setup network (static IP address)
Not LoRa related, but needed in my case.
Let's start network manager utility :

```
sudo nmtui
```

Browse to Wired Connection, set :
- IP = 169.254.72.40/16
- gateway = server IP (for me server IP = 169.254.72.20)

Finally disable + enable connection, should be good to go


### Setup software

#### Setup serial port
Start Raspberry Pi config utility :

```
sudo raspi-config
```
- Browse to Interface options -> Serial Port
- First disable shell over serial, then enable serial port hardware 

And then reboot
```
reboot
```

#### Setup pyserial (serial port access from python)
[Download pyserial](https://github.com/pyserial/pyserial/releases)

```
gzip -d pyserial-3.5b0.tar.gz
tar -xvf pyserial-3.5b0.tar
cd pyserial-3.5b0 && sudo python setup.py install
```

### Install my small LoRa PoC
```
git clone https://github.com/20100dbg/lora
cd lora
```

#### Check if we can access serial port
```
sudo python check_port.py
```

#### Let's play
```
sudo python lora.py
```


### Just so you know

I made a small change on **sx126x.py** I got from [MithunHub's LoRa repo](https://github.com/MithunHub/LoRa/).

I wanted the receive method to return data instead of printing it.
The receive method was replaced with :

```
def receive(self):
    if self.ser.inWaiting() > 0:
        time.sleep(0.5)
        r_buff = self.ser.read(self.ser.inWaiting())

        if r_buff:

            src_node = ((r_buff[0]<<8)+r_buff[1])
            message = r_buff[2:-1]
            rssi_dbm = 256-r_buff[-1:][0]

            return (src_node, message, rssi_dbm)

    return (None, None, None)

```


### References

[MithunHub's LoRa repo](https://github.com/MithunHub/LoRa/)

[LoRa Example software](https://github.com/MithunHub/LoRa/archive/refs/heads/main.zip)

