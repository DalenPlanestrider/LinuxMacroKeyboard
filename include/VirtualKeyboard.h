#ifndef MACRO_VIRTUALKEYBOARD_H
#define MACRO_VIRTUALKEYBOARD_H

#include "KeyDefs.h"
#include <time.h>

namespace macro {
    class VirtualKeyboard {
        int uinputFD;
        bool fdCreated;
        bool devCreated;
        long defaultSleeptime;
        timespec sleeptime;
    public:
        VirtualKeyboard(long pressDelayNanosecond = 0);
        ~VirtualKeyboard();


        void doKeyOutput(const MappedKey& key);
    };
}


#endif