# Константы для Multiboot
.set ALIGN,    1<<0             # Выравнивать модули на границах страниц
.set MEMINFO,  1<<1             # Предоставить карту памяти
.set FLAGS,    ALIGN | MEMINFO  # Флаги multiboot
.set MAGIC,    0x1BADB002       # Магическое число для GRUB
.set CHECKSUM, -(MAGIC + FLAGS) # Контрольная сумма

# Секция заголовка Multiboot (GRUB прочитает её в первые 8 КБ файла)
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Выделяем 16 Килобайт под стек для Си-кода
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 КиБ
stack_top:

# Точка старта системы
.section .text
.global _start
.type _start, @function
_start:
    # Настраиваем указатель стека
    mov $stack_top, %esp

    # Вызываем главную функцию на Си
    extern kernel_main
    call kernel_main

    # Если Си-код завершится, уходим в вечный цикл ожидания прерываний
_halt:
    cli
    hlt
    jmp _halt

.global inb
inb:
    mov 4(%esp), %dx
    in %dx, %al
    ret

.global outb
outb:
    mov 8(%esp), %al
    mov 4(%esp), %dx
    out %al, %dx
    ret
