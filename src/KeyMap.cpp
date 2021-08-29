#include "KeyMap.h"
#include "KeyTable.h"

#include <iostream>
#include <cstring>

namespace macro {

    std::vector<std::string> split(const std::string& input) {
        std::vector<std::string> out(1);

        size_t offset = 0;
        bool splitting = true;
        for(size_t i = 0; i < input.size(); i++) {
            if (input[i] == '\n')
                continue;

            if (splitting) {
                if (input[i] == ' ') {
                    offset++;
                    out.emplace_back();
                }
                else if (input[i] == '\"') {
                    splitting = !splitting;
                }
                else {
                    out[offset] += input[i];
                }
            }
            else {
                if (input[i] == '\"') {
                    splitting = !splitting;
                }
                else {
                    out[offset] += input[i];
                }
            }
        }

        return out;
    }
    
    KeyMap::KeyMap(const std::string& filepath) {
        noAction = MappedKey{};

        FILE* mapfd = fopen(filepath.c_str(), "r");
    
        if (mapfd == NULL) {
            std::cerr << "Failed to open " << filepath.c_str() << " - make sure it exists\n";
            return;
        }

        size_t len = 0;
        char* line = NULL;
        ssize_t readbytes;

        std::string devline;
        std::vector<std::string> lines;

        while((readbytes = getline(&line, &len, mapfd) != -1)) {
            if (strncmp("device ", line, 7) == 0) {
                char* name = line + 7;

                size_t offset = 0;
                while(name[offset] != '\0') {
                    if (name[offset] != '\"' && name[offset] != '\n') {
                        devline += name[offset];
                    }
                    offset++;
                }
            }
            else {
                if ((line[0] == '/' && line[1] == '/') || strlen(line) == 0 || line[0] == '\n') {
                    free(line);
                    line = nullptr;
                    continue;
                }
                else
                    lines.emplace_back(line);
            }

            free(line);
            line = nullptr;
        }

        fclose(mapfd);

        deviceName = devline;

        for(size_t i = 0; i < lines.size(); i++) {
            auto comps = split(lines[i]);
            if (comps.size() == 1) {
                printf("Key %s not mapped to anything", comps[0].c_str());
                continue;
            }

            MappedKey k{};

            bool isNumeric = true;;

            for(auto& chara : comps[0]) {
                if (chara <= '0' || chara >= '9') {
                    isNumeric = false;
                    break;
                }
            }

            if (isNumeric)
                k.code = atoi(comps[0].c_str());
            else {
                auto code = getKeyCode(comps[0].c_str());
                if (code == -1) {
                    std::cerr << "Key " << comps[0] << " is not a recognized key name\n";
                    continue;
                }
                else k.code = code;
            }

            auto keys = split(comps[1]);

            for(int j = 0; j < keys.size() && j < 10; j++) {
                auto& key = keys[j];
                auto len = key.length();
                char last = key[len - 1];
                
                KeyAction act = None;
                if (last == '+') {
                    act = Press;
                    len--;
                }
                else if (last == '-') {
                    act = Release;
                    len--;
                }
                else {
                    act = Tap;
                }

                std::string keycode;
                for(int k = 0; k < len; k++) {
                    keycode += key[k];
                }

                KeyOutput output{};
                output.action = act;
                bool isNumeric = true;

                for(auto& chara : keycode) {
                    if (chara <= '0' || chara >= '9') {
                        isNumeric = false;
                        break;
                    }
                }

                if (isNumeric)
                    output.code = atoi(keycode.c_str());
                else {
                    auto code = getKeyCode(keycode.c_str());
                    if (code == -1) {
                        std::cerr << "Key " << keycode << " is not a recognized key name\n";
                        continue;
                    }
                    else output.code = code;
                }


                k.actions[j] = output;
            }

            for(int j = 2; j < keys.size(); j++) {
                auto mod = keys[j];
                if (mod[0] == '-') {
                    if (strcmp(mod.c_str(), "-allowrepeat") == 0) {
                        k.allowRepeat = true;
                    }
                }
                else {
                    std::cerr << "Unrecognized behaviour modifier: " << mod;
                }
            }

            this->keys.emplace_back(k);
        }
    }

    KeyMap::~KeyMap() = default;

    const MappedKey& KeyMap::translateEvent(const input_event& ev) const {
        for(auto& m : keys) {
            if (m.code == ev.code) {
                if (ev.value == 2) {
                    if (m.allowRepeat)
                        return m;
                    else return noAction;
                }
                else return m;
            }
        }

        std::cout << "Key " << ev.code << " (" << getKeyName(ev.code) << ") is not mapped\n";
        
        return noAction;
    }

    const std::string& KeyMap::getDeviceName() {
        return deviceName;
    }
}