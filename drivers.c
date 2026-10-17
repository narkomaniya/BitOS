#include "drivers.h"

static const char keymap[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

void ps2_wait(void) {
    int timeout = 100000;
    while (timeout--) {
        if ((inb(0x64) & 2) == 0) return;
    }
}

void init_keyboard(void) {
    while (inb(0x64) & 1) {
        inb(0x60);
    }
    ps2_wait();
    outb(0x64, 0xAE);
}

void init_mouse(void) {
    ps2_wait();
    outb(0x64, 0xA8);
    ps2_wait();
    outb(0x64, 0xD4);
    ps2_wait();
    outb(0x60, 0xF4);
}

char keyboard_read_char(void) {
    if (inb(0x64) & 1) {
        unsigned char scancode = inb(0x60);
        if (scancode < 59) {
            return keymap[scancode];
        }
    }
    return 0;
}

void sys_reboot(void) {
    ps2_wait();
    outb(0x64, 0xFE);
}

void sys_shutdown(void) {
    outb(0x604, 0x00);
}
