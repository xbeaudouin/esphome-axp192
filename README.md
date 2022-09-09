# ESPHome AXP192 Component

This custom component it to implement support for the AXP192 for both the M5Stick-C, and the M5Stack Core2, building on top of airy10's code. 

## Installation

Copy the components to a custom_components directory next to your .yaml configuration file.

## Configuration

Sample configurations can be found within `/sample-config`. Please note that I've not yet been able to correctly configure the M5Stick-C screen just yet, however the AXP192 component does initalise it, and the sample configuration currently demostrates a white screen, when it should present some text. I plan to fix this soon. 

This component adds a new model configuration to the AXP192 sensor, `model: M5CORE2` & `model: M5STICKC`, as so the right pins are initalized and voltages go to the right places:

### M5Stick-C

```yaml
sensor:
  - platform: axp192
    model: M5STICKC
    address: 0x34
    i2c_id: bus_a
    update_interval: 30s
    battery_level:
      name: "M5Stick Battery Level"
      id: "m5stick_batterylevel"
```

### M5Stack Core2

```yaml
sensor:
  - platform: axp192
    model: M5CORE2
    address: 0x34
    i2c_id: bus_a
    update_interval: 30s
    battery_level:
      name: "${upper_devicename} Battery Level"
      id: "${devicename}_batterylevel"
```
