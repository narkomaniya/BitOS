#ifndef BIOS_HPP
#define BIOS_HPP

// Подключаем низкоуровневые функции Си-драйверов (inb и outb)
extern "C" {
    #include "drivers.h"
}

class BiosController {
private:
    // Внутренние методы для чтения и записи физических регистров CMOS BIOS
    static unsigned char read_register(unsigned char reg) {
        outb(0x70, reg);
        return inb(0x71);
    }

    static void write_register(unsigned char reg, unsigned char value) {
        outb(0x70, reg);
        outb(0x71, value);
    }

public:
    // 1. Получить тип загрузочного диска (Boot Drive Configuration)
    static unsigned char GetBootDriveType() {
        return read_register(0x1D);
    }

    // ИСПРАВЛЕНО: Теперь drive_type корректно передается в CMOS
    static void SetBootDriveType(unsigned char drive_type) {
        write_register(0x1D, drive_type);
    }

    // 2. Проверить заряд батарейки BIOS (микросхемы RTC на материнке)
    static bool IsBiosBatteryLow() {
        // 7-й бит регистра 0x0D показывает статус питания чипа
        unsigned char status = read_register(0x0D);
        return (status & 0x80) == 0; 
    }

    // 3. Сбросить пользовательские настройки BIOS (Clear CMOS)
    static void ResetBiosSettings() {
        // Очищаем основные конфигурационные ячейки памяти BIOS
        for (unsigned char reg = 0x10; reg <= 0x2D; reg++) {
            write_register(reg, 0x00);
        }
    }
};

#endif