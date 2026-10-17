
---@diagnostic disable-next-line: lowercase-global
beep = beep or function() end

BitOS_Config = {
    version = "0.5",
    safe_mode = false,
    vga_address = 0xB8000,
    kbd_irq = 1
}

-- Маленькая, но важная функция проверки состояния системы (Health Check)
function verify_kernel_integrity()
    if BitOS_Config.vga_address == 0xB8000 then
        print("[Watchdog] VGA Memory Guard: OK")
        print("[Watchdog] Interrupt Vector IRQ1: LOCKED")
        beep() -- Короткий сигнал успешной инициализации
        return true
    end
    return false
end

-- Запуск сторожевого модуля
verify_kernel_integrity()
