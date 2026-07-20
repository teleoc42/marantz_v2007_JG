#include "marantz_v2007.h"

#include "esphome/core/log.h"

namespace esphome {
namespace marantz_v2007 {

static const char *const TAG = "marantz_v2007";

void MarantzV2007::setup() {
  ESP_LOGCONFIG(TAG, "Starting Marantz V2007...");
}

void MarantzV2007::loop() {
  // Future parser will be implemented here.
}

void MarantzV2007::dump_config() {
  ESP_LOGCONFIG(TAG, "Marantz V2007");
  LOG_UART_DEVICE(this);
}

}  // namespace marantz_v2007
}  // namespace esphome