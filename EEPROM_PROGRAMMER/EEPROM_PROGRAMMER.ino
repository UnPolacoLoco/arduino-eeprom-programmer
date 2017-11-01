
#define DATA_PIN 4
#define SR_CLK 3
#define LATCH 2

#define EEPROM_D0 5
#define EEPROM_D1 6
#define EEPROM_D2 7
#define EEPROM_D3 8
#define EEPROM_D4 9
#define EEPROM_D5 10
#define EEPROM_D6 11
#define EEPROM_D7 12

#define WRITE_EN 13

#define ArrayLenght(x) (sizeof(x) / sizeof(x[0]))

int dataLenght;

void setup() {


  pinMode(LATCH, OUTPUT);
  digitalWrite(LATCH, LOW);

  pinMode(WRITE_EN, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);

  pinMode(DATA_PIN, OUTPUT);
  pinMode(SR_CLK, OUTPUT);

  // 4-bit hex decoder for common cathode 7-segment display
  byte digits[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b}; //0x77, 0x1f, 0x4e, 0x3d, 0x4f, 0x47 }

  // 4-bit hex decoder for common anode 7-segment display
  // byte data[] = { 0x81, 0xcf, 0x92, 0x86, 0xcc, 0xa4, 0xa0, 0x8f, 0x80, 0x84, 0x88, 0xe0, 0xb1, 0xc2, 0xb0, 0xb8 };



  Serial.begin(9600);

  Serial.println("Starting EEPROM writing routines, this might take a while, please wait...");

  for (int value = 0; value < 256; value++)
  {
    writeEEPROM(value, digits[value % 10]);
  }
  Serial.print(".");
  for (int value = 0; value < 256; value++)
  {
    writeEEPROM(value + 256, digits[(value / 10) % 10]);
  }
  Serial.print(".");
  for (int value = 0; value < 256; value++)
  {
    writeEEPROM(value + 512, digits[(value / 100)]);
  }
  Serial.print(".");
  for (int value = 0; value < 256; value++)
  {
    writeEEPROM(value + 768, 0);
  }
  Serial.println(".");

  Serial.println("Operation successful!");

  //  for (int i = 0; i < dataLenght; i++)
  //  {
  //    writeEEPROM(i, data[i]);
  //  }

  Serial.println("Testing...");

  for (int address = 0; address < ArrayLenght(digits); address++)
  {
    Serial.print("0x0");
    Serial.print(address, HEX);
    if (readEEPROM(address) == digits[address])
      Serial.println(" success!");

    else
      Serial.print(" error!");
  }
  showEEPROM();



}

void setAddress(int address, bool outputEnable)
{
  if (outputEnable)
    bitClear(address, 15);
  //address |= 0x00; //sets QA (MSB) on 1st HC595 low, so output is enabled.
  else
    bitSet(address, 15);
  //address |= 0x8000; //sets QA (MSB) on  1st HC595 high, so output is disabled.

  shiftOut(DATA_PIN, SR_CLK, LSBFIRST, address);
  shiftOut(DATA_PIN, SR_CLK, LSBFIRST, (address >> 8));

  digitalWrite(LATCH, 0);
  digitalWrite(LATCH, 1);
  digitalWrite(LATCH, 0);

  delay(1);

}




void showEEPROM()
{
  for (int address = 0; address < 1024; address++)
  {
    Serial.print("Address: 0x");
    Serial.print(address, HEX);
    Serial.print(" - ");
    Serial.println(readEEPROM(address), HEX);
  }

}



void writeEEPROM(int address, byte data)
{
  setAddress(address, false);




  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin++)
  {
    pinMode(pin, OUTPUT);

  }

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin++)
  {
    digitalWrite(pin, data & 1);
    data = data >> 1;

  }

  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);


}

byte readEEPROM(int address)
{
  byte value = 0;
  setAddress(address, true);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin++)
  {
    pinMode(pin, INPUT);
  }

  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin--)
  {
    value = (value << 1) + digitalRead(pin);
  }
  return value;
}



void loop() {
  // put your main code here, to run repeatedly:

}
