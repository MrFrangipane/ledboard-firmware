#ifndef PLATFORMIO_LEDBOARD_H
#define PLATFORMIO_LEDBOARD_H


#include "SerialProtocol.h"
#include "SerialCommunicator.h"
#include "Callbacks.h"
#include "WireOled.h"
#include "ILEDBoard.h"


namespace Frangitron {

    class LEDBoard : public ILEDBoard {
    public:
        void init() {
            display.init();
            display.setContrast(0);
            display.write("Hello");
            display.write(1, 0, String(sizeof(SerialProtocol::BoardSettings)));
            display.write(1, 4, String(sizeof(char)));
            display.write(1, 8, String(sizeof(int)));

            serialCommunicator.setCallbackParent(this);
            serialCommunicator.registerSendCallback(
                SerialProtocol::DataTypeCode::BoardSettingsCode,
                sendSettings
            );
            serialCommunicator.registerReceiveCallback(
                SerialProtocol::DataTypeCode::BoardSettingsCode,
                receiveSettings
            );
        }

        void loop() {
            display.pollScreensaver();
            serialCommunicator.poll();
        }

        void displayWrite(uint8_t row, uint8_t column, String text) override {
            display.write(row, column, text);
        }

    private:
        SerialCommunicator serialCommunicator;
        WireOled display;
    };
}

#endif //PLATFORMIO_LEDBOARD_H
