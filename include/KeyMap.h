#ifndef MACRO_KEYMAP_H
#define MACRO_KEYMAP_H

#include "KeyDefs.h"

#include <vector>
#include <string>
#include <linux/input.h>

namespace macro {
    class KeyMap {
        std::vector<MappedKey> keys;
        MappedKey noAction;
        std::string deviceName;
    public:
        KeyMap(const std::string& filepath);
        ~KeyMap();

        const std::string& getDeviceName();
        const MappedKey& translateEvent(const input_event& ev) const;
    };
}

#endif