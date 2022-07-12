#include <LedControl.h>
#define SPI_bitRead(value, bit) (((value) >> (bit)) & 0x01)

int SPI_Pin_MOSI = 11;
int SPI_Pin_CS = 10;
int SPI_Pin_CLK = 13;

LedControl lc = LedControl(SPI_Pin_MOSI, SPI_Pin_CLK, SPI_Pin_CS, 0);

void test01() {
  //Create an array with the data to shift out
  int addr = 0x00;
  byte opcode = 0x02;
  byte data = 0b11100011;

  //enable the line
  digitalWrite(SPI_Pin_CS, LOW);

  for (int i = 7; i >= 0; i--) {// 8 bits in a byte

    digitalWrite(SPI_Pin_MOSI, SPI_bitRead(opcode, i));   // Set MOSI
    digitalWrite(SPI_Pin_CLK, HIGH);                  // SCK high
    digitalWrite(SPI_Pin_CLK, LOW);                   // SCK low
  }

  for (int i = 7; i >= 0; i--) { // 8 bits in a byte

    digitalWrite(SPI_Pin_MOSI, SPI_bitRead(data, i));   // Set MOSI
    digitalWrite(SPI_Pin_CLK, HIGH);                  // SCK high
    digitalWrite(SPI_Pin_CLK, LOW);                   // SCK low
  }

  digitalWrite(SPI_Pin_CS, HIGH);
}

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 5);     //Adjust the brightness maximum is 15
  lc.clearDisplay(0);
  test01();
  while (1);
}

void loop() {
}
