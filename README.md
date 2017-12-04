# RGB strip WiFi server

This server has been tested with:
* ws2812b LED strip
* WeMos ESP8266 board


# Instructions

The first time it boots, the board will create its own access point so you can configure the SSID and password.
To configure it, just connect to the `led` network and access the board either using its IP (it is the default gateway for the network) or its MDNS name (`esp8266.local`):

```
curl http://esp8266.local/credentials?ssid=mywifi&pass=mypassword
```

The board will then reboot and try to log in with the given credentials.
If the credentials were correct, the `led` network should have disappeared.
If it hasn't, try rebooting the board or debugging.

Once the board is connected to your router, you can start using the API of the board.

# API

* `/clear` clear WIFI credentials
* `/credentials?ssid=<SSID>&pass=<PASSWD>` Set the
* `/color?r=<R>&g=<G>&b=<B>` Set 
* `/off` turn off the lights
* `/on` turn on the lights (using the last color and value)
* `/toggle` turn the lights if they are off, and vice versa
* `/white` set the color to white
* `/brightness?value=<VALUE>` set the brightness value (0-255)
* `/brightness/up` turn up the brightness (up to 255)
* `/brightness/down` turn down the brightness (down to 255)

# Debugging
You can use a serial monitor (e.g. in the Arduino IDE) to connect to the board.

# Over-the-air updates

You can connect to your board wirelessly and update its firmware using OTA updates.
To do so, follow this instructions: http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html
