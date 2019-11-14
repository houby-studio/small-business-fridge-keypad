# Small business fridge IoT keypad
## Arduino C code

This repo contains code for ESP-12E chip. This chip is connected to keypad and small LED display for customer interactions and it is connected via WiFi to communicate via API with our Small business fridge e-shop, which allows customers to order any product directly next to our fridge.

## Prerequisites

### Hardware

*You can replace any piece of hardware with something similar, but then code changes may be necessary.*

ESP8266 NodeMCU V1.0 ESP-12E WiFI Module  
https://protosupplies.com/product/esp8266-nodemcu-v1-0-esp-12e-wifi-module/

IIC I2C OLED display 0,96" 128x64  
https://arduino-shop.cz/arduino/1569-iic-i2c-oled-display-0-96-128x64-bily.html

Adafruit matrix keypad  
https://www.adafruit.com/product/3845

### Software

We are using **Arduino IDE** for simplicity, but you may want to use VSCode or other IDE, which may offer more features.

1. Subscribe to ESP8266 url for board library downloads.  
http://arduino.esp8266.com/stable/package_esp8266com_index.json
2. Then open board manager and install **esp8266 by ESP8266 Community version 2.61 or later**
3. Choose board type **NodeMCU 1.0 (ESP-12E Module)** (or different if you own different board)
4. Install following libraries:  
  a) Adafruit GFX Library by Adafruit version 1.6.1  
  b) ArduinoJson by Benoit Blanchon version 6.10.0  
  c) ESP8266 and ESP32 OLED driver for SSD1306 displays by ThingPulse, Fabrice Weinberg version 4.1.0  
  d) Keypad by Mark Stanley, Alexander Brevig version 3.1.1
  
*Note: Library versions mentioned above are latest versions at the time of writing and developing this software. Using newer versions may be prefered, but there may be breaking changed.*
