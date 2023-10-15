# Tutorials on how to use this Ergo Poor Man's Wallet Project

## Hardware needed
Total price when purchasing on Aliexpress approx. 10$ including shipping to Europe.
- STM32F103C8T6 blue pill board (can be bought as cheap as 2$ including shipping to Europe on Aliexpress)
- ST-Link V2 programmer (cost also around 2$ including shipping to Europe on Aliexpress)
- 1.3inch OLED I2C (IIC) monochromatic display (costs around 4$ including shipping on Aliexpress), there are multiple versions with different driver chips, all of them should be compatible with the current implementation of the OLED driver library in this project
- female to female wires (can be bought for 1$ including shipping to Europe on Aliexpress)
- 2x electronic button (any button which can be connected to the wires is ok, 100 pcs of buttons cost 1$ including shipping on Aliexpress)
- microusb or usbc cable (depending on the connector of the bluepill board - there are multiple variants which differ only in the conenctor used on their pcb) (this item is free assuming you have some microusb/usbc cables lying around)

## Hardware connections
Display:
```
Display VCC <--> 3V3
Display SDA <--> PB11
Display SCL <--> PB10
Display GND <--> GND
```
Buttons:
```
LEFT_BUTTON_FIRST_LEG <--> PB0
LEFT_BUTTON_SECOND_LEG <--> GND
RIGHT_BUTTON_FIRST_LEG <--> PB1
RIGHT_BUTTON_SECOND_LEG <--> GND
```

## Software requirements
For the information about required software, please see following directories (this text will be updated in the future)
