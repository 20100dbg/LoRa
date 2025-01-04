#define lora Serial2

// mode selection pins
const int M0 = 47;
const int M1 = 48;

const byte TYPE_PING = 1;
const byte TYPE_PONG = 2;

const int BUFFER_SIZE = 25;

unsigned char received[BUFFER_SIZE];
unsigned char to_send[BUFFER_SIZE];

void setup() 
{
  Serial.begin(115200);                     // Initialize ESP32 serial communication on COM Port
  lora.begin(9600, SERIAL_8N1, 18, 17); // Initialize ESP32 Serial communication with LoRa module

  pinMode(M0, OUTPUT); // Define pin as OUTPUT
  pinMode(M1, OUTPUT); // Define pin as OUTPUT

  //setconfig();

  //set normal mode
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);

  delay(100);
  while (lora.available()) lora.read();
}

void setconfig() {
  
  digitalWrite(M0, LOW);
  digitalWrite(M1, HIGH);

  unsigned char montest[] = {0xC0, 0x00, 0x09, 0xFF, 0xFF, 0x00, 0x62, 0x40, 0x12, 0x0B, 0x00, 0x00};
  lora.write((byte*)&montest, sizeof(montest));

  delay(200);
  char txt[2];
  
  while (lora.available())
  {
    byte b = lora.read();
    sprintf(txt, "%X ", b);
    Serial.write(txt);
    delay(100);
  }
  printf("\ndone\n");
}


void readconfig() {
  
  digitalWrite(M0, LOW);
  digitalWrite(M1, HIGH);
  
  unsigned char montest[] = {0xC1, 0x00, 0x09};
  lora.write((byte*)&montest, sizeof(montest));
  //lora.write(montest, sizeof(montest));

  delay(200);
  char txt[2];
  
  while (lora.available())
  {
    byte b = lora.read();
    sprintf(txt, "%X ", b);
    Serial.write(txt);
    delay(50);
  }
  printf("\ndone\n");
}


void loop() 
{
  /*
  setconfig();
  delay(1000);
  readconfig();
  delay(1000);
  return;
*/
  
  //listening for packets
  if (lora.available())
  {
    Serial.print("\nReceiving data");
    int count = 0;
    while (lora.available())
    {
      received[count++] = lora.read();
      delay(1);
    }
    Serial.print("\nReceived : ");
    for (int i = 0; i < count; i++) { Serial.print(received[i], HEX); Serial.print(" "); }

    if (received[0] == TYPE_PING)
    {
      Serial.print("\nData type : PING");
      to_send[0] = TYPE_PONG;

      for (int i = 1; i < count + 1; i++)
      {
        to_send[i] = received[count - i];
      }
    }
    Serial.print("\nSending : ");
    for (int i = 0; i < count; i++) { Serial.print(to_send[i], HEX); Serial.print(" "); }

    Serial.print("\n");
    Serial.print(count);

    delay(2000);

    //lora.write(to_send);
    lora.write((byte*)&to_send, count);
  }

  delay(1);
}