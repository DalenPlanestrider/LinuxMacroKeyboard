#ifndef MACRO_KEYBOARD_H
#define MACRO_KEYBOARD_H

#include <string>
#include <linux/input.h>

namespace macro {
    class Keyboard {
        int handlerFD;
        bool opened;
        bool owned;
        ssize_t n;
    public:
        Keyboard(const std::string& deviceName);
        ~Keyboard();
        
        // Run in a loop. 0 Means good, 1 means continue, 2 means break
        int read(input_event& ev);
    };
}


#endif