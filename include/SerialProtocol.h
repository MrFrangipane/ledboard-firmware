#ifndef PLATFORMIO_SERIALPROTOCOL_H
#define PLATFORMIO_SERIALPROTOCOL_H


#include <cstdint>


namespace ledboard {

class SerialProtocol {
    /*
     * Message topology
     *
     * ```
     * |   0   |       1      | 2 | 3 | 4 | 5 |   n  | 6 + n |
     * | begin | message type |   data size   | data |  end  |
     * |-------|           header             | data |-------|
     * ```
     */
public:
    enum class MessageType : byte {
        Illuminate = 0x41,  // "A"
        Configure = 0x42    // "B"
    };

    struct IlluminatedLed {
        int ledIndex = 0;
        int r = 0;
        int g = 0;
        int b = 0;
        int w = 0;
    };

    enum class PixelType : int {
        RGB,
        RGBW
    };

    struct Configuration {
        int pixelType = static_cast<int>(PixelType::RGB);
        int pixelCount = 60;
    };

    static constexpr uint8_t headerSize = 5;
    static constexpr byte flagBegin = 0x3c; // "<"
    static constexpr byte flagEnd = 0x3e;   // ">"

    static std::map<MessageType, int> messageTypeToDataSize;
};

std::map<SerialProtocol::MessageType, int> SerialProtocol::messageTypeToDataSize = {
    {SerialProtocol::MessageType::Illuminate, sizeof(SerialProtocol::IlluminatedLed)},
    {SerialProtocol::MessageType::Configure, sizeof(SerialProtocol::Configuration)}
};

}
#endif //PLATFORMIO_SERIALPROTOCOL_H
