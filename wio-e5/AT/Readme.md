Complete specifications : https://files.seeedstudio.com/products/317990687/res/LoRa-E5%20AT%20Command%20Specification_V1.0%20.pdf

###### Enable test mode
AT+MODE=TEST

###### print current parameters 
AT+TEST=?

###### set RF parameters (for test mode)

AT+TEST=RFCFG,[FREQUENCY],[SF],[BANDWIDTH],[TX PR],[RX PR],[TX POWER]
AT+TEST=RFCFG,866,SF12,250,8,8,22


###### send hex packet
AT+TEST=TXLRPKT
AT+TEST=TXLRPKT, "00 AA 11 BB 22 CC"

###### send string packet
AT+TEST=TXLRSTR
AT+TEST=TXLRSTR, "hello"

###### receive a packet
AT+TEST=RXLRPKT
