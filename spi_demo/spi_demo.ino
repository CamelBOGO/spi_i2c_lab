#define SPI_bitWrite(value, bit) (((value) >> (7-bit)) & 0b1)

// Global Variables
uint8_t spiMosiPin = 11;  // MOSI
uint8_t spiCsPin = 10;    // CS
uint8_t spiClkPin = 13;   // clk
uint8_t spiMisoPin = 9;	  // MISO
int numOfDevices = 1;	  // Number of the max7219-matrix Dot devices

// Function Prototypes
void spiInit(uint8_t spiMosiPin, uint8_t spiCsPin, uint8_t spiClkPin);
void spiWriteByte(byte data);
byte spiWriteReadByte(byte data);

void displayInit();
void displayWrite(byte opcode, byte data);
int displayRead();
byte spiRead();
void displayClear();

void test01();

/*------------------------------Main---------------------------------*/

void setup() {
	Serial.begin(9600);

	spiInit(spiMosiPin, spiCsPin, spiClkPin);
	displayInit();

	test01();
}

void loop() {}

/*------------------------------Defined Function---------------------------------*/

 // Initialize the SPI protocol.
void spiInit(uint8_t spiMosiPin, uint8_t spiCsPin, uint8_t spiClkPin) {
	// Set MOSI, CLK, MISO and CS pin to output mode.
	pinMode(spiMosiPin, OUTPUT);
	pinMode(spiClkPin, OUTPUT);
	pinMode(spiCsPin, OUTPUT);
	pinMode(spiMisoPin, INPUT_PULLUP);
	// CS pin high for default.
	// Clk pin low for default.
	digitalWrite(spiCsPin, HIGH);
	digitalWrite(spiClkPin, LOW);

}

void displayInit() {
	// Initializations of all max7219-matrix Dot devices.

	displayWrite(0x09, 0);		// Set Decode-mode register to no decode for digit 0-7.
	displayWrite(0x0A, 5);		// Set Intensity register to adjust the brightness, maximum is 15.	
	displayWrite(0x0B, 0x07);		// Set scan-limit register to determine how many digits(0-7) are displayed.
	displayWrite(0x0C, 1);  		// Set Shutdown register to normal operation.
	displayWrite(0X0F, 0);		// Set Display-test register to normal operation.
	//loop for all devices to initialization.
	for (int i = 0; i < numOfDevices; i++) {
		displayWrite(0, 0);
	}

	displayClear();			// Initialize the led display.

}

void test01() {
	// Create an array with the data to shift out.
	// int addr = 0x00;
	byte opcode = 0x02;
	byte data = 0b11100011;

	displayWrite(opcode, data);
}

// SPI write a byte without enable/disable.
void spiWriteByte(byte data) {

	// Be careful MSB or LSB first.
	for (int i = 0; i < 8; i++) {
		// Set MOSI (Current bit).
		digitalWrite(spiMosiPin, SPI_bitWrite(data, i));
		// Clock Pulse
		digitalWrite(spiClkPin, HIGH);
		digitalWrite(spiClkPin, LOW);
	}

}

// SPI write 2 bytes to MAX7219.
void displayWrite(byte opcode, byte data) {
	// Enable the line.
	digitalWrite(spiCsPin, LOW);

	// Write 2 bytes.
	spiWriteByte(opcode);
	spiWriteByte(data);

	// Disable the line.
	digitalWrite(spiCsPin, HIGH);
}

// Read 2 bytes from MAX7219 by SPI.
int displayRead() {
	digitalWrite(spiCsPin, LOW);

	// Read 2 bytes.
	byte opcode = spiRead();
	byte data = spiRead();

	// Disable the line.
	digitalWrite(spiCsPin, HIGH);

	int read = (opcode << 8) | data;
	return read;
}

// Send one byte (0x00) by MISO and return a byte from MOSI.
byte spiRead() {
	byte read = 0;

	// Be careful MSB or LSB first.
	for (int i = 0; i < 8; i++) {
		// Set MOSI (no-operation).
		digitalWrite(spiMosiPin, SPI_bitWrite(0, i));

		// Read
		read |= digitalRead(spiMisoPin);
		if (i < 7) read <<= 1;

		// Clock Pulse
		digitalWrite(spiClkPin, HIGH);
		digitalWrite(spiClkPin, LOW);
	}

	return read;
}

// Set all LEDs off
void displayClear() {
	// Use MAX7219Write instead?
	for (int i = 0; i < 8; i++) {
		displayWrite(i + 1, 0);
	}
	for (int i = 0; i < numOfDevices; i++) {
		displayWrite(0, 0);
	}
}
