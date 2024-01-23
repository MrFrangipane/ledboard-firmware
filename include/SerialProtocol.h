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
            responseOk = 0x41,  // "A"
            getBoardInfo,       // "B"
            responseBoardInfo,  // "C"
        };

        struct StructTest {
            int someInt = 0;
            float someFloat = 0.0;
        };

        struct BoardInfo {
            int boardVersion = 1;
            char ipAddress[16] = "192.168.100.201";
            float temperature = 0.75;
        };

        static constexpr uint8_t headerSize = 5;
        static constexpr byte flagBegin = 0x3c; // "<"
        static constexpr byte flagEnd = 0x3e;   // ">"
    };
}
#endif //PLATFORMIO_SERIALPROTOCOL_H
