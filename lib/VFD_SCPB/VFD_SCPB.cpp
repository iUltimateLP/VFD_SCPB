/*
	Arduino Library to provide a HD44780-like interface for older Noritake
	VFD displays with the SCPB-Txx chipset (e.g. CU20025SCPB-T20A)

	Written by Jonathan Verbeek
*/

#include "VFD_SCPB.h"

#define VFD_ESC 0x1B
#define VFD_CHAR_ADDRESS0 0xF0
#define VFD_CHAR_MAX 8

// Big credits to @falko17
// Takes in a 7byte encoding and stores it in outData, which is assumed to be a 5*sizeof(uint8_t) big buffer
int enc_7bit_to_5bit(const uint8_t* inData, uint8_t* outData, bool underline = false)
{
	outData[0] = (inData[1] & 0b111) << 5 | inData[0];
	outData[1] = (inData[3] & 1) << 7 | inData[2] << 2 | (inData[1] & 0b11000) >> 3;
	outData[2] = (inData[4] & 0b1111) << 4 | (inData[3] & 0b11110) >> 1;
	outData[3] = (inData[6] & 0b11) << 6 | inData[5] << 1 | (inData[4] & 0b10000) >> 4;
	outData[4] = (inData[6] & 0b11100) >> 2;

	if (underline)
		outData[4] |= 1 << 3;

	return 0;
}

VFD_SCPB::VFD_SCPB() {}

VFD_SCPB::VFD_SCPB(int txPin)
{
	this->txPin = txPin;
}

bool VFD_SCPB::begin(int baudRate, Config config)
{
	// Initialize serial interface, do not need a RX pin
	serial = new SoftwareSerial(-1, this->txPin);
	if (!serial)
		return false;

	// Begin
	serial->begin(baudRate);

	// Initialize the screen and apply the config
	reset();
	serial->write(config.characterTable == CharacterTable::International ? 0x18 : 0x19);
	setCursorMode(config.cursorMode);
	setBrightness(config.brightness);
	setBlinkSpeed(config.blinkSpeed);

	return true;
}

void VFD_SCPB::print(const char *text)
{
	// Simple text can just be printed via standard serial transmission
	serial->print(text);
}

void VFD_SCPB::reset()
{
	serial->write(VFD_ESC);
	serial->write(0x49); // Initialize - "Module is reset as just after power on."
}

void VFD_SCPB::clear()
{
	serial->write(0x0E); // CLR - "All displayed characters are cleared. The cursor doesn't move."
	serial->write(0x0C); // FF  - "The cursor moves to the top left end."
}

void VFD_SCPB::setCursorMode(CursorMode mode)
{
	// Figure out the right command to use
	uint8_t cmd;
	switch (mode)
	{
		case CursorMode::Invisible:
			cmd = 0x16;
			break;
		case CursorMode::StaticUnderline:
			cmd = 0x14;
			break;
		case CursorMode::BlinkingBlock:
			cmd = 0x15;
			break;
		case CursorMode::BlinkingUnderline:
			cmd = 0x17;
			break;
		default:
			break;
	}

	// Write
	serial->write(cmd);
}

void VFD_SCPB::setBrightness(uint8_t brightness)
{
	serial->write(VFD_ESC);
	serial->write(0x4C);
	serial->write(brightness);
}

void VFD_SCPB::setBlinkSpeed(uint8_t speed)
{
	serial->write(VFD_ESC);
	serial->write(0x54);
	serial->write(speed);
}

uint8_t VFD_SCPB::setCustomCharacter(uint8_t slot, const uint8_t *charData, bool underline)
{
	// Figure out where to store the address
	uint8_t charAddress = VFD_CHAR_ADDRESS0 + slot;

    // Encode to the 5-byte encoding the VFD display uses per datasheet
	uint8_t* buffer = static_cast<uint8_t*>(malloc(5 * sizeof(uint8_t)));
	enc_7bit_to_5bit(charData, buffer, underline);

	// Send to display
	serial->write(VFD_ESC);
	serial->write(0x43);
	serial->write(charAddress);
	for (int i = 0; i <= 4; i++)
	{
		serial->write(buffer[i]);
	}

	return charAddress;
}

void VFD_SCPB::cursorLeft()
{
	serial->write(0x08); // BS - "The cursor moves one character to the left."
}

void VFD_SCPB::cursorRight()
{
	serial->write(0x09); // HT - "The cursor moves one character to the right."
}

void VFD_SCPB::cursorDown()
{
	serial->write(0x0A); // LF - "The cursor moves to the same column on the lower line."
}

void VFD_SCPB::cursorReset()
{
	serial->write(0x0C); // FF - "The cursor moves to the top left end."
}

void VFD_SCPB::cursorLineStart()
{
	serial->write(0x0D); // CR - "The cursor moves to the left end on the same line"
}

void VFD_SCPB::cursorNewLine()
{
	cursorDown();
	cursorLineStart();
}

void VFD_SCPB::cursorTo(uint8_t x, uint8_t y)
{
	// Go to topleft first
	cursorReset();

	// Go to the location
	for (size_t _y = 0; _y < y; _y++)
	{
		cursorDown();
	}

	for (size_t _x = 0; _x < x; _x++)
	{
		cursorRight();
	}
}
