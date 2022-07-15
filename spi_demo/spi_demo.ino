/*
 * SPI for Daisy Chain
 */

struct spi {
	uint8_t csPin;		 // CS
	uint8_t clkPin;		  // CLK
	uint8_t mosiPin;	 // MOSI
	uint8_t misoPin;	  // MISO
};

// Global Variables
spi spi1 = { 9,10,11,12 };
uint8_t numOfDisp = 1;	  // Number of the max7219-matrix Dot devices

// Function Prototypes
// SPI Related
void spiInit(spi spi);
void spiWriteByte(spi spi, byte data);
void spiClockPulse(spi spi);
byte spiReadWriteByte(spi spi, byte data);

// Display Related
void displayWrite(spi spi, byte opcode, byte data);
uint16_t displayReadWrite(spi spi, byte opcode, byte data);
void displayClear(spi spi, uint8_t numOfDisp);
void displayInit(spi spi, uint8_t numOfDisp);

/*------------------------------Main---------------------------------*/

void setup() {
	spiInit(spi1);
	displayInit(spi1, numOfDisp);
}

void loop() {
	// A simple program which includes the demo of SPI R/W
	// that it shows a dot going from (0, 0) to (7, 7) then go back to (0, 0) and loop again.
	static byte opcode = 0x01;
	static byte data = 0x80;

	// Send the cmd.
	displayWrite(spi1, opcode, data);

	// Get current respond for single display by sending 0;
	uint16_t res = displayReadWrite(spi1, 0, 0);
	data = res;
	res >>= 8;
	opcode = res;

	delay(100);

	if (data != 0x01) {
		// Case: The dot is at row middle.
		data >>= 1;
	} else if (data == 0x01 && opcode >= 0x08) {
		// Case: The dot is at the row and column end.
		displayWrite(spi1, opcode, 0);
		opcode = 0x01;
		data = 0x80;
	} else {
		// Case: The dot is at the row end.
		displayWrite(spi1, opcode, 0);
		opcode++;
		data = 0x80;
	}
}

/*------------------------------SPI Related Function---------------------------------*/

 // Initialize the SPI protocol.
void spiInit(spi spi) {
	// Set pins mode.
	pinMode(spi.csPin, OUTPUT);
	pinMode(spi.clkPin, OUTPUT);
	pinMode(spi.mosiPin, OUTPUT);
	pinMode(spi.misoPin, INPUT_PULLUP);

	// Set pins default value.
	digitalWrite(spi.csPin, HIGH);
	digitalWrite(spi.clkPin, LOW);
	digitalWrite(spi.mosiPin, LOW);
}

// SPI send out a clock pulse on this port.
void spiClockPulse(spi spi) {
	digitalWrite(spi.clkPin, HIGH);
	digitalWrite(spi.clkPin, LOW);
}

// SPI write a byte without enable/disable and read.
void spiWriteByte(spi spi, byte data) {
	// Be careful MSB or LSB first.
	for (int i = 0; i <= 7; i++) {
		// Set MOSI (Current bit).
		digitalWrite(spi.mosiPin, (data >> (7 - i)) & 0b1);

		// Clock Pulse
		spiClockPulse(spi);
	}
}

// SPI read and write a byte at the same time without enable/disable.
byte spiReadWriteByte(spi spi, byte data) {
	byte readData = 0;

	// Be careful MSB or LSB first.
	for (int i = 0; i <= 7; i++) {
		// Read
		readData |= digitalRead(spi.misoPin);
		if (i < 7) readData <<= 1;

		// Write: Set MOSI (Current bit).
		digitalWrite(spi.mosiPin, (data >> (7 - i)) & 0b1);

		// Clock Pulse
		spiClockPulse(spi);
	}

	return readData;
}

/*------------------------------Display Related Function---------------------------------*/

// SPI write a 2 bytes cmd to MAX7219 without read
void displayWrite(spi spi, byte opcode, byte data) {
	// Enable the line.
	digitalWrite(spi.csPin, LOW);

	// Write 2 bytes. MSB first.
	spiWriteByte(spi, opcode);
	spiWriteByte(spi, data);

	// Disable the line.
	digitalWrite(spi.csPin, HIGH);
}

// SPI read and write a 2 bytes cmd from MAX7219
uint16_t displayReadWrite(spi spi, byte opcode, byte data) {
	uint16_t readData = 0;

	// Enable the line.
	digitalWrite(spi.csPin, LOW);

	// Read and Write 2 bytes. MSB first.
	readData = spiReadWriteByte(spi, opcode) << 8;
	readData |= spiReadWriteByte(spi, data);

	// Disable the line.
	digitalWrite(spi.csPin, HIGH);

	return readData;
}

// Set all LEDs off
void displayClear(spi spi, uint8_t numOfDisp) {
	// Write all LED to 0.
	for (int i = 0; i < 8; i++) {
		displayWrite(spi, i + 1, 0);
	}

	// Make sure the cmd above are shift through all displays.
	for (int i = 0; i < numOfDisp; i++) {
		displayWrite(spi, 0, 0);
	}
}

void displayInit(spi spi, uint8_t numOfDisp) {
	// Initializations of all max7219-matrix Dot devices.
	displayWrite(spi, 0x09, 0);		// Set Decode-mode register to no decode for digit 0-7.
	displayWrite(spi, 0x0A, 5);		// Set Intensity register to adjust the brightness, maximum is 15.	
	displayWrite(spi, 0x0B, 0x07);		// Set scan-limit register to determine how many digits(0-7) are displayed.
	displayWrite(spi, 0x0C, 1);  		// Set Shutdown register to normal operation.
	displayWrite(spi, 0X0F, 0);		// Set Display-test register to normal operation.

	// Loop for all devices to initialization. Make sure the cmd above are shift through all displays.
	for (int i = 0; i < numOfDisp; i++) {
		displayWrite(spi, 0, 0);
	}

	displayClear(spi, numOfDisp);			// Initialize the led display.
}
