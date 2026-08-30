start:
    cli             ; Отключаем прерывания на время настройки стека
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 7C00h   ; Настраиваем стек
    sti             ; Включаем прерывания обратно

    mov si, msg     ; Загружаем в SI указатель на нашу строку
    call print_string

hang:
    jmp hang        ; Бесконечный цикл

; --- Функция вывода строки ---
print_string:
    lodsb           ; Берем байт из [SI] в AL и инкрементируем SI
    or al, al       ; Проверяем, не ноль ли это (конец строки)
    jz .done        ; Если ноль — выходим
    mov ah, 0x0E    ; Функция BIOS для вывода символа в телексировании
    int 0x10        ; Вызов видеопрерывания
    jmp print_string
.done:
    ret

msg db 'BitOS Loaded successfully!', 0xD, 0xA, 0

times 510-($-$$) db 0 ; Забиваем нулями до 512 байт
dw 0xAA55             ; Сигнатура загрузчика
