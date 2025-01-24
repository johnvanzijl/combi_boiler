# text_sensor.py
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    CONF_ID,
)
from . import CombiBoiler, combi_boiler_ns

CONF_COMBI_BOILER_ID = "combi_boiler_id"
CONF_SENSOR_NUMBER = "sensor_number"

CONFIG_SCHEMA = text_sensor.text_sensor_schema().extend({
    cv.GenerateID(CONF_COMBI_BOILER_ID): cv.use_id(CombiBoiler),
    cv.Required(CONF_SENSOR_NUMBER): cv.one_of(1, 2),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_COMBI_BOILER_ID])
    var = await text_sensor.new_text_sensor(config)
    
    if config[CONF_SENSOR_NUMBER] == 1:
        cg.add(parent.set_status1(var))
    else:
        cg.add(parent.set_status2(var))