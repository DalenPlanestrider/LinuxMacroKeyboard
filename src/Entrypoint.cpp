#include "VirtualKeyboard.h"
#include "Keyboard.h"
#include "KeyMap.h"
#include "KeyTable.h"

#include <iostream>
#include <signal.h>
#include <string>
#include <vector>
#include <cstring>
#include <linux/input.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

bool running = true;

void sigtermhandler(int signum, siginfo_t* info, void* ptr) {
    std::cout << "\nCaught signal, shutting down gracefully\n";
    running = false;
}

bool exists(const std::string& path) {
    struct stat s{};
    if (stat(path.c_str(), &s) == -1) {
        return false;
    }
    else return true;
}

std::vector<std::string> listDirFiles(const std::string& path) {
    DIR* directory;
    struct dirent* entry;

    directory = opendir(path.c_str());

    std::vector<std::string> out;

    if (directory) {
        while((entry = readdir(directory)) != NULL) {
            if (strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0) {
                continue;
            }

            out.emplace_back(entry->d_name);
        }
    }

    return out;
}

const char* exampleMap[] = {
    "//comments can be made with two slashes as the first two line characters\n",
    "//find your device name with cat /proc/bus/input/devices\n",
    "//you will need permission to acces uinput and the device event handler\n", 
    "//which means either sudo, or making your user part of the appropriate group\n",
    "//Keys are defined by:  [key] [action(s)] [behaviour modifiers]\n",
    "//Keys are accpeted as names or scancodes\n",
    "//Use quotes to enclose multiple key actions or behaviour modifiers\n",
    "//Global key delay can be set by specifying delay on the device\n",
    "\n",
    "device \"name goes here\"\n",
    "45 \"lctrl+ z lctrl-\" -delay:50000000 \n",
    "t backspace -allowrepeat\n",
    "z kp*\n",
    "y \"h e l l o\"\n",
};

void printkeys() {
    for(auto& entry : table<>) {
        std::cout << entry.name << " (" << entry.key << ")\n";
    }
}

long globalInputDelay = 0;

void setdelay(const char* number) {
    size_t pos = 0;
    while(number[pos] == '\0') {
        if (number[pos] < '0' || number[pos] > '9') {
            std::cerr << "Unrecognized delay time of: " << number << "\n";
            return;
        }

        pos++;
    }

    globalInputDelay = atol(number);
}

void printhelp() {
    std::cout << "\n"
    << "-help - view this message\n"
    << "-listkeys - list the key names recognized and their scancodes\n"
    << "-delay:[number] - set the global input delay time in nanoseconds\n";
}

bool argparse(int argc, char** argv) {
    for(size_t i = 0; i < argc; i++) {
        char* param = argv[i];

        if (param[0] == '-') {
            if (strcmp(param, "-help") == 0) {
                printhelp();
                return true;
            }
            else if (strcmp(param, "-listkeys") == 0) {
                printkeys();
                return true;
            }
            else if (strncmp(param, "-delay:", 7)==0) {
                setdelay(param + 7);
            }
        }
    }

    return false;
}

int main(int argc, char** argv) {
    struct sigaction term{};
    struct sigaction inter{};
    struct sigaction quit{};
    term.sa_sigaction = sigtermhandler;
    term.sa_flags = SA_SIGINFO;
    inter.sa_sigaction = sigtermhandler;
    inter.sa_flags = SA_SIGINFO;
    quit.sa_sigaction = sigtermhandler;
    quit.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &term, NULL);
    sigaction(SIGINT, &inter, NULL);
    sigaction(SIGQUIT, &quit, NULL);

    if (argc > 1) {
        if (argparse(argc, argv)) {
            return EXIT_SUCCESS;
        }
    }

    if (!exists("modules")) {
        mkdir("modules", S_IRWXU);
    }

    if (!exists("keymaps")) {
        mkdir("keymaps", S_IRWXU);
        int fd = open("keymaps/default.map", O_CREAT | O_WRONLY, S_IRWXU);
        for(const char* line : exampleMap) {
            auto len = strlen(line);
            write(fd, line, len);
        }
        close(fd);

        std::cout << "Example keymap file generated\n";
        return EXIT_SUCCESS;
    }

    

    auto maps = listDirFiles("keymaps");
    auto modules = listDirFiles("modules");

    std::string chosen_map;

    if (maps.size() == 0) {
        std::cerr << "No keymaps in the keymap directory\n";
        return EXIT_FAILURE;
    }
    else if (maps.size() == 1) {
        chosen_map = "keymaps/" + maps[0];
    }
    else {
        std::cout << "Choose your map\n";

        for(size_t i = 0; i < maps.size(); i++) {
            std::cout << i << ": " << maps[i] << "\n";
        }

        std::string inputNum;
        int selectedID = 0;

        bool selected = false;
        while (!selected) {
            std::cout << "Enter a number of a map to select it, or exit to close the program\n";
            std::cin >> inputNum;

            if (inputNum == "exit") {
                exit(0);
            }

            for(auto& character : inputNum) {
                if (character < '0' || character > '9') {
                    std::cout << "Invalid input, only numbers\n";
                    continue;
                }
            }

            selectedID = atoi(inputNum.c_str());

            if (selectedID >= maps.size() || selectedID < 0) {
                std::cout << "Not a valid map\n";
                continue;
            }

            selected = true;
        }

        chosen_map = "keymaps/" + maps[selectedID];
    }

    auto map = macro::KeyMap(chosen_map);
    auto kbd = macro::Keyboard(map.getDeviceName());
    auto vkbd = macro::VirtualKeyboard(globalInputDelay);

    input_event ev;
    int code = 0;
    while (running) {
        code = kbd.read(ev);
        if (code == 1)
            continue;
        else if (code == 2)
            break;

        auto& mkey = map.translateEvent(ev);
        vkbd.doKeyOutput(mkey);
    }

    return EXIT_SUCCESS;
}