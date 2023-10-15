# Ergo Poor Man's Wallet Firmware

This folder contains "Ergo Poor Man's Wallet" firmware for STM32F103C8T6 (or STM32F103CBT6 - with bigger flash size) microcontroller which can be easily purchased in form of userfriendly development board (called "blue pill") for as low as 2$ from china.

Assuming that you have installed required build tools, namely `gcc-arm-none-eabi` toolchain and `st-flash` tool, you can build this firmware and automatically flash it to the blue pill board (via connected st-link v2 programmer board which cost again approx. 2$) by running:
`make all` command.

# Copyright
Since this project was made during 1.5 day of programming at the ErgoHack VII event, this code utilizes a lot of external libraries. Libraries are left with their corresponding copyright, however multiple of them were modified in order to fit this project purpose. For the purpose of the embedded firmware FreeRTOS, libopencm3 and the usbcdc library (for emulating UART over the builtin usb on the blue pill board so the EPMW user's don't have to buy another board for the purpose of UART <-> USB conversion) were used. All of the foreign code (with a few exceptions (Makefile, linker scripts)) is always located within "external_libs" directories.
