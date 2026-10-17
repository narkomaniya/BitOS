#ifndef DRIVERS_H
#define DRIVERS_H

#ifdef __cplusplus
extern "C" {
#endif

// Низкоуровневый ввод-вывод портов x86
extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char data);

// Драйверы клавиатуры и мыши
void init_keyboard(void);
void init_keyboard_driver(void); // Наша новая функция с прерываниями
void init_mouse(void);
char keyboard_read_char(void);

// Управление питанием ПК
void sys_reboot(void);
void sys_shutdown(void);

// Характеристики железа (из модуля hardware.c)
unsigned long long get_cpu_cycles(void);
unsigned int get_vga_memory_size(void);

#ifdef __cplusplus
}
#endif

#endif
