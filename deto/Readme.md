### config wifi
```
# wpa_passphrase WLAN_NAME WLAN_PASSWORD > /etc/wpa_supplicant.conf
$ sudo wpa_supplicant -B -iwlan0 -c/etc/wpa_supplicant.conf -Dnl80211
$ sudo dhclient wlan0
```

### config GPIO
guide : https://www.raspberrypi-france.fr/comment-utiliser-les-port-gpio-raspberry-pi/
télécharger RPi.GPIO : https://pypi.org/project/RPi.GPIO/

```
python -m venv NOM_DOSSIER
cd NOM_DOSSIER
source bin/activate
```

Récupérer la version de python
```
python --version
```

Installer les fichiers de dev
```
sudo apt-get update
sudo apt-get install python3.11-dev
```

Installer la lib RPi.GPIO
```
cp ~/RPi.GPIO.tar.gz ./RPi.GPIO.tar.gz
tar -xvf RPi.GPIO.tar.gz
cd RPi.GPIO
python setup.py install
```