#define SPI_bitRead(value, bit) (((value) >> (bit)) & 0x01)

// Global Variables
uint8_t spiMosiPin = 11;  // MOSI
uint8_t spiCsPin = 10;    // CS
uint8_t spiClkPin = 13;   // clk

// Function Prototypes
void spiInit(uint8_t spiMosiPin, uint8_t spiCsPin, uint8_t spiClkPin);
void spiWrite(byte opcode, byte data);
void clearDisplay();
void test01();

/*------------------------------Main---------------------------------*/

void setup() {
	spiInit(spiMosiPin, spiCsPin, spiClkPin);

	spiWrite(0x0c, 1); // lc.shutdown(0, false);	
	// Adjust the brightness, maximum is 15.
	spiWrite(0x0A, 5); // lc.setIntensity(0, 5);	
	clearDisplay(); // lc.clearDisplay(0);

	test01();
}

void loop() {}

/*------------------------------Defined Function---------------------------------*/

 // Initialize the SPI protocol.
void spiInit(uint8_t spiMosiPin, uint8_t spiCsPin, uint8_t spiClkPin) {
	// Set MOSI, CLK and CS pin to output mode.
	pinMode(spiMosiPin, OUTPUT);
	pinMode(spiClkPin, OUTPUT);
	pinMode(spiCsPin, OUTPUT);
	// CS pin high for default.
	// How about other pins?
	digitalWrite(spiCsPin, HIGH);

	// Some display initializations.
	// This part should not be inside SPI init?
	spiWrite(0X0F, 0);  // Display test
	spiWrite(0x0B, 0x07);  // Scan limit
	spiWrite(0x09, 0);  // Decode mode

	clearDisplay();

	spiWrite(0x0C, 0);  // Shutdown true
}

void test01() {
	// Create an array with the data to shift out.
	// int addr = 0x00;
	byte opcode = 0x02;
	byte data = 0b11100011;

	spiWrite(opcode, data);
}

// SPI write a byte without enable/disable.
void spiWriteByte(byte data) {
	// Be careful MSB or LSB first.
	for (int i = 7; i >= 0; i--) {
		// Set MOSI (Current bit).
		digitalWrite(spiMosiPin, SPI_bitRead(data, i));
		// Clock Pulse
		digitalWrite(spiClkPin, HIGH);
		digitalWrite(spiClkPin, LOW);
	}
}

// SPI write 2 bytes with enable/disable.
void spiWrite(byte opcode, byte data) {
	// Enable the line.
	digitalWrite(spiCsPin, LOW);

	// Write 2 bytes.
	spiWriteByte(opcode);
	spiWriteByte(data);

	// Disable the line.
	digitalWrite(spiCsPin, HIGH);
}

void clearDisplay() {
	// Use spiWrite instead?
	for (int i = 0; i < 8; i++) {
		spiWrite(i + 1, 0);
	}
}
