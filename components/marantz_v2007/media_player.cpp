#include "media_player.h"
#include "marantz_v2007.h"

namespace esphome {
namespace marantz_v2007 {

using namespace media_player;

MediaPlayerTraits MarantzMediaPlayer::get_traits() {
  MediaPlayerTraits traits;

  // On retire ce qu'on ne sait pas piloter pour l'instant :
  // - PLAY_MEDIA / BROWSE_MEDIA / STOP : pas pertinent pour un ampli
  // - VOLUME_SET : on ne connait pas encore la plage de dB exacte
  //   de l'ampli, donc pas de slider volume pour l'instant (voir
  //   README.md pour comment la déterminer et l'activer ensuite).
  traits.clear_feature_flags(MediaPlayerEntityFeature::PLAY_MEDIA |
                              MediaPlayerEntityFeature::BROWSE_MEDIA |
                              MediaPlayerEntityFeature::STOP |
                              MediaPlayerEntityFeature::VOLUME_SET);

  // Ce qu'on sait piloter :
  traits.add_feature_flags(MediaPlayerEntityFeature::TURN_ON |
                            MediaPlayerEntityFeature::TURN_OFF |
                            MediaPlayerEntityFeature::VOLUME_STEP);

  return traits;
}

bool MarantzMediaPlayer::is_muted() const {
  return parent_ != nullptr && parent_->is_muted();
}

void MarantzMediaPlayer::control(const MediaPlayerCall &call) {
  if (parent_ == nullptr)
    return;

  if (call.get_command().has_value()) {
    switch (*call.get_command()) {
      case MediaPlayerCommand::MEDIA_PLAYER_COMMAND_TURN_ON:
        parent_->power_on();
        break;

      case MediaPlayerCommand::MEDIA_PLAYER_COMMAND_TURN_OFF:
        parent_->power_off();
        break;

      case MediaPlayerCommand::MEDIA_PLAYER_COMMAND_TOGGLE:
        parent_->toggle_power();
        break;

      case MediaPlayerCommand::MEDIA_PLAYER_COMMAND_MUTE:
        parent_->set_mute(true);
        break;

      case MediaPlayerCommand::MEDIA_PLAYER_COMMAND_UNMUTE:
        parent_->set_mute(false);
        break;

      case MediaPlayerCommand::MEDIA_PLAYER_COMMAND_VOLUME_UP:
        parent_->volume_up();
        break;

      case MediaPlayerCommand::MEDIA_PLAYER_COMMAND_VOLUME_DOWN:
        parent_->volume_down();
        break;

      default:
        break;
    }
  }
}

void MarantzMediaPlayer::publish_state_from_parent() {
  if (parent_ == nullptr)
    return;

  this->state = parent_->is_power_on() ? MediaPlayerState::MEDIA_PLAYER_STATE_ON
                                        : MediaPlayerState::MEDIA_PLAYER_STATE_OFF;

  this->publish_state();
}

}  // namespace marantz_v2007
}  // namespace esphome
