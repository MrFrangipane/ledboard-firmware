#ifndef PLATFORMIO_CALLBACKS_H
#define PLATFORMIO_CALLBACKS_H


#include <Arduino.h>

#include "SerialProtocol.h"
#include "ILEDBoard.h"


namespace Frangitron {

    void sendSettings(void *vBoard, std::vector<byte> &data) {
        auto board = static_cast<ILEDBoard*>(vBoard);

        data.resize(sizeof(SerialProtocol::BoardSettings)); // FIXME be more efficient
        memcpy(data.data(), board->getSettings(), sizeof(SerialProtocol::BoardSettings));

        board->displayWrite(0, 15, ">");
    }

    void receiveSettings(void *vBoard, const std::vector<byte> &data) {
        SerialProtocol::BoardSettings settings;
        memcpy(&settings, data.data(), data.size());

        auto board = static_cast<ILEDBoard*>(vBoard);

        if (settings.doSave == 1) {
            board->saveSettings();
            board->displayWrite(0, 0, "Conf < saved    ");
        } else {
            board->displayWrite(0, 0, "Conf < unsaved  ");
        }

        board->setSettings(&settings);

        board->displayWrite(0, 15, "<");
    }
}


void receiveArtNet(const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote) {
    if (metadata.universe == 1) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
}

#endif //PLATFORMIO_CALLBACKS_H
