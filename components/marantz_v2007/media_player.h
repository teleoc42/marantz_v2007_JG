#pragma once

#include "esphome/core/component.h"
#include "esphome/components/media_player/media_player.h"

namespace esphome {
namespace marantz_v2007 {

class MarantzV2007;

class MarantzMediaPlayer : public Component,
                           public media_player::MediaPlayer {
 public:
  MarantzMediaPlayer() = default;

  void set_parent(MarantzV2007 *parent) { parent_ = parent; }

  // Component
  void setup() override {}
  void loop() override {}

  // MediaPlayer
  media_player::MediaPlayerTraits get_traits() override;
  bool is_muted() const override;

  // Synchronise l'état Home Assistant avec le backend UART
  void publish_state_from_parent();

 protected:
  void control(const media_player::MediaPlayerCall &call) override;

  MarantzV2007 *parent_{nullptr};
};

}  // namespace marantz_v2007
}  // namespace esphome
