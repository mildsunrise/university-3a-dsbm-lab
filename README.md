# DSBM project

Code produced for DSBM lab, QT2018.

Includes Netbeans project files for convenience.

## Building / flashing

Install toolchain, OpenOCD and optionally debugger:

~~~ bash
sudo apt install gcc-arm-none-eabi openocd gdb-arm-none-eabi
~~~

Download and unpack ChibiOS 2.6.8:

~~~ bash
wget https://downloads.sourceforge.net/project/chibios/ChibiOS%20GPL3/Version%202.6.8/ChibiOS_2.6.8.zip
unzip ChibiOS_2.6.8.zip
~~~

Now, clone the repo and compile!

~~~
make CHIBIOS="/path/to/ChibiOS_2.6.8" REBUILD=yes RAM_COMPILE=no
~~~

Flash `build/ch.elf` to your board:

~~~
openocd -f /usr/share/openocd/scripts/board/stm32f4discovery.cfg -c 'program build/ch.elf verify reset init'
~~~

**Note:** If you have the newer board model (STM32F407G-DISC1), the above
config probably won't detect your board since it has a newer STLink 2.1.
You need to edit it and replace the fourth line with
`source [find interface/stlink-v2-1.cfg]`.
