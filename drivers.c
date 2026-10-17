#include "drivers.h"

static const char keymap[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

#define KBD_BUFFER_SIZE 256
#define IDT_SIZE 256

static volatile char kbd_buffer[KBD_BUFFER_SIZE];
static volatile int kbd_head = 0;
static volatile int kbd_tail = 0;
static volatile int interrupts_enabled = 0;

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
    if (interrupts_enabled) {
        if (kbd_head == kbd_tail) return 0;
        char c = kbd_buffer[kbd_tail];
        kbd_tail = (kbd_tail + 1) % KBD_BUFFER_SIZE;
        return c;
    }

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

// ============================================================================
// Аппаратные прерывания клавиатуры (IDT + IRQ1)
// ============================================================================

#pragma pack(push, 1)
struct idt_entry {
    unsigned short base_low;
    unsigned short selector;
    unsigned char  zero;
    unsigned char  flags;
    unsigned short base_high;
};

struct idt_ptr {
    unsigned short limit;
    unsigned int   base;
};
#pragma pack(pop)

static struct idt_entry idt_table[IDT_SIZE];
static struct idt_ptr idt_pointer;

// Эти функции лежат в boot.s
extern void keyboard_handler_asm(void);
extern void load_idt_asm(unsigned int idtp_addr);

void keyboard_handler_c(void) {
    unsigned char scancode = inb(0x60);
    
    if (!(scancode & 0x80) && scancode < 59) {
        char c = keymap[scancode];
        if (c != 0) {
            int next = (kbd_head + 1) % KBD_BUFFER_SIZE;
            if (next != kbd_tail) {
                kbd_buffer[kbd_head] = c;
                kbd_head = next;
            }
        }
    }
    
    outb(0x20, 0x20);
}

void init_keyboard_driver(void) {
    int i;
    unsigned int handler_addr;

    init_keyboard();

    // Настройка контроллера прерываний PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0xFD);
    outb(0xA1, 0xFF);

    idt_pointer.limit = (unsigned short)((sizeof(struct idt_entry) * IDT_SIZE) - 1);
    idt_pointer.base  = (unsigned int)&idt_table[0];

    for (i = 0; i < IDT_SIZE; i++) {
        idt_table[i].base_low  = 0;
        idt_table[i].selector  = 0;
        idt_table[i].zero      = 0;
        idt_table[i].flags     = 0;
        idt_table[i].base_high = 0;
    }

    handler_addr = (unsigned int)keyboard_handler_asm;
    idt_table[0x21].base_low  = (unsigned short)(handler_addr & 0xFFFF);
    idt_table[0x21].base_high = (unsigned short)((handler_addr >> 16) & 0xFFFF);
    idt_table[0x21].selector  = 0x08;
    idt_table[0x21].flags     = 0x8E;

    interrupts_enabled = 1;

    load_idt_asm((unsigned int)&idt_pointer);
}
