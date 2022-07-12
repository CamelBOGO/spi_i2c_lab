#define SPI_bitRead(value, bit) (((value) >> (bit)) & 0x01)

// Global Variables
int SPI_Pin_MOSI = 11;  // MOSI
int SPI_Pin_CS = 10;    // CS
int SPI_Pin_CLK = 13;   // clk

// Function Prototypes
void spiInt(int SPI_Pin_MOSI, int SPI_Pin_CS, int SPI_Pin_CLK);
void spiWrite(byte opcode, byte data);
void clearDisplay();
void test01();

/*------------------------------Main---------------------------------*/

void setup() {
	// spiInt();

	//   lc.shutdown(0, false);
	spiWrite(0x0c, 1);

	//   lc.setIntensity(0, 5);     // Adjust the brightness maximum is 15
	spiWrite(0x0A, 5);

	//   lc.clearDisplay(0);
	clearDisplay();
	test01();
	while (1);
}

void loop() {}

/*------------------------------Defined Function---------------------------------*/

 // initialize the SPI protocol
void spiInt(int SPI_Pin_MOSI, int SPI_Pin_CS, int SPI_Pin_CLK) {
	pinMode(SPI_Pin_MOSI, OUTPUT);
	pinMode(SPI_Pin_CLK, OUTPUT);
	pinMode(SPI_Pin_CS, OUTPUT);
	digitalWrite(SPI_Pin_CS, HIGH);

	spiWrite(0X0F, 0);  // Display test

	spiWrite(0x0B, 0x07);  // scan limit

	spiWrite(0x09, 0);  // Decode mode

	clearDisplay();

	spiWrite(0x0C, 0);  // Shutdown true
}

void test01() {
	// Create an array with the data to shift out
	int addr = 0x00;
	byte opcode = 0x02;
	byte data = 0b11100011;

	// enable the line
	digitalWrite(SPI_Pin_CS, LOW);

	for (int i = 7; i >= 0; i--) {                           // 8 bits in a byte
		digitalWrite(SPI_Pin_MOSI, SPI_bitRead(opcode, i));  // Set MOSI
		digitalWrite(SPI_Pin_CLK, HIGH);                     // SCK high
		digitalWrite(SPI_Pin_CLK, LOW);                      // SCK low
	}

	for (int i = 7; i >= 0; i--) {                         // 8 bits in a byte
		digitalWrite(SPI_Pin_MOSI, SPI_bitRead(data, i));  // Set MOSI
		digitalWrite(SPI_Pin_CLK, HIGH);                   // SCK high
		digitalWrite(SPI_Pin_CLK, LOW);                    // SCK low
	}

	digitalWrite(SPI_Pin_CS, HIGH);
}

void spiWrite(byte opcode, byte data) {
	// enable the line
	digitalWrite(SPI_Pin_CS, LOW);

	for (int i = 7; i >= 0; i--) {  // 8 bits in a byte

		digitalWrite(SPI_Pin_MOSI, SPI_bitRead(opcode, i));  // Set MOSI
		digitalWrite(SPI_Pin_CLK, HIGH);                     // SCK high
		digitalWrite(SPI_Pin_CLK, LOW);                      // SCK low
	}
	for (int i = 7; i >= 0; i--) {  // 8 bits in a byte

		digitalWrite(SPI_Pin_MOSI, SPI_bitRead(data, i));  // Set MOSI
		digitalWrite(SPI_Pin_CLK, HIGH);                   // SCK high
		digitalWrite(SPI_Pin_CLK, LOW);                    // SCK low
	}

	digitalWrite(SPI_Pin_CS, HIGH);
}

void clearDisplay() {
	for (int i = 0; i < 8; i++) {
		spiWrite(i + 1, 0);
	}
}
