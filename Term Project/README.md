# HW3 #

Send sensor data via socket and visualize sensor data

## Mbed OS Settings ##

1. ```platform/mbed-lib.json```: ```callback-nontrivial = false```.
2. ```targets/targets.json```: ```printf_lib = std```.

## Usage ##

1. Edit ```mbed_app.json``` to include the correct SSID and password.

2. Set ```SocketAddress addr("IP",port)``` in ```main.cpp```.

3. Set ```HOST``` and ```PORT``` in ```server.py```.

4. Run ```server.py```.