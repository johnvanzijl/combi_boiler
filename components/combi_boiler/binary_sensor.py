
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_HEAT,
)
from . import CombiBoiler, combi_boiler_ns

CONF_COMBI_BOILER_ID = "combi_boiler_id"
CONF_SENSOR_NUMBER = "sensor_number"

CombiBoilerBinarySensor = combi_boiler_ns.class_("CombiBoilerBinarySensor", binary_sensor.BinarySensor)

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(
    device_class=DEVICE_CLASS_HEAT,
).extend({
    cv.GenerateID(CONF_COMBI_BOILER_ID): cv.use_id(CombiBoiler),
    cv.Required(CONF_SENSOR_NUMBER): cv.one_of(1, 2),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_COMBI_BOILER_ID])
    var = await binary_sensor.new_binary_sensor(config)

    if config[CONF_SENSOR_NUMBER] == 1:
        cg.add(parent.set_boiler1_full_binary_sensor(var))
    else:
        cg.add(parent.set_boiler2_full_binary_sensor(var))