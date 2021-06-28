# Obstacle Detection System

## Motivation

As an essential part of smart cities, road-side devices are

## Methods

The

### Hardware

* STM32 (B-L475E-IOT01A)
    * Laser ToF and gesture detection system (VL53L0X, onboard)
    * Ultrasonic ranging module (HC-SR04)
* PiRacer AI Kit
    * Raspberry Pi 4 Model B

### Software

## Results

### Usage

#### STM32 Settings

1. For proper EventQueue handling, go to `mbed-os/platform/mbed-lib.json` and change the value of `callback-nontrivial` to `false`.

2. (Optional) To make console print out floating point numbers, go to `mbed-os/targets/targets.json` and change the value of `printf_lib` to `std`.

3. Edit `mbed_app.json` to include the correct SSID and password.

4. Set `SocketAddress addr("IP",port)` in `main.cpp`.

#### Raspberry Pi Settings

1. Setup Wi-Fi connection. The username of Raspberry Pi is `pi`, and the password is `eslab305`.

2. `cd mycar`.

3. Set `HOST` and `PORT` in `server.py`.

4. Run `server.py` using command `python server.py`.

### Demo Videos

Please refer to 

## Discussion

* Ultrasonic

* Laser

## Future Work

* Other sensing method

* Determine whether car passed obstacle detection area or not

* Allow multiple cars to be on road

* Obstacle information receiving mode

## References