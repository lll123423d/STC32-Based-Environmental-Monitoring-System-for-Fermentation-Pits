SET C251INC=C:\Keil_v5\C251\Inc;C:\Keil_v5\C251\Inc\STC
SET C251LIB=C:\Keil_v5\C251\Lib
SET CPU_TYPE=STC32G12K128 Series
SET CPU_VENDOR=STC
SET UV2_TARGET=Target 1
SET CPU_XTAL=0x02160EC0
"C:\Keil_v5\C251\BIN\C251.EXE" @".\Objects\main.__i"
"C:\Keil_v5\C251\BIN\C251.EXE" @".\Objects\SPI.__i"
"C:\Keil_v5\C251\BIN\C251.EXE" @".\Objects\OLED.__i"
"C:\Keil_v5\C251\BIN\L251.EXE" @".\Objects\project.lnp"
"C:\Keil_v5\C251\BIN\OH251.EXE" ".\Objects\project" H386 RANGE (0 - 0xFFFFFF)
