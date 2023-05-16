import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID,\
    CONF_BATTERY_LEVEL, CONF_BATTERY_VOLTAGE, CONF_VOLTAGE, CONF_CURRENT, CONF_BRIGHTNESS,\
    CONF_TEMPERATURE, UNIT_PERCENT, UNIT_VOLT, UNIT_AMPERE, UNIT_CELSIUS, ICON_BATTERY, ICON_CURRENT_AC, ICON_THERMOMETER, CONF_MODEL, CONF_MAX_CURRENT

DEPENDENCIES = ['i2c']
CONF_BATTERY_CURRENT = "battery_current"
CONF_VIN_CURRENT = "vin_current"

axp192_ns = cg.esphome_ns.namespace('axp192')
AXP192Component = axp192_ns.class_('AXP192Component', cg.PollingComponent, i2c.I2CDevice)
AXP192Model = axp192_ns.enum("AXP192Model")
AXP192ChargeCurrent = axp192_ns.enum("AXP192ChargeCurrent")

MODELS = {
    "M5CORE2": AXP192Model.AXP192_M5CORE2,
    "M5STICKC": AXP192Model.AXP192_M5STICKC,
    "M5TOUGH": AXP192Model.AXP192_M5TOUGH,
    "TTGO_TCALL": AXP192Model.AXP192_TTGO_TCALL,
}
CHARGE_CURRENTS = {
    "100MA": AXP192ChargeCurrent.CURRENT_100MA,
    "190MA": AXP192ChargeCurrent.CURRENT_190MA,
    "280MA": AXP192ChargeCurrent.CURRENT_280MA,
    "360MA": AXP192ChargeCurrent.CURRENT_360MA,
    "450MA": AXP192ChargeCurrent.CURRENT_450MA,
    "550MA": AXP192ChargeCurrent.CURRENT_550MA,
    "630MA": AXP192ChargeCurrent.CURRENT_630MA,
    "700MA": AXP192ChargeCurrent.CURRENT_700MA,
}

AXP192_MODEL = cv.enum(MODELS, upper=True, space="_")
AXP192_CHARGE_CURRENT = cv.enum(CHARGE_CURRENTS, upper=True, space="")

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(AXP192Component),
    cv.Required(CONF_MODEL): AXP192_MODEL,
    cv.Optional(CONF_MAX_CURRENT): AXP192_CHARGE_CURRENT,
    cv.Optional(CONF_BATTERY_LEVEL):
        sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=1,
            icon=ICON_BATTERY,
        ),
    cv.Optional(CONF_BATTERY_VOLTAGE):
        sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            icon=ICON_BATTERY,
        ),
    cv.Optional(CONF_BATTERY_CURRENT):
        sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            icon=ICON_BATTERY,
        ),
    cv.Optional(CONF_VOLTAGE):
        sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            icon=ICON_CURRENT_AC,
        ),
    cv.Optional(CONF_CURRENT):
        sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            icon=ICON_CURRENT_AC,
        ),
    cv.Optional(CONF_VIN_CURRENT):
        sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            icon=ICON_CURRENT_AC,
        ),
    cv.Optional(CONF_TEMPERATURE):
        sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            icon=ICON_THERMOMETER,
        ),
    cv.Optional(CONF_BRIGHTNESS, default=1.0): cv.percentage,
}).extend(cv.polling_component_schema('60s')).extend(i2c.i2c_device_schema(0x77))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)

    cg.add(var.set_model(config[CONF_MODEL]))

    if CONF_MAX_CURRENT in config:
        cg.add(var.set_charge_current(config[CONF_MAX_CURRENT]))

    if CONF_BATTERY_LEVEL in config:
        conf = config[CONF_BATTERY_LEVEL]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_batterylevel_sensor(sens))

    if CONF_BATTERY_VOLTAGE in config:
        conf = config[CONF_BATTERY_VOLTAGE]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_batteryvoltage_sensor(sens))

    if CONF_BATTERY_CURRENT in config:
        conf = config[CONF_BATTERY_CURRENT]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_batterycurrent_sensor(sens))

    if CONF_CURRENT in config:
        conf = config[CONF_CURRENT]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_vbuscurrent_sensor(sens))

    if CONF_VOLTAGE in config:
        conf = config[CONF_VOLTAGE]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_vbusvoltage_sensor(sens))

    if CONF_VIN_CURRENT in config:
        conf = config[CONF_VIN_CURRENT]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_vincurrent_sensor(sens))

    if CONF_TEMPERATURE in config:
        conf = config[CONF_TEMPERATURE]
        sens = yield sensor.new_sensor(conf)
        cg.add(var.set_temperature_sensor(sens))

    if CONF_BRIGHTNESS in config:
        conf = config[CONF_BRIGHTNESS]
        cg.add(var.set_brightness(conf))
