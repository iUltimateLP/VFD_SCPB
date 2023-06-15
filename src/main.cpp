/*
    Arduino Library to provide a HD44780-like interface for older Noritake
    VFD displays with the SCPB-Txx chipset (e.g. CU20025SCPB-T20A)

    Written by Jonathan Verbeek
*/

#include <Arduino.h>
#include <VFD_SCPB.h>

VFD_SCPB vfd(7);

// Can be generated with tools like this: https://omerk.github.io/lcdchargen/
// Keep in mind though that the VFD characters only have seven lines!
uint8_t heart[7] = {
	0b00000,
	0b01010,
	0b11111,
	0b11111,
	0b01110,
	0b00100,
	0b00000,
};

void setup() {
	// Set up the VFD config
	VFD_SCPB::Config config;
	config.cursorMode = VFD_SCPB::CursorMode::Invisible;

	// Initialize the display. Check your display's PCB and their jumpers to find the baud rate!
	vfd.begin(9600, config);

	// Set custom character (slot 0 will be char \xf0)
	vfd.setCustomCharacter(0, heart, false);
	
	// Write some text
	vfd.clear();
	vfd.print("Hello World!");
	vfd.cursorNewLine();
	vfd.print("I \xf0 VFDs!");
}

void loop() {
	vfd.cursorTo(10, 1);

	unsigned long time = millis();
	unsigned long seconds = (time / 1000) % 60;
	unsigned long milliseconds = time % 100;

	vfd.print(String(seconds).c_str());
	vfd.print(":");
	vfd.print(String(milliseconds).c_str());
}