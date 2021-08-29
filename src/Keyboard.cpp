#include "Keyboard.h"

#include <sys/ioctl.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <string>

namespace macro {
    std::string getEventHandler(const std::string& line) {
        bool hassysrq = false;

        static const char* sys = "sysrq";

        for(size_t i = 0; i < line.length() - 5; i++) {
            if (strncmp(line.c_str() + i, sys, 5) == 0) {
                hassysrq = true;
                break;
            }
        }

        if (!hassysrq) {
            return std::string();
        }

        static const char* e = "event";

        std::string out;

        for(size_t i = 0; i < line.length() - 5; i++) {
            if (strncmp(line.c_str() + i, e, 5) == 0) {
                const char* numstart = line.c_str() + i + 5;

                size_t offset = 0;
                while(numstart[offset] >= '0' && numstart[offset] <= '9') {
                    out += numstart[offset];
                    offset++;
                }

                break;
            }
        }

        return out;
    }

    std::string getDeviceHandlerFromName(const std::string& name) {
        FILE* devfd = fopen("/proc/bus/input/devices", "r");
    
        if (devfd == NULL) {
            std::cerr << "Failed to open /proc/bus/input/devices\n";
            return std::string();
        }

        size_t len = 0;
        char* line = NULL;
        ssize_t readbytes;

        std::vector<std::string> lines;

        while((readbytes = getline(&line, &len, devfd) != -1)) {
            lines.emplace_back(line);
            free(line);
            line = nullptr;
        }

        fclose(devfd);

        for(size_t i = 0; i < lines.size(); i++) {
            auto& line = lines[i];

            if (line[0] == 'N') {
                const char* devname = line.c_str() + strlen("N: Name=\"");
                if (strncmp(devname, name.c_str(), name.length()) == 0) {
                    std::string out = "/dev/input/event";
                    std::string append;

                    for(size_t j = 0; j < 12; j++) {
                        if (lines[i + j][0] == 'H') {
                            append = getEventHandler(lines[i + j]);
                            break;
                        }
                    }

                    // use this empty checking system, because we need the device with the sysrq thing
                    if (!append.empty()) {
                        return out + append;
                    }
                }
            }
        }

        return std::string();
    }

    Keyboard::Keyboard(const std::string& deviceName) {
        bool opened = false;
        bool owned = false;

        std::string handler = getDeviceHandlerFromName(deviceName);

        if (handler.empty()) {
            std::cerr << "Failed to find event hander for device: " << deviceName << "\n";
            return;
        }

        handlerFD = open(handler.c_str(), O_RDONLY);

        if (handlerFD == -1) {
            std::cerr << "Can't open hander: " << handler.c_str() << " " << strerror(errno) << "\n";
            return;
        }

        opened = true;

        if (ioctl(handlerFD, EVIOCGRAB, 1) == -1) {
            std::cerr << "Can't take ownership of device\n";
            return;
        }

        owned = true;
    }

    Keyboard::~Keyboard() {
        if (owned) {
            ioctl(handlerFD, EVIOCGRAB, 0);
        }

        if (opened) {
            close(handlerFD);
        }
    }

    int Keyboard::read(input_event& ev) {
        n = ::read(handlerFD, &ev, sizeof(ev));
        if (n == (ssize_t)-1) {
            if (errno == EINTR) {
                return 1;
            }
            else {
                return 2;
            }
        }
        else {
            // Pretty unlikely to happen, but just in case
            if (n != sizeof(ev)) {
                errno = EIO;
                return 2;
            }
        }

        if (ev.type == EV_KEY) {
            if (ev.value == 1 || ev.value == 2) {
                if (ev.code == KEY_ESC || ev.code == 108)
                    return 2;
                else 
                    return 0;
            }
            else return 1;
        }
        else return 1;
    }
} 
