import os

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_PAGES,
)
from esphome.const import (
    __version__ as ESPHOME_VERSION,
)

DEPENDENCIES = ["esp32"]

CONF_GREYSCALE = "greyscale"

ed047tc1_ns = cg.esphome_ns.namespace("ed047tc1")
ED047TC1 = ed047tc1_ns.class_("ED047TC1", display.DisplayBuffer)

CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ED047TC1),
            cv.Optional(CONF_GREYSCALE, default=True): cv.boolean,
        }
    ).extend(cv.polling_component_schema("5s")),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
)


async def to_code(config):
    cg.add_platformio_option("lib_compat_mode", "off")
    cg.add_library(
        "LilyGoEPD47",
        repository="https://github.com/Xinyuan-LilyGO/LilyGo-EPD47",
        version="3607e73ba8b84f10e1595511894df52fb008b82a",
    )

    script_path = os.path.join(os.path.dirname(__file__), "patch_lilygo.py")
    cg.add_platformio_option("extra_scripts", [f"pre:{script_path}"])

    cg.add_build_flag("-include esp_idf_version.h")

    var = cg.new_Pvariable(config[CONF_ID])

    if cv.Version.parse(ESPHOME_VERSION) < cv.Version.parse("2023.12.0"):
        await cg.register_component(var, config)
    await display.register_display(var, config)

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    cg.add(var.set_greyscale(config[CONF_GREYSCALE]))
