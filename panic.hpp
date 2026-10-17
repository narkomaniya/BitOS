#ifndef PANIC_HPP
#define PANIC_HPP

#include "vga.h"
#include "gui.hpp"

extern "C" {
    #include "sound.h"
}

class PanicManager {
public:
    // Вызывает Синий экран смерти (BSOD) и безопасно останавливает процессор
    static void TriggerBSOD(const char* reason, const char* stop_code) {
        volatile unsigned char* video_mem = (volatile unsigned char*) 0xB8000;

        // 1. Заливаем весь экран синим цветом (0x1F - белый текст на синем фоне)
        for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
            video_mem[i] = ' ';
            video_mem[i + 1] = 0x1F;
        }

        // 2. Рисуем окно Kernel Panic по центру экрана
        WindowManager::DrawWindow(12, 4, 56, 15, " BITOS KERNEL PANIC (BSOD) ", 0x1F, 0x4F);
        WindowManager::DrawText(15, 6, "A fatal error has occurred during execution", 0x1F);
        WindowManager::DrawText(15, 7, "and BitOS has been halted to protect hardware.", 0x1F);

        WindowManager::DrawText(15, 10, "FAULT REASON:", 0x1E);
        WindowManager::DrawText(30, 10, reason, 0x1F);

        WindowManager::DrawText(15, 12, "STOP CODE:", 0x1E);
        WindowManager::DrawText(30, 12, stop_code, 0x1F);

        WindowManager::DrawText(15, 15, "Handler: panic.lua -> PanicManager::TriggerBSOD", 0x1B);
        WindowManager::DrawText(15, 17, "Please restart your computer or virtual machine.", 0x1F);

        // 3. Подаем аварийный звуковой сигнал
        beep(300, 5);

        // 4. Замораживаем систему
        while (1) {
        }
    }
};

#endif
