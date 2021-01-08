# Arduino script
## Getting started

### Hardware

Component list
- ESP32 (any breakout board with USB programmer. We use ESP32 DOIT DevKit v1)
- MPU-6050 (any breakout board)
- Breadboard, jumper wires, microUSB cable

Connection
MPU-6050 pin | ESP32 pin
-------------|----------
VCC          | 3.3V (connect to +ve rail of breadboard)
GND          | GND (connect to -ve rail of breadboard)
SCL          | D22 / GPIO22 (SCL pin)
SDA          | D21 / GIPO21 (SDA pin)
INT          | D18 / GPIO18
XCL, XDA, AD0| not used


### Software
0. Might need to install USB driver. For the board we use (ESP32 DOIT DevKit v1), the driver can be found [here](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
1. Install PlatformIO [https://platformio.org/](https://platformio.org/) for VS Code
2. In PlatformIO Home, Import existing project, choose the `arduino` folder of this repo
3. Go to `src\main.cpp`
- Change `ssid` and `password` to your wifi password
- Change `server_ip` and `port` to your server
4. Connect USB cable to ESP32
5. Upload the code