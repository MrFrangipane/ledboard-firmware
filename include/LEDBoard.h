#ifndef PLATFORMIO_LEDBOARD_H
#define PLATFORMIO_LEDBOARD_H

#include "SerialProtocol.h"
#include "SerialCommunicator.h"
#include "Callbacks.h"
#include "WireOled.h"
#include "ILEDBoard.h"


namespace Frangitron {

    class LEDBoard: public ILEDBoard {
    public:
        void init() {
            display.init();
            display.setContrast(0);
            display.write("Hello");

            serialCommunicator.init(this);
            serialCommunicator.registerSendCallback(SerialProtocol::DataTypeCode::Configuration, sendConfiguration);
            serialCommunicator.registerReceiveCallback(SerialProtocol::DataTypeCode::Configuration, receiveConfiguration);
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
