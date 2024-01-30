#ifndef PLATFORMIO_ILEDBOARD_H
#define PLATFORMIO_ILEDBOARD_H

#include "Arduino.h"


namespace Frangitron {

    class ILEDBoard {
    public:
        virtual ~ILEDBoard() { }
        virtual void displayWrite(uint8_t row, uint8_t column, String text) = 0;
    };

}

#endif //PLATFORMIO_ILEDBOARD_H
