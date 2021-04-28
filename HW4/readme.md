# HW4 #

Let BLE central read button status, control LED, and get student ID from BLE peripheral (STM32).

## Usage ##

1. Run the program on STM32.

2. Use BLE scanner or python program (ble_scan_connect.py) to test.

## Custom Services ##

1. Button Service (UUID: 0xA000): read, notify

2. LED Service (UUID: 0xA002): read, write

3. Student ID Service (UUID: 0xA004): read