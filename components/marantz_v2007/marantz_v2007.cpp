#include "marantz_v2007.h"

#include "esphome/core/application.h"
#include "esphome/core/log.h"

namespace esphome {
namespace marantz_v2007 {

static const char *const TAG = "marantz_v2007";

float MarantzV2007::get_setup_priority() const {
  return setup_priority::BUS;
}

void MarantzV2007::setup() {
  ESP_LOGCONFIG(TAG, "Starting Marantz V2007...");
}

void MarantzV2007::dump_config() {
  ESP_LOGCONFIG(TAG, "Marantz V2007");
}

void MarantzV2007::send_command_(const std::string &cmd) {
  ESP_LOGD(TAG, "TX -> %s", cmd.c_str());
  this->write_str(cmd.c_str());
  this->write_str("\r");
}

void MarantzV2007::process_line_(const std::string &line) {
  ESP_LOGD(TAG, "RX <- %s", line.c_str());
}

void MarantzV2007::loop() {
  while (this->available()) {
    uint8_t c;

    if (!this->read_byte(&c))
      break;

    if (c == '\r' || c == '\n') {
      if (!this->rx_buffer_.empty()) {
        this->process_line_(this->rx_buffer_);
        this->rx_buffer_.clear();
      }
    } else {
      this->rx_buffer_ += static_cast<char>(c);
    }
  }

  const uint32_t now = millis();

  if (now - this->last_poll_ > 5000) {
    this->last_poll_ = now;
    this->send_command_("@PWR:?");
  }
}

}  // namespace marantz_v2007
}  // namespace esphome