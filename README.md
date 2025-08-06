# Digital Clock with Alarm (LPC2148 + Proteus)

This project implements a **Digital Clock with Alarm** using the ARM LPC2148 microcontroller. The clock features time and alarm setting capabilities, coded in assembly using IAR Embedded Workbench. The circuit is designed in Proteus, and the LCD displays the current time-date-month-year, and alarm status.

## Features

- Real-time clock display on LCD
- Set and change time using push-button switches
- Alarm mode for setting alarm time
- Buzzer output for alarm notification
- Circuit design provided in Proteus
- Assembly code for LPC2148 (IAR)
- C source code (`main.c`, `lcd.c`) included for feature extension

## Setup Instructions

1. **Open Circuit in Proteus**
   - Open the `clocker.pdsprj` file in Proteus.
   - If the file does not open, manually recreate the circuit as shown in the schematic image provided.

2. **Microcontroller Programming**
   - Right-click on the LPC2148 microcontroller in Proteus.
   - Add the `clk.hex` file (generated and packaged using IAR Embedded Workbench).

3. **LCD Display**
   - The LCD (LM016L) will show the current time.
   - Use the first four switches (`SET`, `INC`, `DEC`, `SAVE`) to change and set the time.
   - Use the last switch (`MODE`) to toggle between clock and alarm modes. In alarm mode, set the alarm time.

4. **Extending Features**
   - C source files (`main.c`, `lcd.c`) are included for users who want to add more features.
   - You can use any IDE to compile/package the C code and generate a HEX file for use in Proteus.

## Components Used (as shown in the schematic)

- **LPC2148 ARM Microcontroller**
- **LM016L LCD Display**
- **Push-button Switches** (SET, INC, DEC, SAVE, MODE)
- **Buzzer** (with NPN transistor driver)
- **LED (Red)**
- **Crystal Oscillator** (X1, X2) with capacitors for RTC
- **Resistors** (R1–R8 for pull-ups, current limiting, and biasing)
- **Capacitors** (C1–C5 for filtering and oscillator stability)
- **NPN Transistor** (Q1, e.g., 2N2222 for buzzer drive)

Refer to the schematic image for exact wiring and connections.
![photo_2025-06-13_11-09-35](https://github.com/user-attachments/assets/463b3ba6-5ee5-4185-93ba-d9cf40bfa280)

## Notes

- The project is coded in assembly for IAR, but C code is provided for further development.
- All necessary HEX and source files are included.
- For any issues with Proteus file compatibility, manually build the circuit as per the schematic.

---
**Enjoy building and customizing your Digital Clock with Alarm!**
