import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import media_player
from esphome.const import CONF_ID

from . import marantz_v2007_ns, MarantzV2007

DEPENDENCIES = ["marantz_v2007"]

CONF_MARANTZ_ID = "marantz_id"

MarantzMediaPlayer = marantz_v2007_ns.class_(
    "MarantzMediaPlayer",
    media_player.MediaPlayer,
    cg.Component,
)

CONFIG_SCHEMA = media_player.media_player_schema(
    MarantzMediaPlayer
).extend(
    {
        cv.Required(CONF_MARANTZ_ID): cv.use_id(MarantzV2007),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await media_player.register_media_player(var, config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_MARANTZ_ID])
    cg.add(parent.set_media_player(var))
