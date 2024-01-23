#ifndef PLATFORMIO_SERIALCALLBACKS_H
#define PLATFORMIO_SERIALCALLBACKS_H

#include <Arduino.h>
#include <vector>
#include "SerialCommunicator.h"
#include "WireOled.h"


namespace ledboard {


void getBoardInfo(SerialCommunicator &serialCommunicator, WireOled &display, const std::vector <byte> &data) {
    SerialProtocol::StructTest value;
    memcpy(&value, data.data(), data.size());

    display.write(0, 0, "f: ");
    display.write(0, 3, String(value.someFloat) + "    ");
    display.write(1, 0, "i: ");
    display.write(1, 3, String(value.someInt) + "    ");

    SerialProtocol::BoardInfo boardInfo;
    boardInfo.boardVersion = value.someInt;
    serialCommunicator.sendResponse(
        SerialProtocol::MessageType::responseBoardInfo,
        reinterpret_cast<byte*>(&boardInfo)
    );
}

}
#endif //PLATFORMIO_SERIALCALLBACKS_H
