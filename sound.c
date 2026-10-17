#include "sound.h"
#include "drivers.h"

void play_sound(unsigned int frequency) {
    if (frequency == 0) return;

    unsigned int divisor = 1193180 / frequency;

    // Программируем таймер через функцию outb из нашего boot.s
    outb(0x43, 0xB6);
    outb(0x42, (unsigned char)(divisor & 0xFF));
    outb(0x42, (unsigned char)((divisor >> 8) & 0xFF));

    unsigned char speaker_state = inb(0x61);
    if (speaker_state != (speaker_state | 3)) {
        outb(0x61, speaker_state | 3);
    }
}

void stop_sound(void) {
    unsigned char speaker_state = inb(0x61) & 0xFC;
    outb(0x61, speaker_state);
}

void beep(unsigned int frequency, unsigned int duration) {
    play_sound(frequency);
    
    // Чистая задержка на Си, которая не вызывает ошибок компилятора
    volatile unsigned int timeout = duration * 2000000;
    while (timeout > 0) {
        timeout--;
    }
    
    stop_sound();
}
