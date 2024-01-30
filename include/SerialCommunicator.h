#ifndef PLATFORMIO_SERIALCOMMUNICATOR_H
#define PLATFORMIO_SERIALCOMMUNICATOR_H


#include <Arduino.h>
#include <array>
#include <functional>
#include <vector>

#include "ILEDBoard.h"
#include "SerialProtocol.h"


namespace Frangitron {

    class SerialCommunicator {
    public:
        using ReceiveCallback = std::function<void(ILEDBoard *, const std::vector<byte> &)>;
        using SendCallback = std::function<void(ILEDBoard *, std::vector<byte> &)>;

        void init(ILEDBoard *board1) {
            board = board1;
        }

        void registerReceiveCallback(SerialProtocol::DataTypeCode dataTypeCode1, ReceiveCallback callback) {
            receiveCallbacks[dataTypeCode1] = callback;
        }

        void registerSendCallback(SerialProtocol::DataTypeCode dataTypeCode1, SendCallback callback) {
            sendCallbacks[dataTypeCode1] = callback;
        }

        void poll() {
            if (!Serial.dtr()) {
                return;
            }

            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

            while (Serial.available() > 0) {
                byte incomingByte = Serial.read();

                if (receivingStatus == ReceivingStatus::Idle && incomingByte == SerialProtocol::flagBegin) {
                    receivingStatus = ReceivingStatus::ReceivingHeader;
                    return;
                }

                if (receivingStatus == ReceivingStatus::ReceivingHeader) {
                    if (headerBufferIndex < SerialProtocol::headerSize) {
                        headerBuffer[headerBufferIndex] = incomingByte;
                        headerBufferIndex++;
                    } else {
                        direction = static_cast<SerialProtocol::Direction>(headerBuffer[0]);
                        dataTypeCode = static_cast<SerialProtocol::DataTypeCode>(headerBuffer[1]);
                        if (direction == SerialProtocol::Direction::Send) {
                            inDataSize = SerialProtocol::DataSize.at(dataTypeCode);
                        } else {
                            inDataSize = 0;
                        }
                        receivingStatus = ReceivingStatus::ReceivingData;
                    }
                }

                if (receivingStatus == ReceivingStatus::ReceivingData) {
                    if (receiveDataBuffer.size() < inDataSize) {
                        receiveDataBuffer.push_back(incomingByte);
                        continue;
                    } else if (receiveDataBuffer.size() == inDataSize && incomingByte == SerialProtocol::flagEnd) {
                        if (direction == SerialProtocol::Direction::Send &&
                            receiveCallbacks.find(dataTypeCode) != receiveCallbacks.end()) {
                            receiveCallbacks.at(dataTypeCode)(board, receiveDataBuffer);
                        } else if (direction == SerialProtocol::Direction::Receive &&
                                   sendCallbacks.find(dataTypeCode) != sendCallbacks.end()) {
                            sendCallbacks.at(dataTypeCode)(board, sendDataBuffer);
                            sendResponse();
                        }
                    }
                    resetToIdle();
                }
            }
        }

        void sendResponse() {
            Serial.write(static_cast<byte>(SerialProtocol::flagBegin));
            Serial.write(static_cast<byte>(dataTypeCode));
            Serial.write(sendDataBuffer.data(), sendDataBuffer.size());
            Serial.write(static_cast<byte>(SerialProtocol::flagEnd));
        }

    private:
        enum class ReceivingStatus : byte {
            Idle, ReceivingHeader, ReceivingData
        };

        ILEDBoard *board;
        std::map<SerialProtocol::DataTypeCode, ReceiveCallback> receiveCallbacks;
        std::map<SerialProtocol::DataTypeCode, SendCallback> sendCallbacks;
        ReceivingStatus receivingStatus = ReceivingStatus::Idle;
        uint8_t headerBufferIndex = 0;
        std::array<byte, SerialProtocol::headerSize> headerBuffer;
        SerialProtocol::DataTypeCode dataTypeCode;
        SerialProtocol::Direction direction = SerialProtocol::Direction::Receive;
        uint16_t inDataSize = 0;
        std::vector<byte> receiveDataBuffer;
        std::vector<byte> sendDataBuffer;

        void resetToIdle() {
            receivingStatus = ReceivingStatus::Idle;
            headerBufferIndex = 0;
            inDataSize = 0;
            headerBuffer.fill(0);
            receiveDataBuffer.clear();
            sendDataBuffer.clear();
        }
    };
}

#endif //PLATFORMIO_SERIALCOMMUNICATOR_H
