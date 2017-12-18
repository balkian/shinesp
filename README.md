# RGB strip WiFi server

This server has been tested with:
* ws2812b LED strip
* WeMos ESP8266 board


# Instructions

The first time it boots, the board will create its own access point (`led`).
To configure the board, just connect to that access point and access the board either using its IP (it is the default gateway for the network) or its MDNS name (`esp8266.local`):

```
curl http://esp8266.local/credentials?ssid=mywifi&pass=mypassword
```

The board will then reboot and try to log in with the given credentials.
If the credentials were correct, the `led` network should have disappeared.
If it hasn't, try rebooting the board or debugging.

Once the board is connected to your router, you can start using the API of the board.
To get the IP of the board you have several options:

* Using the MDNS address: `esp8266.local`
* Get the IP from your router's configuration page
* The board prints the IP address it was given to the serial monitor
* Use nmap/zenmap/similar


# WEB UI

There is a simple HTML website that can be used to test the API and set the colors of the LED.
From the root of the repository, open it with your favourite browser, or with python:

```
firefox $PWD/index.html

# OR

python3 -m http.server
```


# API

* `/clear` clear WIFI credentials
* `/credentials?ssid=<SSID>&pass=<PASSWD>` Set the
* `/color?r=<R>&g=<G>&b=<B>` Set 
* `/off` turn off the lights
* `/on` turn on the lights (using the last color and value)
* `/toggle` turn on the lights if they are off, and vice versa
* `/white` set the color to white
* `/brightness?value=<VALUE>` set the brightness value (0-255)
* `/brightness/up` turn up the brightness (up to 255)
* `/brightness/down` turn down the brightness (down to 0)


# Developing

* Install arduino

* Install support for ESP8266 boards in arduino: https://github.com/esp8266/Arduino

* Install the fastled library in arduino: https://github.com/FastLED/FastLED/

* Profit

# Debugging
You can use a serial monitor (e.g. in the Arduino IDE) to connect to the board.

# Over-the-air updates

You can connect to your board wirelessly and update its firmware using OTA updates.
To do so, follow this instructions: http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html
