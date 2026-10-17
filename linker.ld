ENTRY(_start)

SECTIONS
/* GRUB обычно загружает ядра по адресу 1 Мегабайт */
. = 1M;

{
    /* Сначала идет заголовок Multiboot, затем код */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    /* Только чтение (константные строки) */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Инициализированные глобальные переменные */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Неинициализированные переменные и стек */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
