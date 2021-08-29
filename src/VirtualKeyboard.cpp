#include "VirtualKeyboard.h"

#include <iostream>
#include <cstring>

#include <sys/ioctl.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <fcntl.h>

namespace macro {
    VirtualKeyboard::VirtualKeyboard(long pressDelayNanoseconds) {
        devCreated = false;
        fdCreated = false;
        uinputFD = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

        if (uinputFD == -1) {
            std::cerr << "Failed to open dev/uinput\n";
            return;
        }

        fdCreated = true;

        ioctl(uinputFD, UI_SET_EVBIT, EV_KEY);

        int min_inclusive = 1;
        int max_inclusive = 194;

        for (ssize_t i = min_inclusive; i < max_inclusive + 1; i++) {
            ioctl(uinputFD, UI_SET_KEYBIT, i);
        }

        struct uinput_setup usetup;

        memset(&usetup, 0, sizeof(usetup));
        usetup.id.bustype = BUS_USB;
        usetup.id.vendor = 0x1234;
        usetup.id.product = 0x5678;
        strcpy(usetup.name, "Macro vkeyboard");

        if (ioctl(uinputFD, UI_DEV_SETUP, &usetup) == -1) {
            std::cerr << "Failed to setup uinput device\n";
            return;
        }

        if (ioctl(uinputFD, UI_DEV_CREATE) == -1) {
            std::cerr << "Failed to create uinput device\n";
            return;
        }

        devCreated = true;
        defaultSleeptime = pressDelayNanoseconds;
    }

    VirtualKeyboard::~VirtualKeyboard() {
        if (devCreated) {
            ioctl(uinputFD, UI_DEV_DESTROY);
        }
        if (fdCreated) {
            close(uinputFD);
        }
    }

    void VirtualKeyboard::doKeyOutput(const MappedKey& mkey) {
        struct input_event ie{};

        if (mkey.sleeptime != 0) {
            sleeptime.tv_nsec = mkey.sleeptime;
        }
        else {
            sleeptime.tv_nsec = defaultSleeptime;
        }

        for(auto& key : mkey.actions) {
            if (key.action == None) {
                break;
            }
            else if (key.action == Press) {
                ie.type = EV_KEY;
                ie.code = key.code;
                ie.value = 1;

                write(uinputFD, &ie, sizeof(ie));

                ie.type = EV_SYN;
                ie.code = SYN_REPORT;
                ie.value = 0;

                write(uinputFD, &ie, sizeof(ie));

                nanosleep(&sleeptime, nullptr);
            }
            else if (key.action == Release) {
                ie.type = EV_KEY;
                ie.code = key.code;
                ie.value = 0;

                write(uinputFD, &ie, sizeof(ie));

                ie.type = EV_SYN;
                ie.code = SYN_REPORT;
                ie.value = 0;

                write(uinputFD, &ie, sizeof(ie));

                nanosleep(&sleeptime, nullptr);
            }
            else if (key.action == Tap) {
                ie.type = EV_KEY;
                ie.code = key.code;
                ie.value = 1;
 
                write(uinputFD, &ie, sizeof(ie));

                ie.type = EV_SYN;
                ie.code = SYN_REPORT;
                ie.value = 0;

                write(uinputFD, &ie, sizeof(ie));

                nanosleep(&sleeptime, nullptr);

                ie.type = EV_KEY;
                ie.code = key.code;
                ie.value = 0;

                write(uinputFD, &ie, sizeof(ie));

                ie.type = EV_SYN;
                ie.code = SYN_REPORT;
                ie.value = 0;

                write(uinputFD, &ie, sizeof(ie));

                nanosleep(&sleeptime, nullptr);
            }
            else if (key.action == Special) {
                if (key.specialfunc)
                    key.specialfunc(mkey);
                else
                    std::cerr << "Key mapped to null function\n";
            }
        }
    }
}
