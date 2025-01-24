# sensor.py
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_POWER_FACTOR,
    STATE_CLASS_MEASUREMENT,
    UNIT_PERCENT,
)
from . import CombiBoiler, combi_boiler_ns

CONF_COMBI_BOILER_ID = "combi_boiler_id"
CONF_SENSOR_NUMBER = "sensor_number"
CONF_TEMPERATURE_ID = "temperature_id"

CONFIG_SCHEMA = sensor.sensor_schema(
    device_class=DEVICE_CLASS_POWER_FACTOR,
    state_class=STATE_CLASS_MEASUREMENT,
    unit_of_measurement=UNIT_PERCENT,
).extend({
    cv.GenerateID(CONF_COMBI_BOILER_ID): cv.use_id(CombiBoiler),
    cv.Required(CONF_SENSOR_NUMBER): cv.one_of(1, 2),
    cv.Required(CONF_TEMPERATURE_ID): cv.use_id(sensor.Sensor),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_COMBI_BOILER_ID])
    var = await sensor.new_sensor(config)
    temp_var = await cg.get_variable(config[CONF_TEMPERATURE_ID])
    
    if config[CONF_SENSOR_NUMBER] == 1:
        cg.add(parent.set_power1(var))
        cg.add(parent.set_temperature1(temp_var))
    else:
        cg.add(parent.set_power2(var))
        cg.add(parent.set_temperature2(temp_var))
