#ifndef PLATFORMIO_SERIALCOMMUNICATOR_H
#define PLATFORMIO_SERIALCOMMUNICATOR_H

#include <Arduino.h>
#include <functional>
#include <vector>
#include <map>
#include "SerialProtocol.h"
#include "WireOled.h"


#define COMMAND_BUFFER_SIZE


namespace ledboard {

enum class ReceivingStatus : byte {
    Idle, ReceivingHeader, ReceivingData
};

class SerialCommunicator {
public:
    using Callback = std::function<void(SerialCommunicator &, WireOled &, const std::vector<byte> &)>;

    void registerCallback(SerialProtocol::MessageType messageType, Callback callback) {
        callbacks[messageType] = callback;
    }

    void init() {
        display.init();
        display.setContrast(0);
        display.write("Hello");
    }

    void poll() {
        display.pollScreensaver();

        if (!Serial.dtr()) {
            return;
        }

        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

        while (Serial.available() > 0) {
            byte incomingByte = Serial.read();

            if (receivingStatus == ReceivingStatus::Idle) {
                if (incomingByte == SerialProtocol::flagBegin) {
                    receivingStatus = ReceivingStatus::ReceivingHeader;
                    return; // discard incomingByte as it is a flag
                }
            }
            if (receivingStatus == ReceivingStatus::ReceivingHeader) {
                if (headerBufferIndex < SerialProtocol::headerSize) {
                    headerBuffer[headerBufferIndex] = incomingByte;
                    headerBufferIndex++;
                }
                else {
                    mMessageType = static_cast<SerialProtocol::MessageType>(headerBuffer[0]);
                    inDataSize = (headerBuffer[4] << 24) | (headerBuffer[3] << 16) | (headerBuffer[2] << 8) | headerBuffer[1];
                    receivingStatus = ReceivingStatus::ReceivingData;
                }
            }

            if (receivingStatus == ReceivingStatus::ReceivingData) {
                if (dataBuffer.size() < inDataSize) {
                    dataBuffer.push_back(incomingByte);
                }
                else if (dataBuffer.size() == inDataSize && incomingByte == SerialProtocol::flagEnd) {
                    if (callbacks.find(mMessageType) != callbacks.end()) {
                        callbacks[mMessageType](*this, display, dataBuffer);
                    }
                    resetToIdle();
                }
                else {
                    resetToIdle();
                }
            }
        }
    }

    void sendResponse(SerialProtocol::MessageType messageType, byte* data) {
        uint16_t dataSize;
        // FIXME use a pre calculated map ?
        switch (messageType) {
            case SerialProtocol::MessageType::responseBoardInfo:
                dataSize = sizeof(SerialProtocol::BoardInfo);
                break;

            default:
                return;
        }
        Serial.write(static_cast<byte>(SerialProtocol::flagBegin));
        Serial.write(static_cast<byte>(messageType));
        Serial.write(dataSize);
        for (int i = 0; i < dataSize; i++) {
            Serial.write(data[i]);
        }
        Serial.write(static_cast<byte>(SerialProtocol::flagEnd));
    }

private:
    std::map<SerialProtocol::MessageType, Callback> callbacks;
    ReceivingStatus receivingStatus = ReceivingStatus::Idle;
    uint8_t headerBufferIndex = 0;
    uint16_t inDataSize = 0;
    SerialProtocol::MessageType mMessageType;
    std::array<byte, SerialProtocol::headerSize> headerBuffer;
    std::vector<byte> dataBuffer;
    std::vector<byte> responseBuffer;
    WireOled display;

    void resetToIdle() {
        receivingStatus = ReceivingStatus::Idle;
        headerBufferIndex = 0;
        inDataSize = 0;
        headerBuffer.fill(0);
        dataBuffer.clear();
    }
};
}
#endif //PLATFORMIO_SERIALCOMMUNICATOR_H
