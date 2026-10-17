#include "vga.h"
#include "bios.hpp"
#include "gui.hpp"

// Подключаем только гарантированно рабочие низкоуровневые вызовы железа
extern "C" {
    #include "drivers.h"
    #include "sound.h"
    void kernel_main(void);
}

static int cursor_x = 0;
static int cursor_y = 6; // Начинаем с 6-й строки, чтобы не стереть окно TUI
static char current_color = COLOR_WHITE;
static volatile unsigned char* video_memory = (volatile unsigned char*) 0xB8000;

// Выделяем память под буфер прямо внутри ядра без использования внешних Си-функций
static char internal_cmd_buffer[64];
static int cmd_index = 0;

// Своя простейшая функция сравнения строк прямо внутри файла
static int local_str_equal(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return 0;
        i++;
    }
    return (s1[i] == '\0' && s2[i] == '\0');
}

void clear_screen(void) {
    for (int i = 6 * VGA_WIDTH * 2; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = COLOR_LIGHT_GREY;
    }
    cursor_x = 0;
    cursor_y = 6;
}

void print_char(char c) {
    if (cursor_y >= VGA_HEIGHT) {
        for (int y = 7; y < VGA_HEIGHT; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                int dest_index = ((y - 1) * VGA_WIDTH + x) * 2;
                int src_index = (y * VGA_WIDTH + x) * 2;
                video_memory[dest_index] = video_memory[src_index];
                video_memory[dest_index + 1] = video_memory[src_index + 1];
            }
        }
        for (int x = 0; x < VGA_WIDTH; x++) {
            int index = ((VGA_HEIGHT - 1) * VGA_WIDTH + x) * 2;
            video_memory[index] = ' ';
            video_memory[index + 1] = COLOR_LIGHT_GREY;
        }
        cursor_y = VGA_HEIGHT - 1;
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
        video_memory[index] = (unsigned char)c;
        video_memory[index + 1] = (unsigned char)current_color;
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

    // Обновляем статус запущенной команды в верхнем баре
    WindowManager::UpdateStatusBar(20, 0, internal_cmd_buffer, 0x1F, 15);

    // БАЗОВЫЕ БЕЗОПАСНЫЕ КОМАНДЫ
    if (local_str_equal(internal_cmd_buffer, "help")) {
        print_color("--- BitOS Core Commands ---\n", COLOR_YELLOW);
        print_color(" help        ", COLOR_LIGHT_GREEN); print_string("- Show this help window\n");
        print_color(" cls         ", COLOR_LIGHT_GREEN); print_string("- Clear the terminal screen\n");
        print_color(" info        ", COLOR_LIGHT_GREEN); print_string("- Display system version\n");
        print_color(" beep        ", COLOR_LIGHT_GREEN); print_string("- Test audio hardware speaker\n");
        print_color(" bios-info   ", COLOR_LIGHT_GREEN); print_string("- Check motherboard BIOS status\n");
        print_color(" reboot      ", COLOR_LIGHT_GREEN); print_string("- Restart the computer\n");
        print_color(" shutdown    ", COLOR_LIGHT_GREEN); print_string("- Turn off the power\n");
    } 
    else if (local_str_equal(internal_cmd_buffer, "cls")) {
        clear_screen();
    } 
    else if (local_str_equal(internal_cmd_buffer, "info")) {
        print_color("BitOS v0.5 - Clean C++ Stable Build.\n", COLOR_CYAN);
    } 
    else if (local_str_equal(internal_cmd_buffer, "beep")) {
        print_color("Beeping...\n", COLOR_CYAN);
        beep(1000, 3);
    } 
    else if (local_str_equal(internal_cmd_buffer, "reboot")) {
        sys_reboot();
    } 
    else if (local_str_equal(internal_cmd_buffer, "shutdown")) {
        sys_shutdown();
    } 
    else if (local_str_equal(internal_cmd_buffer, "bios-info")) {
        print_color("--- BIOS Diagnostics ---\n", COLOR_CYAN);
        if (BiosController::IsBiosBatteryLow()) {
            print_color(" Battery Status: [ LOW ]\n", COLOR_LIGHT_RED);
        } else {
            print_color(" Battery Status: [ OK ]\n", COLOR_LIGHT_GREEN);
        }
    } 
    else if (cmd_index > 0) {
        print_color("Unknown command: ", COLOR_LIGHT_RED);
        print_string(internal_cmd_buffer);
        print_char('\n');
    }

    // Чистим внутренний буфер
    for (int i = 0; i < 64; i++) {
        internal_cmd_buffer[i] = '\0';
    }
    cmd_index = 0;
    print_prompt();
}

void kernel_main(void) {
    // Полностью заливаем экран черным перед выводом TUI
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = 0x00;
    }

    // Отрисовка TUI интерфейса
    WindowManager::DrawRect(0, 0, 80, 1, 0x1F); 
    WindowManager::DrawText(1, 0, "OS: BitOS", 0x1F);
    WindowManager::DrawText(12, 0, "| Cmd:", 0x1F);
    WindowManager::DrawText(20, 0, "none", 0x1F); 
    WindowManager::DrawText(65, 0, "Core: C++", 0x1E); 

    WindowManager::DrawWindow(20, 2, 40, 3, " Welcome to BitOS ", 0x4F, 0x4E); 
    WindowManager::DrawText(22, 3, "TUI Interface Initialized!", 0x4F);

    // Первичная очистка локального буфера
    for (int i = 0; i < 64; i++) {
        internal_cmd_buffer[i] = '\0';
    }

    init_keyboard();
    init_mouse();

    cursor_x = 0;
    cursor_y = 6;
    print_prompt();

    while (1) {
        char key = keyboard_read_char();
        if (key != 0) {
            if (key == '\n') {
                internal_cmd_buffer[cmd_index] = '\0';
                execute_command();
            } else if (key == '\b') {
                if (cmd_index > 0) {
                    cmd_index--;
                    internal_cmd_buffer[cmd_index] = '\0';
                    print_char('\b');
                }
            } else if (cmd_index < 60) {
                internal_cmd_buffer[cmd_index] = key;
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
