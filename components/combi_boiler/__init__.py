import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import api

CODEOWNERS = ["@esphome"]
DEPENDENCIES = ["sensor", "api"]
AUTO_LOAD = ["sensor", "api"]

combi_boiler_ns = cg.esphome_ns.namespace("combi_boiler")
CombiBoiler = combi_boiler_ns.class_("CombiBoiler", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CombiBoiler),
    cv.GenerateID("api"): cv.use_id(api.APIServer),  # Changed to use string "api"
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    # Get API reference using the string ID
    # api_var = await cg.get_variable(config["api"])
    # cg.add(var.set_api(api_var))
