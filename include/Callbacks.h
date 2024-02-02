#ifndef PLATFORMIO_CALLBACKS_H
#define PLATFORMIO_CALLBACKS_H


#include <Arduino.h>

#include "SerialProtocol.h"
#include "SerialCommunicator.h"
#include "ILEDBoard.h"


namespace Frangitron {

    void sendSettings(void *vBoard, std::vector<byte> &data) {
        SerialProtocol::BoardSettings settings;
        settings.ipAddress[0] = 0x61;
        settings.ipAddress[1] = 0x62;
        settings.ipAddress[2] = 0x63;
        settings.ipAddress[3] = 0x64;

        data.resize(sizeof(SerialProtocol::BoardSettings)); // FIXME be more efficient
        memcpy(data.data(), &settings, sizeof(SerialProtocol::BoardSettings));

        auto board = static_cast<ILEDBoard*>(vBoard);
        board->displayWrite(0, 0, "Configuration >");
        board->displayWrite(1, 0, String(settings.ipAddress, 4) + "    ");
        board->displayWrite(1, 5, String(settings.name, 10));
    }

    void receiveSettings(void *vBoard, const std::vector<byte> &data) {
        SerialProtocol::BoardSettings settings;
        memcpy(&settings, data.data(), data.size());

        auto board = static_cast<ILEDBoard*>(vBoard);
        board->displayWrite(0, 0, "Configuration <");
        board->displayWrite(1, 0, String(settings.ipAddress, 4) + "    ");
        board->displayWrite(1, 5, String(settings.name, 10));
    }
}

#endif //PLATFORMIO_CALLBACKS_H