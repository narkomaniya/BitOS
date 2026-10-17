#include "string.h"

// Подсчет длины строки
int str_len(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Сравнение двух строк
int str_equal(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) return 0;
        i++;
    }
    return (s1[i] == '\0' && s2[i] == '\0');
}

// Очистка буфера памяти
void mem_clear(char* buffer, int size) {
    for (int i = 0; i < size; i++) {
        buffer[i] = '\0';
    }
}
