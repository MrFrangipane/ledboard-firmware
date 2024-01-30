#ifndef ARDUINO_I2C_US2066_OLED_WIREOLED_H
#define ARDUINO_I2C_US2066_OLED_WIREOLED_H

// FIXME update and use the library https://github.com/MrFrangipane/Arduino_I2C_US2066_OLED

#include <Arduino.h>
#include <Wire.h>


#define DEFAULT_ADDRESS 0x3c
#define DEFAULT_PIN_RESET 3
#define MODE_COMMAND 0x80
#define MODE_DATA 0x40
#define ROW_MODE_2ROWS 0x08

#define STATE_ON 0x04
#define STATE_CURSOR_BLINKING 0x01


struct WireOledConfig {
    short pinReset { DEFAULT_PIN_RESET };
    uint8_t rowMode { ROW_MODE_2ROWS };
    uint8_t addr { DEFAULT_ADDRESS };
    uint16_t screensaverTimeout { 30000 };  // 30s
};


class WireOled {
public:
    WireOled() {}
    ~WireOled() {}

    bool detect(const WireOledConfig config) {
        mConfig = config;
        return detect();
    }

    bool detect() {
        bool result = false;

        Wire.begin();
        pinMode(mConfig.pinReset, OUTPUT);
        digitalWrite(mConfig.pinReset, HIGH);

        Wire.beginTransmission(mConfig.addr);
        if (Wire.endTransmission() == 0) {
            result = true;
        }

        Wire.end();
        return result;
    }

    void init(const WireOledConfig config) {
        mConfig = config;
        init();
    }

    void init() {
        pinMode(mConfig.pinReset, OUTPUT);
        digitalWrite(mConfig.pinReset, HIGH);

        delay(100);
        Wire.begin();

        command(0x2A);  // function set (extended command set)
        command(0x71);  // function selection A, disable internal Vdd regualtor
        data(0x00);

        command(0x28);  // function set (fundamental command set)
        command(0x08);  // display off, cursor off, blink off

        command(0x2A);  // function set (extended command set)
        command(0x79);  // OLED command set enabled
        command(0xD5);  // set display clock divide ratio/oscillator frequency
        command(0x70);  // set display clock divide ratio/oscillator frequency
        command(0x78);  // OLED command set disabled

        command(mConfig.rowMode);// extended function set (4/2-lines)
        command(0x06);  // COM SEG direction

        command(0x72);  // function selection B, disable internal Vdd regualtor
        data(0x00);         // ROM CGRAM selection

        command(0x2A);  // function set (extended command set)
        command(0x79);  // OLED command set enabled
        command(0xDA);  // set SEG pins hardware configuration

        // TODO: check other doc for other screens
        command(0x00);  // set SEG pins ... NOTE: When using NHD-0216AW-XB3 or NHD_0216MW_XB3 change to (0x00)

        command(0xDC);  // function selection C
        command(0x00);  // function selection C
        command(0x81);  // set contrast control
        command(0x7F);  // set contrast control
        command(0xD9);  // set phase length
        command(0xF1);  // set phase length
        command(0xDB);  // set VCOMH deselect level
        command(0x40);  // set VCOMH deselect level
        command(0x78);  // OLED command set disabled

        command(0x28);  // function set (fundamental command set)
        command(0x01);  // clear display
        command(0x80);  // set DDRAM address to 0x00
        command(0x0C);  // display ON

        delay(100);
    }

    void setActive(bool isActive) {
        if (isActive) {
            mState |= STATE_ON;
        } else {
            mState &= ~STATE_ON;
        }
        applyState();
    }

    void setContrast(uint8_t contrast) {
        command(0x2A);
        command(0x79);      // OLED command set
        command(0x81);      // contrast
        command(contrast);
        command(0x78);      // exit command set
        command(0x28);
    }

    void setCursorBlinking(bool isBlinking) {
        if(isBlinking) {
            mState |= STATE_CURSOR_BLINKING;
        } else {
            mState &= ~STATE_CURSOR_BLINKING;
        }
        applyState();
    }

    void clear() {
        command(0x01);
    }

    void home() {
        move(0,0);
    }

    void move(uint8_t row, uint8_t col) {
        int row_offset[] = { 0x00, 0x40 }; // TODO: 4 rows ?
        command(0x80 | (col + row_offset[row]));
    }

    void write(char c) {
        data(c);
    }
    void write(const String &s) {
        int i = 0;
        while (s[i]) {
            data(s[i]);
            i++;
        }
    }

    void write(uint8_t row, uint8_t col, const String &s) {
        move(row, col);
        write(s);
    }

    void pollScreensaver() {
        if ( millis() - mlastActivity > mConfig.screensaverTimeout ) {
            setActive(false);
        }
    }

    void resetScreensaver() {
        if ((mState & STATE_ON) == 0) {
            setActive(true);
        }
        mlastActivity = millis();
    }

private:
    WireOledConfig mConfig;
    uint8_t mState = STATE_ON;
    unsigned long mlastActivity = 0;

    void command(unsigned char command) {
        resetScreensaver();
        Wire.beginTransmission(mConfig.addr);
        Wire.write(MODE_COMMAND);
        Wire.write(command);
        Wire.endTransmission();
    }

    void data(unsigned char data) {
        resetScreensaver();
        Wire.beginTransmission(mConfig.addr);
        Wire.write(MODE_DATA);
        Wire.write(data);
        Wire.endTransmission();
    }

    void applyState() {  // excluded from screensaver FIXME
        Wire.beginTransmission(mConfig.addr);
        Wire.write(MODE_COMMAND);
        Wire.write(0x08 | mState);
        Wire.endTransmission();
    }
};
#endif //ARDUINO_I2C_US2066_OLED_WIREOLED_H
