#ifndef PLATFORMIO_SERIALPROTOCOL_H
#define PLATFORMIO_SERIALPROTOCOL_H


#include <cstdint>


namespace ledboard {

    class SerialProtocol {
    public:
        enum class DataType : byte {
            StructTestType = 0x30, // "0"
        };

        static constexpr uint8_t commandBufferSize = 6;

        static constexpr byte flagBegin = 0x3c;  // "<"
        static constexpr byte flagEnd = 0x3e;  // ">"

        static constexpr byte responseOk = 0x21;  // "!"

        static constexpr byte commandA = 0x41; // "A"
        static constexpr byte commandB = 0x42; // "B"
        static constexpr byte commandC = 0x43; // "C"
        static constexpr byte commandD = 0x44; // "D"
        static constexpr byte commandE = 0x45; // "E"
        static constexpr byte commandF = 0x46; // "F"
        static constexpr byte commandG = 0x47; // "G"
        static constexpr byte commandH = 0x48; // "H"
    };
}
#endif //PLATFORMIO_SERIALPROTOCOL_H

