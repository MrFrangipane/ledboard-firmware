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
        Idle, ReceivingCommand, ReceivingData
    };

    class SerialCommunicator {
    public:
        using Callback = std::function<byte(WireOled &display, const std::vector<byte> &, SerialProtocol::DataType)>;

        void registerCallback(byte commandCode, Callback callback) {
            callbacks[commandCode] = callback;
        }

        void init() {
            display.init();
            display.setContrast(0);
            display.write("Hello");
        }

        void poll() {
            if (!Serial.dtr()) {
                return;
            }

            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

            while (Serial.available() > 0) {
                byte incomingByte = Serial.read();

                if (receivingStatus == ReceivingStatus::Idle) {
                    if (incomingByte == SerialProtocol::flagBegin) {
                        receivingStatus = ReceivingStatus::ReceivingCommand;
                    }
                }
                else if (receivingStatus == ReceivingStatus::ReceivingCommand) {
                    if (commandBufferIndex < SerialProtocol::commandBufferSize) {
                        commandBuffer[commandBufferIndex] = incomingByte;
                        commandBufferIndex++;
                    }
                    else {
                        dataSize =
                            (commandBuffer[5] << 24) |
                            (commandBuffer[4] << 16) |
                            (commandBuffer[3] << 8) |
                            commandBuffer[2];
                        receivingStatus = ReceivingStatus::ReceivingData;
                    }
                }

                if (receivingStatus == ReceivingStatus::ReceivingData) {
                    if (dataBuffer.size() < dataSize) {
                        dataBuffer.push_back(incomingByte);
                    }
                    else if (dataBuffer.size() == dataSize && incomingByte == SerialProtocol::flagEnd) {
                        auto dataType = static_cast<SerialProtocol::DataType>(commandBuffer[1]);
                        byte commandCode = commandBuffer[0];

                        if (callbacks.find(commandCode) != callbacks.end()) {
                            byte response = callbacks[commandCode](display, dataBuffer, dataType);
                            Serial.write(response);
                        }
                        receivingStatus = ReceivingStatus::Idle;
                        commandBufferIndex = 0;
                        dataSize = 0;
                        commandBuffer.fill(0);
                        dataBuffer.clear();
                    }
                }
            }
        }

    private:
        std::map<byte, Callback> callbacks;
        ReceivingStatus receivingStatus = ReceivingStatus::Idle;
        uint8_t commandBufferIndex = 0;
        uint16_t dataSize = 0;
        std::array<byte, SerialProtocol::commandBufferSize> commandBuffer;
        std::vector<byte> dataBuffer;
        WireOled display;
    };
}
#endif //PLATFORMIO_SERIALCOMMUNICATOR_H
