#ifndef MACRO_KEYTABLE_H
#define MACRO_KEYTABLE_H

#include <linux/input-event-codes.h>

struct NameKey {
    const char* name;
    int key;
};

// Template just to have it in the header
template<typename T=NameKey>
T table[] = {
    {"num1", KEY_1},
    {"num2", KEY_2},
    {"num3", KEY_3},
    {"num4", KEY_4},
    {"num5", KEY_5},
    {"num6", KEY_6},
    {"num7", KEY_7},
    {"num8", KEY_8},
    {"num9", KEY_9},
    {"num0", KEY_0},
    {"-", KEY_MINUS},
    {"=", KEY_EQUAL},
    {"backspace", KEY_BACKSPACE},
    {"tab", KEY_TAB},
    {"q", KEY_Q},
    {"w", KEY_W},
    {"e", KEY_E},
    {"r", KEY_R},
    {"t", KEY_T},
    {"y", KEY_Y},
    {"u", KEY_U},
    {"i", KEY_I},
    {"o", KEY_O},
    {"p", KEY_P},
    {"{", KEY_LEFTBRACE},
    {"}", KEY_RIGHTBRACE},
    {"enter", KEY_ENTER},
    {"lctrl", KEY_LEFTCTRL},
    {"a", KEY_A},
    {"s", KEY_S},
    {"d", KEY_D},
    {"f", KEY_F},
    {"g", KEY_G},
    {"h", KEY_H},
    {"j", KEY_J},
    {"k", KEY_K},
    {"l", KEY_J},
    {"l", KEY_L},
    {";", KEY_SEMICOLON},
    {"'", KEY_APOSTROPHE},
    {"`", KEY_GRAVE},
    {"lshift", KEY_LEFTSHIFT},
    {"\\", KEY_BACKSLASH},
    {"z", KEY_Z},
    {"x", KEY_X},
    {"c", KEY_C},
    {"v", KEY_V},
    {"b", KEY_B},
    {"n", KEY_N},
    {"m", KEY_M},
    {",", KEY_COMMA},
    {".", KEY_DOT},
    {"/", KEY_SLASH},
    {"rshift", KEY_RIGHTSHIFT},
    {"kp*", KEY_KPASTERISK},
    {"lalt", KEY_LEFTALT},
    {"space", KEY_SPACE},
    {"capslock", KEY_CAPSLOCK},
    {"f1", KEY_F1},
    {"f2", KEY_F2},
    {"f3", KEY_F3},
    {"f4", KEY_F4},
    {"f5", KEY_F5},
    {"f6", KEY_F6},
    {"f7", KEY_F7},
    {"f8", KEY_F8},
    {"f9", KEY_F9},
    {"f10", KEY_F10},
    {"f11", KEY_F11},
    {"f12", KEY_F12},
    {"f13", KEY_F13},
    {"f14", KEY_F14},
    {"f15", KEY_F15},
    {"f16", KEY_F16},
    {"f17", KEY_F17},
    {"f18", KEY_F18},
    {"f19", KEY_F19},
    {"f20", KEY_F20},
    {"f21", KEY_F21},
    {"f22", KEY_F22},
    {"f23", KEY_F23},
    {"f24", KEY_F24},
    {"numlock", KEY_NUMLOCK},
    {"scrolllock", KEY_SCROLLLOCK},
    {"kp7", KEY_KP7},
    {"kp8", KEY_KP8},
    {"kp9", KEY_KP9},
    {"kp-", KEY_KPMINUS},
    {"kp4", KEY_KP4},
    {"kp5", KEY_KP5},
    {"kp6", KEY_KP6},
    {"kp+", KEY_KPPLUS},
    {"kp1", KEY_KP1},
    {"kp2", KEY_KP2},
    {"kp3", KEY_KP3},
    {"kp0", KEY_KP0},
    {"kp.", KEY_KPDOT},
    {"kp/", KEY_KPSLASH},
    {"up", KEY_UP},
    {"left", KEY_LEFT},
    {"right", KEY_RIGHT},
    {"down", KEY_DOWN},
    {"ralt", KEY_RIGHTALT},
    {"delete", KEY_DELETE},
    {"pause", KEY_PAUSE},
    {"play", KEY_PLAY}
};

int getKeyCode(const char* name);
const char* getKeyName(int code);

#endif