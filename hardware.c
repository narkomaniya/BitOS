#include "drivers.h"

// Объявляем функцию, которую мы только что написали в boot.s
extern unsigned long long get_cpu_cycles(void);

unsigned int get_vga_memory_size(void) {
    return 80 * 25 * 2;
}
