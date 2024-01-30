#ifndef PLATFORMIO_SERIALPROTOCOL_H
#define PLATFORMIO_SERIALPROTOCOL_H


#include <map>
#include <variant>

#include <Arduino.h>


namespace Frangitron {

    class SerialProtocol {
        /*
         * Message topology
         *
         * ```
         * |   0   |    1      |     2     |   n  | 3 + n |
         * | begin | direction | data type | data |  end  |
         * |-------| ------ header ------- | data |-------|
         * ```
         */
    public:
        enum class Direction : int {
            Send,
            Receive
        };

        static constexpr uint8_t headerSize = 2;
        static constexpr byte flagBegin = 0x3c;
        static constexpr byte flagEnd = 0x3e;

        /* //////////////////////////////// */

        enum class PixelType : int {
            RGB,
            RGBW
        };

        enum class DataTypeCode : byte {
            Configuration = 0x41,
            Illumination = 0x42,
        };

        struct Configuration {
            int pixelType = static_cast<int>(PixelType::RGB);
            int pixelCount = 60;
            byte boardID[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            byte ipAddress[4] = {0, 0, 0, 0};
        };

        struct Illumination {
            int startPixel = 0;
            int endPixel = 0;
            int brightness = 0;
        };

        static const std::map<DataTypeCode, uint16_t> DataSize;
    };

    const std::map<SerialProtocol::DataTypeCode, uint16_t> SerialProtocol::DataSize = {
            {SerialProtocol::DataTypeCode::Configuration, sizeof(SerialProtocol::Configuration)},
            {SerialProtocol::DataTypeCode::Illumination,  sizeof(SerialProtocol::Illumination)},
    };
}

#endif //PLATFORMIO_SERIALPROTOCOL_H
