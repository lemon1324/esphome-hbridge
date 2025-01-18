import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import output
from esphome.const import CONF_ID
from esphome.core import CORE
from esphome import automation

# Define the namespace for your component
hbridge_ns = cg.esphome_ns.namespace("hbridge")
HBridgeComponent = hbridge_ns.class_("HBridgeComponent", cg.Component)
OutputAction = hbridge_ns.class_("OutputAction", automation.Action)

# Configuration keys
CONF_OUTPUT_A = "output_a"
CONF_OUTPUT_B = "output_b"
CONF_OUTPUT_ENABLE = "output_enable"
CONF_OUTPUT = "output"
CONF_OUTPUT_ACTION = "hbridge.set_output"

# Define the schema for the component
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HBridgeComponent),  # Component ID
        cv.Required(CONF_OUTPUT_A): cv.use_id(
            output.FloatOutput
        ),  # Required float output A
        cv.Required(CONF_OUTPUT_B): cv.use_id(
            output.FloatOutput
        ),  # Required float output B
        cv.Optional(CONF_OUTPUT_ENABLE): cv.use_id(
            output.FloatOutput
        ),  # Optional float output for enable
    }
).extend(cv.COMPONENT_SCHEMA)

ACTION_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(HBridgeComponent),
        cv.Required(CONF_OUTPUT): cv.float_,
    }
)


@automation.register_action(CONF_OUTPUT_ACTION, OutputAction, ACTION_SCHEMA)
async def output_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    yield cg.register_parented(var, config[CONF_ID])
    cg.add(var.set_output(config[CONF_OUTPUT]))
    yield var


# Define how to register the component
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])  # Declare the component

    # Configure output_a
    out_a = await cg.get_variable(config[CONF_OUTPUT_A])
    cg.add(var.set_output_a(out_a))

    # Configure output_b
    out_b = await cg.get_variable(config[CONF_OUTPUT_B])
    cg.add(var.set_output_b(out_b))

    # Configure optional output_enable
    if CONF_OUTPUT_ENABLE in config:
        out_enable = await cg.get_variable(config[CONF_OUTPUT_ENABLE])
        cg.add(var.set_output_enable(out_enable))

    # Register the component
    await cg.register_component(var, config)
