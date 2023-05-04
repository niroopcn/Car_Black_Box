# Car_Black_Box

PIC18F4580 Microcontroller on rhydoLABZ Board.

Protocols Utilized: UART & I2C.
Peripherals Used: CLCD(hd44780), Matrix Keypad, Potentiometer, RTC(ds1307) & External EEPROM(24c02).

Black Boxes are typically used in any transportation system (ex: Airplanes) that are used for analysis post-crash and understand the root cause of accidents. Continuous monitoring and logging of events (ex: over-speeding) is critical for effective usage of black box. The goal of this project is to implement core functionalities of a car black-box in a PIC based micro-controller. Events will be logged in EEPROM in this project.

The data can only be accessed using a password which is stored in EEPROM.

Features:
1) Display Time using RTC
2) Simulate Speed using Potentiometer
3) Simulate Gear changes using Matrix Keypad
4) Log Key press to External EEPROM using I2C Protocol
5) View Logs on CLCD
6) Download Logs to Host Console using UART
7) Clear Stored Logs
8) Change password and store it in EEPROM
9) Change time in RTC by using I2C

![IMG_20230504_163700](https://user-images.githubusercontent.com/70747194/236259327-8afbc2b4-8e40-48bd-a3e2-5896c4eec835.jpg)

![IMG_20230504_163831](https://user-images.githubusercontent.com/70747194/236259491-a7295571-b0b5-4c84-b1de-feaec7800660.jpg)

![IMG_20230504_163839](https://user-images.githubusercontent.com/70747194/236259519-cccca234-abdb-4dcb-a426-14047a79fd53.jpg)
