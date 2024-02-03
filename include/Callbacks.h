#ifndef PLATFORMIO_CALLBACKS_H
#define PLATFORMIO_CALLBACKS_H


#include <Arduino.h>

#include <ArtnetEtherENC.h>
#include <Adafruit_NeoPXL8.h>

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

    void illuminate(int *fpsCounter, Adafruit_NeoPXL8 *leds, const SerialProtocol::BoardSettings &settings, const uint8_t *data, uint16_t transmitterIndex) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        fpsCounter[transmitterIndex]++;

        for (int p = 0; p < settings.pixelPerUniverse; p++) {
            leds->setPixelColor(
                p + settings.pixelPerUniverse * transmitterIndex,
                Adafruit_NeoPXL8::Color(data[p * 3], data[p * 3 + 1], data[p * 3 + 2])
            );
        }
    }

    void receiveArtNet(int *fpsCounter, Adafruit_NeoPXL8 *leds, const SerialProtocol::BoardSettings &settings, const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote) {

        if (metadata.universe == settings.universeA) {
            illuminate(fpsCounter, leds, settings, data, 0);
        }
        else if (metadata.universe == settings.universeB) {
            illuminate(fpsCounter, leds, settings, data, 1);
        }
        else if (metadata.universe == settings.universeC) {
            illuminate(fpsCounter, leds, settings, data, 2);
        }
    }
}


#endif //PLATFORMIO_CALLBACKS_H
