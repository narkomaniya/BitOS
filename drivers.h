#ifndef DRIVERS_H
#define DRIVERS_H

extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char data);

void init_keyboard(void);
void init_mouse(void);
char keyboard_read_char(void);

void sys_reboot(void);
void sys_shutdown(void);

#endif
