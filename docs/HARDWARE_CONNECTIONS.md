\# Hardware Connections — ESP32 Smart Vending Machine



\## 1. Project Overview



The ESP32-Based Smart Vending Machine is designed to dispense two different products using an ESP32 DevKit, a 4×4 keypad, a 16×2 I2C LCD display, and two 28BYJ-48 stepper motors.



The keypad is used for PIN authentication, the LCD displays system messages, and the stepper motors control the dispensing mechanisms.



\## 2. Hardware Components



| Component | Quantity |

|---|---|

| ESP32 DevKit | 1 |

| 4×4 Matrix Keypad | 1 |

| 16×2 I2C LCD Display | 1 |

| 28BYJ-48 Stepper Motor (5V) | 2 |

| ULN2003 Motor Driver | 2 |

| External regulated 5V power supply | 1 |

| Breadboard | 1 |

| Jumper Wires | As required |

| USB Cable | 1 |



\## 3. Keypad Connections



The 4×4 matrix keypad is connected to the ESP32 DevKit for four-digit PIN authentication.



| Keypad Pin | ESP32 DevKit GPIO |

|---|---|

| R1 | GPIO 13 |

| R2 | GPIO 14 |

| R3 | GPIO 15 |

| R4 | GPIO 12 |

| C1 | GPIO 23 |

| C2 | GPIO 33 |

| C3 | GPIO 4 |

| C4 | GPIO 5 |



\*\*Keypad Functions:\*\*



\- Keys 0–9: Enter PIN

\- Key #: Submit PIN

\- Key \*: Clear PIN / Logout



\## 4. I2C LCD Connections



A 16×2 I2C LCD displays authentication messages, product selection instructions, dispensing status, and security lock messages.



| LCD Pin | ESP32 DevKit Connection |

|---|---|

| SDA | GPIO 22 |

| SCL | GPIO 21 |

| VCC | Suitable LCD supply |

| GND | Common GND |



\*\*LCD Configuration:\*\*



\- Display: 16×2

\- I2C Address: 0x27

\- Communication Protocol: I2C



If the LCD module uses 5V I2C pull-ups, an appropriate bidirectional I2C logic-level shifter is required.



\## 5. Stepper Motor 1 Connections



The first 28BYJ-48 stepper motor is connected to the ESP32 through ULN2003 Driver 1.



\*\*Original connections from the DAY 1 document:\*\*



| ULN2003 Driver 1 | ESP32 DevKit GPIO |

|---|---|

| IN1 | GPIO 18 |

| IN2 | GPIO 19 |

| IN3 | GPIO 2 |

| IN4 | GPIO 3 |

| VCC | External 5V |

| GND | Common GND |



\*\*Function:\*\* Controls the dispensing mechanism for Product 1.



\*\*Important — Current Firmware Difference:\*\*



The latest firmware (Version 0.7) uses the following revised connections for Motor 1:



| ULN2003 Driver 1 | ESP32 DevKit GPIO in v0.7 |

|---|---|

| IN1 | GPIO 18 |

| IN2 | GPIO 19 |

| IN3 | GPIO 16 |

| IN4 | GPIO 17 |



The revised v0.7 connections avoid GPIO 2 and GPIO 3, which can interfere with ESP32 startup or UART communication. The actual wiring must match the firmware being uploaded.



\## 6. Stepper Motor 2 Connections



The second 28BYJ-48 stepper motor is connected through ULN2003 Driver 2.



| ULN2003 Driver 2 | ESP32 DevKit GPIO |

|---|---|

| IN1 | GPIO 25 |

| IN2 | GPIO 26 |

| IN3 | GPIO 27 |

| IN4 | GPIO 32 |

| VCC | External 5V |

| GND | Common GND |



\*\*Function:\*\* Controls the dispensing mechanism for Product 2.



\## 7. Power Supply Connections



\- ESP32 is powered through USB during development.

\- Both 28BYJ-48 stepper motors operate using an external regulated 5V power supply.

\- Each stepper motor is connected through its ULN2003 motor driver.

\- The ESP32 and both motor drivers share a common ground.

\- Stepper motors must not be powered directly from ESP32 GPIO pins.

\- The power supply must have sufficient current capacity to operate both motors.



\## 8. Working Principle



1\. ESP32 initializes the keypad, LCD, and stepper motors.

2\. LCD displays the four-digit PIN entry screen.

3\. User enters the PIN through the keypad.

4\. ESP32 checks the entered PIN against the predefined PINs.

5\. After three consecutive incorrect PIN attempts, the system locks for 30 seconds.

6\. After successful authentication, the LCD displays the product selection message.

7\. Pressing 1 activates Stepper Motor 1.

8\. Pressing 2 activates Stepper Motor 2.

9\. The corresponding motor rotates through its programmed dispensing cycle.

10\. LCD displays the dispensing completion message.

11\. The system returns to the PIN entry screen.



\## 9. Current Project Status



\*\*Firmware Version: v0.7\*\*



Completed features:



\- ESP32 initialization

\- Keypad interfacing

\- Four-digit PIN authentication

\- I2C LCD interfacing

\- Stepper Motor 1 control

\- Stepper Motor 2 control

\- Dual-product selection

\- Three-attempt PIN security

\- 30-second security lock

\- Automatic reset after dispensing



\## 10. Future Development



The upcoming development stages include:



\- Python Tkinter GUI

\- ESP32–Python Serial Communication

\- GUI-Based Product Selection

\- Virtual Wallet Management

\- Wallet Recharge System

\- Smart Inventory Management

\- Out-of-Stock Detection

\- Insufficient Balance Detection

\- Purchase History

\- Complete Hardware and GUI Integration



\## 11. Hardware Notes



\- The connection tables are based on the final integrated setup in DAY 1.

\- The current v0.7 firmware has revised Motor 1 GPIO assignments.

\- The exact ESP32 DevKit model and physical wiring should be confirmed.

\- Some ESP32 GPIOs have boot-strapping functions and require care during startup.

\- The stepper motor movement must be calibrated for the final cardboard dispensing mechanism.

\- Completion of a motor cycle does not independently confirm that a product has physically fallen into the collection tray.

\- The completed system will use the Python GUI for product selection, wallet management, and inventory monitoring.





