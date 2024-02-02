#ifndef PLATFORMIO_SERIALPROTOCOL_H
#define PLATFORMIO_SERIALPROTOCOL_H

#include <map>

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

        enum DataTypeCode : int {
            BoardSettingsCode,
        };

        struct BoardSettings {
            char name[4] = "   ";
            int hardwareRevision = 1;
            byte hardwareId[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
            byte ipAddress[4] = {0x00, 0x00, 0x00, 0x00};
        };

        static const std::map<DataTypeCode, uint16_t> DataSize;
    };

    const std::map<SerialProtocol::DataTypeCode, uint16_t> SerialProtocol::DataSize = {
        {SerialProtocol::DataTypeCode::BoardSettingsCode, sizeof(SerialProtocol::BoardSettings)},
    };
}

#endif // PLATFORMIO_SERIALPROTOCOL_H