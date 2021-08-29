#ifndef MACRO_KEYDEFS_H
#define MACRO_KEYDEFS_H

namespace macro {
    enum KeyAction {
        None = 0,
        Tap = 1,
        Press = 2,
        Release = 3,
        Special = 4
    };

    struct MappedKey;
    typedef void (*SpecialFuncPtr)(const MappedKey& mapkey);

    struct KeyOutput {
        int code;
        KeyAction action;
        SpecialFuncPtr specialfunc;
    };


    struct MappedKey {
        int code;
        bool allowRepeat;
        KeyOutput actions[10]; // A key can't have more than 10 actions mapped to it
    };
}
    


#endif