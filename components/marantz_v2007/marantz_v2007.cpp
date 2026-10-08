#include "marantz_v2007.h"
#include "media_player.h"
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

void MarantzV2007::set_media_player(MarantzMediaPlayer *player) {
  media_player_ = player;

  if (media_player_ != nullptr) {
    media_player_->set_parent(this);
  }
}

void MarantzV2007::dump_config() {
  ESP_LOGCONFIG(TAG, "Marantz V2007");
}

void MarantzV2007::send_command_(const std::string &cmd) {
  ESP_LOGD(TAG, ">>> TX >>> %s", cmd.c_str());

  this->write_str(cmd.c_str());
  this->write_str("\r");
}

// ===================================================
// Commandes d'écriture
//
// Protocole "legacy" Marantz 2007-2010 (@CMD:VALEUR\r),
// utilisé notamment par le SR5004. Les commandes ci-dessous
// sont déduites par symétrie avec les réponses déjà validées
// par le polling (ex: réponse "@PWR:2" = allumé), et confirmées
// par la documentation RS232 publique de cette famille d'amplis.
//
// A CONFIRMER avec ton ampli réel : regarde les logs ESPHome
// après chaque action pour vérifier que l'état change bien.
// Si ce n'est pas le cas, utilise l'entité de test "commande brute"
// du fichier de configuration exemple pour essayer d'autres valeurs.
// ===================================================

void MarantzV2007::power_on() {
  this->send_command_("@PWR:2");
}

void MarantzV2007::power_off() {
  this->send_command_("@PWR:1");
}

void MarantzV2007::toggle_power() {
  if (this->power_)
    this->power_off();
  else
    this->power_on();
}

void MarantzV2007::set_mute(bool mute) {
  this->send_command_(mute ? "@AMT:2" : "@AMT:1");
}

void MarantzV2007::set_speaker_a(bool on) {
  this->send_command_(on ? "@SPK:2" : "@SPK:1");
}

void MarantzV2007::set_speaker_b(bool on) {
  this->send_command_(on ? "@SPK:4" : "@SPK:3");
}

void MarantzV2007::volume_up() {
  this->send_command_("@VOL:1");
}

void MarantzV2007::volume_down() {
  this->send_command_("@VOL:2");
}

void MarantzV2007::volume_up_5() {
  this->send_command_("@VOL:3");
}

void MarantzV2007::volume_down_5() {
  this->send_command_("@VOL:4");
}

void MarantzV2007::set_volume_db(int db) {
  this->send_command_("@VOL:0" + std::to_string(db));
}

void MarantzV2007::select_source(const std::string &source_name) {
  std::string code;

  if (source_name == "TV")
    code = "1";
  else if (source_name == "CD")
    code = "C";
  else if (source_name == "TUNER")
    code = "G";
  else if (source_name == "M-XPORT")
    code = "N";
  else if (source_name == "BLU-RAY")
    code = "M";
  else if (source_name == "DVD")
    code = "2";
  else if (source_name == "VCR")
    code = "3";
  else if (source_name == "VINYL")
    code = "5";
  else if (source_name == "AUX")
    code = "9";
  else {
    ESP_LOGW(TAG, "Source inconnue: %s", source_name.c_str());
    return;
  }

  this->send_command_("@SRC:" + code);
}

void MarantzV2007::select_surround_mode(const std::string &mode_name) {
  std::string code;

  if (mode_name == "AUTO") code = "00";
  else if (mode_name == "STEREO") code = "01";
  else if (mode_name == "DOLBY") code = "02";
  else if (mode_name == "MULTI-CH") code = "0H";
  else if (mode_name == "DTS") code = "0M";
  else if (mode_name == "VIRTUAL") code = "0L";
  else if (mode_name == "SOURCE DIRECT") code = "0T";
  else if (mode_name == "PURE DIRECT") code = "0U";
  else {
    ESP_LOGW(TAG, "Mode surround inconnu: %s", mode_name.c_str());
    return;
  }

  this->send_command_("@SUR:" + code);
}

// ===================================================
// Réception / parsing (inchangé, déjà validé)
// ===================================================

void MarantzV2007::process_line_(const std::string &line) {
  ESP_LOGD(TAG, "RX <- %s", line.c_str());

  // -------------------------
  // Power
  // -------------------------
  if (line.rfind("@PWR:", 0) == 0) {
    power_ = (line.substr(5) == "2");

    ESP_LOGI(TAG, "Power = %s", power_ ? "ON" : "OFF");

    if (media_player_ != nullptr)
      media_player_->publish_state_from_parent();

    return;
  }

  // -------------------------
  // Volume
  // -------------------------
  if (line.rfind("@VOL:", 0) == 0) {
    volume_ = atoi(line.substr(5).c_str());

    ESP_LOGI(TAG, "Volume = %d dB", volume_);

    if (media_player_ != nullptr)
      media_player_->publish_state_from_parent();

    return;
  }

  // -------------------------
  // Mute
  // -------------------------
  if (line.rfind("@AMT:", 0) == 0) {
    mute_ = (line.substr(5) == "2");

    ESP_LOGI(TAG, "Mute = %s", mute_ ? "ON" : "OFF");

    if (media_player_ != nullptr)
      media_player_->publish_state_from_parent();

    return;
  }

  // -------------------------
  // Speaker A/B
  // -------------------------
  if (line.rfind("@SPK:", 0) == 0) {
    const std::string val = line.substr(5);

    if (val.size() == 2) {
      speaker_a_ = (val[0] == '2');
      speaker_b_ = (val[1] == '2');

      ESP_LOGI(TAG, "Speaker A = %s, Speaker B = %s",
               speaker_a_ ? "ON" : "OFF",
               speaker_b_ ? "ON" : "OFF");
    }

    return;
  }


  // -------------------------
  // Source
  // -------------------------
  if (line.rfind("@SRC:", 0) == 0) {

    const std::string code = line.substr(5);

    if (code.size() != 2) {
      ESP_LOGW(TAG, "Réponse SRC de longueur inattendue: %s", code.c_str());
      return;
    }

    // Le 2ème caractère indique la source réellement active.
    // Le 1er caractère suit la vidéo et ne bouge pas toujours
    // pour les sources purement audio (TV/CD/TUNER/M-XPORT).
    switch (code[1]) {
      case '1': source_ = "TV"; break;
      case 'C': source_ = "CD"; break;
      case 'G': source_ = "TUNER"; break;
      case 'N': source_ = "M-XPORT"; break;
      case 'M': source_ = "BLU-RAY"; break;
      case '2': source_ = "DVD"; break;
      case '3': source_ = "VCR"; break;
      case '5': source_ = "VINYL"; break;
      case '9': source_ = "AUX"; break;
      default:
        ESP_LOGW(TAG, "Code source inconnu: %s (source précédente conservée)", code.c_str());
        return;
    }

    ESP_LOGI(TAG, "Source = %s", source_.c_str());

    if (media_player_ != nullptr)
      media_player_->publish_state_from_parent();

    return;
  }


  if (line.rfind("@SUR:", 0) == 0) {
    const std::string code = line.substr(5);

    if (code.size() != 1) {
      ESP_LOGW(TAG, "Réponse SUR de longueur inattendue: %s", code.c_str());
      return;
    }

    switch (code[0]) {
      case '0': surround_mode_ = "AUTO"; break;
      case '1': surround_mode_ = "STEREO"; break;
      case '2': surround_mode_ = "DOLBY"; break;
      case 'H': surround_mode_ = "MULTI-CH"; break;
      case 'L': surround_mode_ = "VIRTUAL"; break;
      case 'M': surround_mode_ = "DTS"; break;
      case 'T': surround_mode_ = "SOURCE DIRECT"; break;
      case 'U': surround_mode_ = "PURE DIRECT"; break;
      default:
        ESP_LOGW(TAG, "Mode surround inconnu: %s (précédent conservé)", code.c_str());
        return;
    }

    ESP_LOGI(TAG, "Surround = %s", surround_mode_.c_str());

    if (media_player_ != nullptr)
      media_player_->publish_state_from_parent();

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

      case 4:
        this->send_command_("@SPK:?");
        break;

      case 5:
        this->send_command_("@SUR:?");
        break;
    }

    this->poll_step_ = (this->poll_step_ + 1) % 6;
  }
}

}  // namespace marantz_v2007
}  // namespace esphome
