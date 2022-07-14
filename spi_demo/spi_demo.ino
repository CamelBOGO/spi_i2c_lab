#define SPI_bitWrite(value, bit) (((value) >> (7-bit)) & 0b1)

// Global Variables
uint8_t spiMosiPin = 11;  // MOSI
uint8_t spiCsPin = 10;    // CS
uint8_t spiClkPin = 13;   // clk
uint8_t spiMisoPin = 9;	  // MISO
int numOfDevices = 2;	  // Number of the max7219-matrix Dot devices

// Function Prototypes
void spiInit(uint8_t spiMosiPin, uint8_t spiCsPin, uint8_t spiClkPin);
void MAX7219Init();
void MAX7219Write(byte opcode, byte data);
void spiWriteByte(byte data);
int MAX7219Read();
byte spiRead();
void clearDisplay();
void test01();

/*------------------------------Main---------------------------------*/

void setup() {
	Serial.begin(9600);
	spiInit(spiMosiPin, spiCsPin, spiClkPin);
	MAX7219Init();

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
	pinMode(spiMisoPin,INPUT_PULLUP);
	// CS pin high for default.
	// How about other pins?
	digitalWrite(spiCsPin, HIGH);

}

void MAX7219Init(){
	// Initializations of all max7219-matrix Dot devices.

	MAX7219Write(0x09, 0);		// Set Decode-mode register to no decode for digit 0-7
	MAX7219Write(0x0A, 5);		// Set Intensity register to adjust the brightness, maximum is 15.	
	MAX7219Write(0x0B, 0x07);		// Set scan-limit register to determine how many digits(0-7) are displayed
	MAX7219Write(0x0C, 1);  		// Set Shutdown register to normal operation
	MAX7219Write(0X0F, 0);		// Set Display-test register to normal operation
	//loop for all devices to initialization.
	for(int i=0;i<numOfDevices;i++){
		MAX7219Write(0,0);
	}

	clearDisplay();			// Initialize display

}

void test01() {
	// Create an array with the data to shift out.
	// int addr = 0x00;
	byte opcode = 0x02;
	byte data = 0b11100011;

	MAX7219Write(opcode, data);
}

// SPI write a byte without enable/disable.
void spiWriteByte(byte data) {
	// byte read=0;
	// Be careful MSB or LSB first.
	for (int i = 0; i < 8; i++) {
		// Set MOSI (Current bit).
		digitalWrite(spiMosiPin, SPI_bitWrite(data, i));
		// Clock Pulse
		digitalWrite(spiClkPin, HIGH);
		digitalWrite(spiClkPin, LOW);
		
		// // Read
		// read |= digitalRead(spiMisoPin);
		// if (i < 7) read <<= 1;
	}
	// Serial.println(read,HEX);
}

// SPI write 2 bytes with enable/disable.
void MAX7219Write(byte opcode, byte data) {
	// Enable the line.
	digitalWrite(spiCsPin, LOW);

	// Write 2 bytes.
	spiWriteByte(opcode);
	spiWriteByte(data);

	// Disable the line.
	digitalWrite(spiCsPin, HIGH);
}

int MAX7219Read(){
	digitalWrite(spiCsPin, LOW);

	// Read 2 bytes.
	byte opcode = spiRead();
	byte data = spiRead();

	// Disable the line.
	digitalWrite(spiCsPin, HIGH);

	int read = (opcode << 8)|data;
	return read;
}

byte spiRead(){
	byte read = 0;

	// Be careful MSB or LSB first.
	for (int i = 0; i < 8; i++) {
		// Read
		read |= digitalRead(spiMisoPin);
		if (i < 7) read <<= 1;

		// Set MOSI (Current bit).
		digitalWrite(spiMosiPin, SPI_bitWrite(0, i));
		// Clock Pulse
		digitalWrite(spiClkPin, HIGH);
		digitalWrite(spiClkPin, LOW);

	}

	return read;
}

void clearDisplay() {
	// Use MAX7219Write instead?
	for (int i = 0; i < 8; i++) {
		MAX7219Write(i + 1, 0);
	}
	for (int i=0; i<numOfDevices;i++){
		MAX7219Write(0,0);
	}
}
