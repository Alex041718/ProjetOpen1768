SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;C:\Users\alex\AppData\Local\Microsoft\WindowsApps;C:\Users\alex\AppData\Local\Programs\Microsoft VS Code\bin;C:\Users\alex\AppData\Local\Programs\Microsoft VS Code Insiders\bin
SET CPU_TYPE=LPC1768
SET CPU_VENDOR=NXP
SET UV2_TARGET=Open1786
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\output\startup_lpc17xx._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\system_lpc17xx.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\core_cm3.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\lpc17xx_clkpwr.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\lpc17xx_gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\lpc17xx_pinsel.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\lpc17xx_timer.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\lpc17xx_exti.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\lpc17xx_nvic.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\lpc17xx_ssp.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\lpc17xx_i2c.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\ili_lcd_general.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\lcd_api.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\english_16x8.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\affichagelcd.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\touch_panel.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\memoire.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\timer.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Output\Project.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\Output\Project.axf" --i32combined --output ".\Output\Project.hex"
