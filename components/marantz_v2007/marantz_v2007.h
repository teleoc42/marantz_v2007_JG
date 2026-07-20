#pragma once

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

 protected:
  bool connected_{false};
};

}  // namespace marantz_v2007
}  // namespace esphome