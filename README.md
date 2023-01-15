# Pionizer AXP192 Component

AXP192 power management library for ESPHome adopted for a custom ESPHome library for AI and related edge devices.

## Installation

Copy the components to a custom_components directory next to your .yaml configuration file, or include directly from this repository.

## Configuration

Sample configurations are found in the `/sample-config` folder.

This component adds a new model configuration to the AXP192 sensor which determines which registers are needed for each device. Available models are `model: m5core2`, `model: m5stickc` and `model: m5tough`.

### Include axp192 component

```yaml
external_components:
  - source: github://pionizer/pionizer-axp192
    components: [axp192]
```

### M5Stick-C

```yaml
sensor:
  - platform: axp192
    model: m5stickc
    id: power_mgmt
    address: 0x34
    i2c_id: i2c_bus
    update_interval: 60s
    brightness: 100%
    battery_level:
      name: ${device} Battery Level
      id: batterylevel
```

### M5Stack Core2

```yaml
sensor:
  - platform: axp192
    model: m5core2
    id: power_mgmt
    address: 0x34
    i2c_id: i2c_bus
    update_interval: 60s
    brightness: 100%
    battery_level:
      name: ${device} Battery Level
      id: batterylevel
```

### M5Tough

```yaml
sensor:
  - platform: axp192
    model: m5tough
    id: power_mgmt
    address: 0x34
    i2c_id: i2c_bus
    update_interval: 60s
    brightness: 100%
    battery_level:
      name: ${device} Battery Level
      id: batterylevel
```

The display component required for the M5Tough is as follows:

```yaml
display:
  - platform: ili9341
    # 320x240
    model: m5tough
    cs_pin: GPIO5
    dc_pin: GPIO15
    lambda: |-
      it.print(160, 0, id(title_font), id(color_white), TextAlign::TOP_CENTER, "Hello World");
```

# Credits and Disclaimers

This library is built on prior work published by @M5Stack as well as individual contributors like @airy10, @apolselli, @abmantis, @geiseri, @martydingo, @gonzalop, @shish, @cmet7, @JensGuckenbiehl, @leoedin, @rolloo, @paulchilton amongst others.

# TODO

- Additional contributions from @JensGuckenbiehl and @rolloo to review and merge into this code base