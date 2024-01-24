#ifndef PLATFORMIO_SERIALCALLBACKS_H
#define PLATFORMIO_SERIALCALLBACKS_H

#include <Arduino.h>
#include <vector>
#include "SerialCommunicator.h"
#include "WireOled.h"
#include "Adafruit_NeoPXL8.h"


namespace ledboard {


void illuminate(SerialCommunicator &serialCommunicator, WireOled &display, Adafruit_NeoPXL8 &leds, const std::vector <byte> &data) {
    SerialProtocol::IlluminatedLed illuminated;
    memcpy(&illuminated, data.data(), data.size());

    leds.fill(0x00000000); // black
    if (illuminated.ledIndex > 0) {
        leds.setPixelColor(illuminated.ledIndex, leds.Color(illuminated.r, illuminated.g, illuminated.b, illuminated.w));
    }
    leds.show();

    display.write(1, 0, "Illum. led ");
    display.write(1, 11, String(illuminated.ledIndex) + "    ");
}

}
#endif //PLATFORMIO_SERIALCALLBACKS_H
