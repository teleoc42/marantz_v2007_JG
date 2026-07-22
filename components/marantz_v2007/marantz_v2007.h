#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace marantz_v2007 {

class MarantzV2007 : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;
  bool is_power_on() const { return power_; }
  int get_volume() const { return volume_; }
  bool is_muted() const { return mute_; }
  const std::string &get_source() const { return source_; }

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
  std::string source_;
};

}  // namespace marantz_v2007
}  // namespace esphome
