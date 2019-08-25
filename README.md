# Space Within Spaces Firmware

A Space Within Spaces is an art installation by Joseph Morris in the Juliana
Curran Terian Design Center Atrium at Pratt. The installation consists of an
array of 180 LED light bulbs hanging in the space above the atrium. This
project contains the firmware for the bulbs, which are based on the ESP8266 and
are controlled over WiFi using UDP broadcast packets.

## Network Configuration

**network_config.h** should be updated with the network details for the LED
grid:

```
#ifndef H_NETWORK_CONFIG
#define H_NETWORK_CONFIG

const char* SSID_NAME = "my cool network";
const char* SSID_PWD = "super secret";

const char* HOSTNAME = "esp";

#endif
```

## Configuring Bulbs

The bulbs need to know where they are in the array in order to decode the
UDP broadcast packets correctly. This information is stored in a lookup table
in **led_firmware/offsets.h**. *ESP_ADDRESS_OFFSETS* contains the chip IDs for
the bulbs in the order that they are installed on the strands (a zig zag).

