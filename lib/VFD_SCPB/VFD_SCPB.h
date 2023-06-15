/*
    Arduino Library to provide a HD44780-like interface for older Noritake
    VFD displays with the SCPB-Txx chipset (e.g. CU20025SCPB-T20A)

    Written by Jonathan Verbeek
*/

#include <SoftwareSerial.h>

class VFD_SCPB
{
public:
    // Cursor mode enumeration
    enum CursorMode
    {
        Invisible,
        StaticUnderline,
        BlinkingUnderline,
        BlinkingBlock
    };

    // Character table enumeration
    enum CharacterTable
    {
        International,
        Katakana
    };

    // Configuration that can be applied on the display
    struct Config
    {
        // Sets the way the cursor is displayed
        CursorMode cursorMode = CursorMode::BlinkingUnderline;

        // Which character table the screen uses
        CharacterTable characterTable = CharacterTable::International;

        // Sets the screen's brightness (0 = very dark, 255 = very bright) (default = 255)
        uint8_t brightness = 255;

        // Sets the screen's blink speed (higher value = slower) (default = 20)
        uint8_t blinkSpeed = 20;

        // Empty default constructor
        Config() { }
    };

private:
    // Hide the default constructor
    VFD_SCPB();

public:
    // Constructor taking in the TX pin for the display's serial interface
    VFD_SCPB(int txPin);

    // Begins serial communication with the given baud rate
    bool begin(int baudRate, Config config = Config());

    // Prints text to the screen
    void print(const char* text);

    // Resets the whole screen to the original state after power-on
    void reset();

    // Clears the screen
    void clear();

    // Sets the way the cursor is displayed
    void setCursorMode(CursorMode mode);

    // Sets the screen's brightness (0 = very dark, 255 = very bright) (default = 255)
    void setBrightness(uint8_t brightness);

    // Sets the screen's blink speed (higher value = slower) (default = 20)
    void setBlinkSpeed(uint8_t speed);

    // Adds a new custom character to the display's ROM. The slot can go from 0 to 7
    // the chars can be used in text using \xf0 (slot 0) to \xf7 (slot 7)
    uint8_t setCustomCharacter(uint8_t slot, const uint8_t* charData, bool underline = false);

public:
    // Cursor functions
    void cursorLeft();
    void cursorRight();
    void cursorDown();
    void cursorReset();
    void cursorLineStart();
    void cursorNewLine();
    void cursorTo(uint8_t x, uint8_t y);

private:
    // The TX pin
    int txPin = 0;

    // Pointer to the internal serial interface communicating with the display
    SoftwareSerial* serial = nullptr;
};
