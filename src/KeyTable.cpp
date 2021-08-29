#include "KeyTable.h"
#include <cstring>


int getKeyCode(const char* name) {
    for(auto& ent : table<>) {
        if (strcmp(ent.name, name) == 0) {
            return ent.key;
        }
    }

    return -1;
}

const char* getKeyName(int code) {
    for(auto& ent : table<>) {
        if (code == ent.key) {
            return ent.name;
        }
    }

    return "Unknown key";
}