#ifndef GUI_HPP
#define GUI_HPP

#include "vga.h"

class WindowManager {
private:
    // Изменен тип на unsigned char для правильной арифметики указателей без ошибок
    static volatile unsigned char* video_memory;

public:
    static void DrawRect(int start_x, int start_y, int width, int height, char color) {
        for (int y = start_y; y < start_y + height; y++) {
            for (int x = start_x; x < start_x + width; x++) {
                if (x >= 0 && x < 80 && y >= 0 && y < 25) {
                    int index = (y * 80 + x) * 2;
                    video_memory[index] = ' ';
                    video_memory[index + 1] = (unsigned char)color;
                }
            }
        }
    }

    static void DrawText(int x, int y, const char* text, char color) {
        int index = (y * 80 + x) * 2;
        for (int i = 0; text[i] != '\0'; i++) {
            if (x + i < 80) {
                video_memory[index] = (unsigned char)text[i];
                video_memory[index + 1] = (unsigned char)color;
                index += 2;
            }
        }
    }

    static void DrawWindow(int start_x, int start_y, int width, int height, const char* title, char box_color, char text_color) {
        DrawRect(start_x, start_y, width, height, box_color);

        for (int x = start_x; x < start_x + width; x++) {
            video_memory[(start_y * 80 + x) * 2] = '-';
            video_memory[((start_y + height - 1) * 80 + x) * 2] = '-';
        }
        for (int y = start_y; y < start_y + height; y++) {
            video_memory[(y * 80 + start_x) * 2] = '|';
            video_memory[(y * 80 + start_x + width - 1) * 2] = '|';
        }

        int title_len = 0;
        while (title[title_len] != '\0') title_len++;
        int title_x = start_x + (width - title_len) / 2;
        DrawText(title_x, start_y, title, text_color);
    }

    static void UpdateStatusBar(int x, int y, const char* text, char color, int max_len) {
        for (int i = 0; i < max_len; i++) {
            int index = (y * 80 + (x + i)) * 2;
            video_memory[index] = ' ';
            video_memory[index + 1] = (unsigned char)color;
        }
        DrawText(x, y, text, color);
    }
};

// Абсолютно корректная инициализация статического указателя под x86 видеорежим
volatile unsigned char* WindowManager::video_memory = (volatile unsigned char*)0xB8000;

#endif
