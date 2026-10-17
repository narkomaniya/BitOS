
---@diagnostic disable: lowercase-global
trigger_bsod = trigger_bsod or function(reason, code) end
beep = beep or function() end

PanicHandler = {
    active = true,
    last_error = "NONE",
    stop_code = "0x00000000"
}

-- Главная функция вызова Синего экрана смерти из Lua
function raise_kernel_panic(reason, code)
    PanicHandler.last_error = reason
    PanicHandler.stop_code = code

    print("====================================================")
    print(" [!] BITOS BLUE SCREEN OF DEATH (BSOD)")
    print(" [!] FATAL ERROR: " .. tostring(reason))
    print(" [!] STOP CODE:   " .. tostring(code))
    print("====================================================")

    -- Вызываем аппаратную отрисовку BSOD из модуля panic.hpp
    trigger_bsod(reason, code)
end

-- Проверка критических параметров при старте ПК
function check_boot_state(bios_battery_low, vga_ok)
    if not vga_ok then
        raise_kernel_panic("VGA_BUFFER_INIT_FAILED", "0x0000001A")
        return false
    end

    if bios_battery_low then
        raise_kernel_panic("BIOS_CMOS_BATTERY_FAILURE", "0x000000B1")
        return false
    end

    return true
end
