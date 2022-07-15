/*
	Name:       i2c_demo.ino
	Created:	2/6/2022
	Author:     DESKTOP-AORUS03\BOGO
*/

struct i2c {
	uint8_t dataPin;	// sda
	uint8_t clockPin;	// sca
};

// Global Variables
i2c i2c1 = { A4, A5 };
int8_t delayTime = 1;

// Function Prototypes
void i2cHigh(uint8_t pin);
void i2cLow(uint8_t pin);
void i2cClockPulse(uint8_t clockPin);
void i2cStart(i2c i2c);
void i2cStop(i2c i2c);
byte i2cReadBit(i2c i2c);
byte i2cReadByte(i2c i2c);
void i2cWriteBit(bool bit, i2c i2c);
void i2cWriteByte(byte data, i2c i2c);

/*------------------------------Main---------------------------------*/

// Run once when MCU turned on.
void setup() {
	Serial.begin(9600);
}

// Keep repeat after setup().
void loop() {
	// Scan the address from 1 to 127. Send the address in write mode,
	// if there is respond, the device of this address existed.
	for (byte address = 1; address <= 127; address++) {
		// The first cmd is address shifted one to left, a zero is add at RHS.
		byte cmd = address << 1;

		// General I2C Command: Start Bit -> Write Byte -> Read ACK -> Stop Bit.
		i2cStart(i2c1);
		i2cWriteByte(cmd, i2c1);
		byte ack = i2cReadBit(i2c1);
		i2cStop(i2c1);

		// Check if there is respond. If ACK is pulled down, respond existed.
		if (ack == 0) {
			Serial.print("I2C device found at address 0x");
			if (address < 16) Serial.print("0");
			Serial.print(address, HEX);
			Serial.println(".");
		}
	}

	Serial.println("\n");

	delay(1000);
}

/*------------------------------Defined Function---------------------------------*/

// Set this pin to high, then delay for the time.
// At the same time, it can accept pull down input from slave (ACK).
void i2cHigh(uint8_t pin) {
	pinMode(pin, INPUT_PULLUP);
	delayMicroseconds(delayTime);
}

// Set this pin to low, then delay for the time.
void i2cLow(uint8_t pin) {
	digitalWrite(pin, LOW);
	pinMode(pin, OUTPUT);
	delayMicroseconds(delayTime);
}

// Send a clock pulse by this pin. Pull Up -> Hold a Time -> Pull Down.
void i2cClockPulse(uint8_t clockPin) {
	i2cHigh(clockPin);
	delayMicroseconds(delayTime);
	i2cLow(clockPin);
}

// Send a start bit by this I2C pair, including the initial bit.
// All High -> Hold -> Data Low -> Hold -> Clock Low -> Hold
void i2cStart(i2c i2c) {
	// Initial Bit
	i2cHigh(i2c.dataPin);
	i2cHigh(i2c.clockPin);
	delayMicroseconds(delayTime);

	// Start Bit
	i2cLow(i2c.dataPin);
	delayMicroseconds(delayTime);
	i2cLow(i2c.clockPin);
	delayMicroseconds(delayTime);
}

// Send a stop bit by this I2C pair.
// All Low -> Hold -> Clock High -> Hold -> Data High -> Hold
void i2cStop(i2c i2c) {
	// Make sure 2 pins are low.
	i2cLow(i2c.clockPin); // Really need this?
	i2cLow(i2c.dataPin);
	delayMicroseconds(delayTime);

	// Stop Bit
	i2cHigh(i2c.clockPin);
	delayMicroseconds(delayTime);
	i2cHigh(i2c.dataPin);
	delayMicroseconds(delayTime);
}

// Read a voltage level (bit) from this data pin, return it by a byte.
byte i2cReadBit(i2c i2c) {
	// Set the pin to high and input mode.
	i2cHigh(i2c.dataPin);

	// Send a clock pulse, read the bit when the clock is high.
	i2cHigh(i2c.clockPin);
	byte bit = digitalRead(i2c.dataPin);
	i2cLow(i2c.clockPin);

	return bit;
}

// TO-DO: Simplify the data read.
byte i2cReadByte(i2c i2c) {
	byte data = 0;

	i2cHigh(i2c.dataPin);
	for (byte i = 0; i < 8; i++) {
		// Same as bitWrite(data, 7 - i, i2cReadBit());
		// Can it be simpler?
		// data = data & ~(0b1 << (7 - i)) | i2cReadBit(dataPin, clockPin) << (7 - i);
		data |= i2cReadBit(i2c);
		if (i < 7) data <<= 1;
	}

	return data;
}

// Write a voltage level (bit) to this data pin.
// Need input a bit and a I2C pair.
void i2cWriteBit(bool bit, i2c i2c) {
	// Call set high or low function for data pin depending on the input bit.
	if (bit) i2cHigh(i2c.dataPin);
	else i2cLow(i2c.dataPin);

	// Send a clock pulse to trigger the writing.
	i2cClockPulse(i2c.clockPin);
}

// Input a byte to this I2C pair.
void i2cWriteByte(byte data, i2c i2c) {
	// Read the bit from data and write to data pin bit by bit, starting from MSB.
	for (int i = 0; i < 8; i++) {
		// Same as bitRead(data, 7 - i);
		i2cWriteBit(data >> (7 - i) & 0b1, i2c);
	}
}