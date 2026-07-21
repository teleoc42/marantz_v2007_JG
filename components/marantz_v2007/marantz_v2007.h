#pragma once

#include <string>

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace marantz_v2007 {

class MarantzV2007 : public Component, public uart::UARTDevice {
 public:
  MarantzV2007() = default;

  void setup() override;
  void loop() override;
  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  void send_command_(const std::string &cmd);
  void process_line_(const std::string &line);

  std::string rx_buffer_;

  uint32_t last_poll_{0};

  bool connected_{false};
};

}  // namespace marantz_v2007
}  // namespace esphome