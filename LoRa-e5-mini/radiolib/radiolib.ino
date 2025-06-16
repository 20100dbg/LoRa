#include <RadioLib.h>

// no need to configure pins, signals are routed to the radio internally
STM32WLx radio = new STM32WLx_Module();

// set RF switch configuration for Nucleo WL55JC1
// NOTE: other boards may be different!
//       Some boards may not have either LP or HP.
//       For those, do not set the LP/HP entry in the table.
static const uint32_t rfswitch_pins[] = {PC_3,  PC_4,  PC_5, RADIOLIB_NC, RADIOLIB_NC};
static const Module::RfSwitchMode_t rfswitch_table[] = {
  {STM32WLx::MODE_IDLE,  {LOW,  LOW,  LOW}},
  {STM32WLx::MODE_RX,    {HIGH, HIGH, LOW}},
  {STM32WLx::MODE_TX_LP, {HIGH, HIGH, HIGH}},
  {STM32WLx::MODE_TX_HP, {HIGH, LOW,  HIGH}},
  END_OF_MODE_TABLE,
};

int timestamp = millis();
static const int max_size = 256;
byte serial_buffer[max_size];

void print_hex(byte* buffer, int count) {
  
  for (int i = 0; i < count; i++) {
    char x[2];
    sprintf(x, "%02x", buffer[i]);
    Serial.print(x);
    Serial.print(" ");
  }
  Serial.println();
}


volatile bool receivedFlag = false;
void setFlag(void) {
  receivedFlag = true;
}


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);

  // set RF switch control configuration
  // this has to be done prior to calling begin()
  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);

  float freq = 868.0; //150-960mhz
  float bw = 125.0; //125,250,500
  uint8_t sf = 12; //7,8,9,10,11,12
  uint8_t cr = 5; //5,6,7,8
  uint8_t syncWord = RADIOLIB_SX126X_SYNC_WORD_PRIVATE;
  int8_t power = 10; //22, 17, 14, 10
  uint16_t preambleLength = 8;

  int state = radio.begin(freq, bw, sf, cr, syncWord, power, preambleLength);

  //force calibration
  state = radio.setFrequency(freq);
  Serial.println(state);
  delay(50);

  

  radio.setDio1Action(setFlag);
  radio.startReceive();
}


void loop() {

  int state;

  if(receivedFlag) {
    receivedFlag = false;

    int count = radio.getPacketLength(true);

    if (count > 0) {
      byte received_buffer[count];
      state = radio.readData(received_buffer, count);


      if (state == RADIOLIB_ERR_NONE) {
        /*
        Serial.print("RSSI=");
        Serial.print(radio.getRSSI());
        Serial.print(" SNR=");
        Serial.print(radio.getSNR());
        Serial.println();
        */

        //print_hex(received_buffer, count);
        Serial.write(received_buffer, count);
        
      }
    }
  }
  
  size_t count = Serial.readBytes(serial_buffer, max_size);
  
  if (count > 0) {

    //radio.standby();
    radio.clearDio1Action();

    byte send_buffer[count];
    memcpy(send_buffer, serial_buffer, count);
    radio.transmit(send_buffer, count);
    delay(50);
    
    radio.setDio1Action(setFlag);
    radio.startReceive();
  }

}
