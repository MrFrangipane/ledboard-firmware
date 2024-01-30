#ifndef PLATFORMIO_CALLBACKS_H
#define PLATFORMIO_CALLBACKS_H


#include <Arduino.h>

#include "SerialProtocol.h"
#include "SerialCommunicator.h"
#include "ILEDBoard.h"


namespace Frangitron {

    void sendConfiguration(ILEDBoard *board, std::vector<byte> &data) {
        SerialProtocol::Configuration configuration;
        configuration.boardID[0] = 0x41;
        configuration.boardID[1] = 0x42;
        configuration.boardID[2] = 0x43;
        configuration.boardID[3] = 0x44;
        configuration.boardID[4] = 0x45;
        configuration.boardID[5] = 0x46;
        configuration.boardID[6] = 0x47;
        configuration.boardID[7] = 0x48;

        data.reserve(sizeof(SerialProtocol::Configuration));
        memcpy(data.data(), &configuration, sizeof(SerialProtocol::Configuration));

        board->displayWrite(0, 0, "Configuration >");
        board->displayWrite(1, 0, String(configuration.boardID, 8));
    }

    void receiveConfiguration(ILEDBoard *board, const std::vector<byte> &data) {
        SerialProtocol::Configuration configuration;
        memcpy(&configuration, data.data(), data.size());

        board->displayWrite(0, 0, "Configuration <");
        board->displayWrite(1, 0, String(configuration.boardID, 8));
    }
}


#endif //PLATFORMIO_CALLBACKS_H
