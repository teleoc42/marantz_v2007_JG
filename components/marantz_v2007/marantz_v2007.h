#pragma once

#include <string>

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace marantz_v2007 {

class MarantzMediaPlayer;

class MarantzV2007 : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

  // -------------------------------------------------
  // Lecture de l'état (mis à jour par le polling UART)
  // -------------------------------------------------
  bool is_power_on() const { return power_; }
  int get_volume() const { return volume_; }
  bool is_muted() const { return mute_; }
  bool is_speaker_a_on() const { return speaker_a_; }
  bool is_speaker_b_on() const { return speaker_b_; }

  void set_speaker_a(bool on);
  void set_speaker_b(bool on);

  const std::string &get_source() const { return source_; }

  void set_volume_db(int db);

  const std::string &get_surround_mode() const { return surround_mode_; }
  void select_surround_mode(const std::string &mode_name);

  // -------------------------------------------------
  // Commandes envoyées à l'ampli
  // -------------------------------------------------
  void power_on();
  void power_off();
  void toggle_power();

  void set_mute(bool mute);

  void volume_up();
  void volume_down();
  void volume_up_5();
  void volume_down_5();

  // Sélectionne une source par son nom (ex: "TV", "CD", "DVD"...)
  // Utilise la même table de correspondance que le parsing des réponses.
  void select_source(const std::string &source_name);

  void set_media_player(MarantzMediaPlayer *player);

 protected:
  void send_command_(const std::string &cmd);
  void process_line_(const std::string &line);

  // Buffer de réception UART
  std::string rx_buffer_;

  // Dernier poll
  uint32_t last_poll_{0};
  uint8_t poll_step_{0};

  // État actuel de l'ampli
  bool power_{false};
  int volume_{0};
  bool mute_{false};
  bool speaker_a_{false};
  bool speaker_b_{false};
  std::string source_;
  std::string surround_mode_;

  // Interface Media Player
  MarantzMediaPlayer *media_player_{nullptr};
};

}  // namespace marantz_v2007
}  // namespace esphome
