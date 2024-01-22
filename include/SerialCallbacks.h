#ifndef PLATFORMIO_SERIALCALLBACKS_H
#define PLATFORMIO_SERIALCALLBACKS_H

#include <Arduino.h>
#include <vector>
#include "SerialCommunicator.h"
#include "WireOled.h"


namespace ledboard {
    struct StructTest {
        int someInt;
        float someFloat;
    };

    byte handleCommandA(WireOled &display, const std::vector <byte> &data, SerialProtocol::DataType dataType) {
        switch (dataType) {
            case SerialProtocol::DataType::StructTestType: {
                StructTest value;
                memcpy(&value, data.data(), data.size());

                display.write(0, 0, "f: ");
                display.write(0, 3, String(value.someFloat) + "    ");
                display.write(1, 0, "i: ");
                display.write(1, 3, String(value.someInt) + "    ");

                break;
            }
            default:
                Serial.println("Unknown data type");
        }

        return SerialProtocol::responseOk;
    }

}
#endif //PLATFORMIO_SERIALCALLBACKS_H
