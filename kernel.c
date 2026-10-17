#include "vga.h"
#include "string.h"
#include "drivers.h"

static int cursor_x = 0;
static int cursor_y = 0;
static char current_color = COLOR_WHITE;
static volatile char* video_memory = (volatile char*) 0xB8000;

// Указатель на свободную область памяти для буфера команд (64 байта)
static char* cmd_buffer = (char*) 0xB8FA0;
static int cmd_index = 0;

void clear_screen(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = COLOR_LIGHT_GREY;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void print_char(char c) {
    if (cursor_y >= VGA_HEIGHT) {
        clear_screen();
    }

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            int index = (cursor_y * VGA_WIDTH + cursor_x) * 2;
            video_memory[index] = ' ';
            video_memory[index + 1] = COLOR_LIGHT_GREY;
        }
    } else {
        int index = (cursor_y * VGA_WIDTH + cursor_x) * 2;
        video_memory[index] = c;
        video_memory[index + 1] = current_color;
        cursor_x++;
    }

    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
}

void print_string(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        print_char(str[i]);
    }
}

void print_color(const char* str, char color) {
    char old_color = current_color;
    current_color = color;
    print_string(str);
    current_color = old_color;
}

void print_prompt(void) {
    print_color("BitOS> ", COLOR_LIGHT_GREEN);
}

void execute_command(void) {
    print_char('\n');

    if (str_equal(cmd_buffer, "help")) {
        print_color("Commands: help, clear, info, drivers, reboot, shutdown\n", COLOR_YELLOW);
    } else if (str_equal(cmd_buffer, "clear")) {
        clear_screen();
    } else if (str_equal(cmd_buffer, "info")) {
        print_color("BitOS v0.4 - Power Management Ready.\n", COLOR_CYAN);
    } else if (str_equal(cmd_buffer, "drivers")) {
        print_color("Active Drivers:\n", COLOR_WHITE);
        print_color(" - PS/2 Keyboard (0x60)\n", COLOR_LIGHT_GREEN);
        print_color(" - PS/2 Mouse    (0x64)\n", COLOR_LIGHT_GREEN);
    } else if (str_equal(cmd_buffer, "reboot")) {
        print_color("Rebooting system...\n", COLOR_YELLOW);
        sys_reboot();
    } else if (str_equal(cmd_buffer, "shutdown")) {
        print_color("Shutting down...\n", COLOR_YELLOW);
        sys_shutdown();
    } else if (cmd_index > 0) {
        print_color("Unknown command: ", COLOR_LIGHT_RED);
        print_string(cmd_buffer);
        print_char('\n');
    }

    mem_clear(cmd_buffer, 64);
    cmd_index = 0;

    print_prompt();
}

void autoload_drivers(void) {
    print_string("Loading Keyboard Driver... ");
    init_keyboard();
    print_color("[ OK ]\n", COLOR_LIGHT_GREEN);

    print_string("Loading Mouse Driver...    ");
    init_mouse();
    print_color("[ OK ]\n\n", COLOR_LIGHT_GREEN);
}

void kernel_main(void) {
    clear_screen();
    mem_clear(cmd_buffer, 64);
    
    print_color("Starting BitOS...\n\n", COLOR_CYAN);
    autoload_drivers();

    print_string("Type 'help' for available commands.\n\n");
    print_prompt();

    while (1) {
        char key = keyboard_read_char();
        
        if (key != 0) {
            if (key == '\n') {
                cmd_buffer[cmd_index] = '\0';
                execute_command();
            } else if (key == '\b') {
                if (cmd_index > 0) {
                    cmd_index--;
                    cmd_buffer[cmd_index] = '\0';
                    print_char('\b');
                }
            } else if (cmd_index < 60) {
                cmd_buffer[cmd_index] = key;
                cmd_index++;
                print_char(key);
            }
        }
        
        volatile int delay = 50000;
        while (delay > 0) {
            delay--;
        }
    }
}
