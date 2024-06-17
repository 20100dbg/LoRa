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


### Repository content

- sx126x.py : the library. Mandatory to play with LoRa
- lora.py : sample code using the library, with some added features
- simple.py : just enough code to send and receive data over LoRa
- check_port.py : just a piece of code that checks if you can use Serial Port


### Just so you know

I began my journey with [MithunHub's LoRa repo](https://github.com/MithunHub/LoRa/), that helped me to actually communicate through LoRa for the first time.
Then I found a more complete library from the LoRa HAT manufacturer itself : SX126X-LoRa-PI-HAT-Waveshare-main

This library is working very well, it's easy to read and to modify to your need. It allows you to play with the most common settings.

I wrote "my" library using [E22-900T22S User Manual](https://www.ebyte.com/en/downpdf.aspx?id=467) from eByte, the LoRa chipset manufacturer, and took the missing informations from Waveshare's code.


### Additionnal notes (french)


-- Adresses
- l'adresse configurée lors de l'init est davantage celle du destinataire que celle de l'émetteur
- pour que A envoie un message que B puisse recevoir, il faut que A et B ait la même adresse configurée !
- problèmes : on ne sait pas de qui vient le message, cela oblige à configurer en permanence le module si on souhaite envoyer des messages à plusieurs destinataires
- sinon, on peut utiliser l'adresse spéciale 65535 qui joue le rôle de broadcast en émission, et de monitoring en réception

- ma solution : configurer tous les modules en broadcast/monitoring, et créer un champ supplémentaire qui contient l'adresse logique du module. Cette adresse est envoyée au début de chaque message
- il est également possible d'avoir un network logique en plus de l'adresse (voir network plus bas)


-- Network
- un message broadcast est transmis à tous les modules, peu importe l'adresse et le network
- pour communiquer, deux modules doivent avoir la même adresse et le même network (si pas de broadcast)


-- RSSI (Received Signal Strength Indication)
- donne le niveau de réception du dernier paquet
- donne le niveau de bruit courant

- deux méthodes :
1- Activation du RSSI ambient noise (aka Channel noise) (registre 04) : retourne _à la demande_ le niveau de bruit courant et/ou la force du signal du dernier paquet reçu
2- Activation du RSSI (registre 06) : lors de la réception d'un paquet, ajoute _automatiquement_ à la fin du buffer un octet donnant la force du signal du présent paquet

- Il est possible d'activer 0,1 ou 2 RSSI.
- Calcul : rssi = -1 * (256 - OCTET_RSSI)


