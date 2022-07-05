/*
	Name:       i2c_demo.ino
	Created:	2/6/2022
	Author:     DESKTOP-AORUS03\BOGO
*/

uint8_t dataPin = A4; // sda
uint8_t clockPin = A5; // sca
int8_t delayTime = 1; //us

void i2cHigh(uint8_t pin);
void i2cLow(uint8_t pin);
void i2cClockPulse(uint8_t clockPin);
void i2cStart(uint8_t dataPin, uint8_t clockPin);
void i2cStop(uint8_t dataPin, uint8_t clockPin);
byte i2cReadBit(uint8_t dataPin, uint8_t clockPin);
byte i2cReadByte(uint8_t dataPin, uint8_t clockPin);
void i2cWriteBit(bool bit, uint8_t dataPin, uint8_t clockPin);
void i2cWriteByte(byte data, uint8_t dataPin, uint8_t clockPin);

void setup() {
	Serial.begin(9600);
}

void loop() {
	for (byte address = 1; address <= 127; address++) {
		byte cmd = address << 1;

		i2cStart(dataPin, clockPin);
		i2cWriteByte(cmd, dataPin, clockPin);
		byte ack = i2cReadBit(dataPin, clockPin);
		i2cStop(dataPin, clockPin);

		if (ack == 0) {
			Serial.print("I2C device found at address 0x");
			if (address < 16) Serial.print("0");
			Serial.print(address, HEX);
			Serial.println(".");
		}
	}

	Serial.println("\n");

	delay(5000);
}

void i2cHigh(uint8_t pin) {
	pinMode(pin, INPUT_PULLUP);
	delayMicroseconds(delayTime);
}

void i2cLow(uint8_t pin) {
	digitalWrite(pin, LOW);
	pinMode(pin, OUTPUT);
	
	delayMicroseconds(delayTime);
}

void i2cClockPulse(uint8_t clockPin) {
	i2cHigh(clockPin);
	delayMicroseconds(delayTime);
	i2cLow(clockPin);
}

void i2cStart(uint8_t dataPin, uint8_t clockPin) {
	i2cHigh(dataPin);
	i2cHigh(clockPin);
	delayMicroseconds(delayTime);

	i2cLow(dataPin);
	delayMicroseconds(delayTime);
	i2cLow(clockPin);
	delayMicroseconds(delayTime);
}

void i2cStop(uint8_t dataPin, uint8_t clockPin) {
	i2cLow(clockPin); // Really need this?
	i2cLow(dataPin);
	delayMicroseconds(delayTime);

	i2cHigh(clockPin);
	delayMicroseconds(delayTime);
	i2cHigh(dataPin);
	delayMicroseconds(delayTime);
}

byte i2cReadBit(uint8_t dataPin, uint8_t clockPin) {
	i2cHigh(dataPin);
	i2cHigh(clockPin);
	byte bit = digitalRead(dataPin);
	i2cLow(clockPin);
	return bit;
}

byte i2cReadByte(uint8_t dataPin, uint8_t clockPin) {
	byte data = 0;

	i2cHigh(dataPin);
	for (byte i = 0; i < 8; i++) {
		// Same as bitWrite(data, 7 - i, i2cReadBit());
		data = data & ~(0b1 << (7 - i)) | i2cReadBit(dataPin, clockPin) << (7 - i);
	}

	return data;
}

void i2cWriteBit(bool bit, uint8_t dataPin, uint8_t clockPin) {
	if (bit) i2cHigh(dataPin);
	else i2cLow(dataPin);

	i2cClockPulse(clockPin);
}

void i2cWriteByte(byte data, uint8_t dataPin, uint8_t clockPin) {
	for (int i = 0; i < 8; i++) {
		// Same as bitRead(data, 7 - i);
		i2cWriteBit(data >> (7 - i) & 0b1, dataPin, clockPin);
	}
}