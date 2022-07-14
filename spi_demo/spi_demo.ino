#define SPI_bitRead(value, bit) (((value) >> (bit)) & 0x01)

// Global Variables
uint8_t spiMosiPin = 11;  // MOSI
uint8_t spiCsPin = 10;    // CS
uint8_t spiClkPin = 13;   // clk
uint8_t spiMisoPin = 9;	  // MISO
 int numOfDevices = 2;	  // Number of the max7219-matrix Dot devices

// Function Prototypes
void spiInit(uint8_t spiMosiPin, uint8_t spiCsPin, uint8_t spiClkPin);
void MAX7219Init();
void spiWrite(byte opcode, byte data);
void spiWriteByte(byte data);
void clearDisplay();
void test01();

/*------------------------------Main---------------------------------*/

void setup() {
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
	// Some MAX7219 initializations.
	for(int i=0;i<numOfDevices;i++){
	spiWrite(i,0x09, 0);		// Set Decode-mode register to no decode for digit 0-7
	spiWrite(i,0x0A, 5);		// Set Intensity register to adjust the brightness, maximum is 15.	
	spiWrite(i,0x0B, 0x07);	// Set scan-limit register to determine how many digits(0-7) are displayed
	spiWrite(i,0x0C, 1);  	// Set Shutdown register to normal operation
	spiWrite(i,0X0F, 0);		// Set Display-test register to normal operation
	}
	clearDisplay();			// Initialize display

}

void test01() {
	// Create an array with the data to shift out.
	// int addr = 0x00;
	byte opcode = 0x02;
	byte data = 0b11100011;

	spiWrite(0,opcode, data);
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
void spiWrite(int addr,byte opcode, byte data) {
	// Enable the line.
	digitalWrite(spiCsPin, LOW);

	// Write 2 bytes.
	spiWriteByte(opcode);
	spiWriteByte(data);

	while(addr){
		spiWriteByte(0);
		spiWriteByte(0);
		addr--;
	}

	// Disable the line.
	digitalWrite(spiCsPin, HIGH);
}

void clearDisplay() {
	// Use spiWrite instead?
	for(int j=0;j<numOfDevices;j++){
		for (int i = 0; i < 8; i++) {
			spiWrite(j,i + 1, 0);
		}
	}
}
