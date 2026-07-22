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
  ESP_LOGD(TAG, ">>> TX >>> %s", cmd.c_str());

  this->write_str(cmd.c_str());
  this->write_str("\r");
}

void MarantzV2007::process_line_(const std::string &line) {
  ESP_LOGD(TAG, "RX <- %s", line.c_str());

  // -------------------------
  // Power
  // -------------------------
  if (line.rfind("@PWR:", 0) == 0) {
    power_ = (line.substr(5) == "2");

    ESP_LOGI(TAG, "Power = %s", power_ ? "ON" : "OFF");
    return;
  }

  // -------------------------
  // Volume
  // -------------------------
  if (line.rfind("@VOL:", 0) == 0) {
    volume_ = atoi(line.substr(5).c_str());

    ESP_LOGI(TAG, "Volume = %d dB", volume_);
    return;
  }

  // -------------------------
  // Mute
  // -------------------------
  if (line.rfind("@AMT:", 0) == 0) {
    mute_ = (line.substr(5) == "2");

    ESP_LOGI(TAG, "Mute = %s", mute_ ? "ON" : "OFF");
    return;
  }

  // -------------------------
  // Source
  // -------------------------
  if (line.rfind("@SRC:", 0) == 0) {

    const std::string code = line.substr(5);

    if (code == "91")
      source_ = "TV";
    else if (code == "9C")
      source_ = "CD";
    else if (code == "9G")
      source_ = "TUNER";
    else if (code == "9N")
      source_ = "M-XPORT";
    else if (code == "MM")
      source_ = "BLU-RAY";
    else if (code == "22")
      source_ = "DVD";
    else if (code == "33")
      source_ = "VCR";
    else if (code == "55")
      source_ = "VINYL";
    else if (code == "99")
      source_ = "AUX";
    else
      source_ = code;

    ESP_LOGI(TAG, "Source = %s", source_.c_str());
    return;
  }

  ESP_LOGW(TAG, "Unknown response: %s", line.c_str());
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

    switch (this->poll_step_) {

    case 0:
      this->send_command_("@PWR:?");
      break;

    case 1:
      this->send_command_("@VOL:?");
      break;

    case 2:
      this->send_command_("@AMT:?");
      break;

    case 3:
      this->send_command_("@SRC:?");
      break;
  }

  this->poll_step_ = (this->poll_step_ + 1) % 4;
}
}

    // Choisir UNE commande pendant les tests.
    //this->send_command_("@VOL:?");
    //this->send_command_("@AMT:?");
    //this->send_command_("@SRC:?");

}  // namespace marantz_v2007
}  // namespace esphome


  