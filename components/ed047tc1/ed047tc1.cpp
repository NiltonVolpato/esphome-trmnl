#include "ed047tc1_display.h"
#include "epd_driver.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"

#include <esp_heap_caps.h>

namespace esphome {
namespace ed047tc1 {

static const char *const TAG = "ed047tc1";

void ED047TC1::setup() {
  ESP_LOGV(TAG, "Initialize called");
  epd_init();
  uint32_t buffer_size = this->get_buffer_length_();

  if (this->buffer_ != nullptr) {
    free(this->buffer_);  // NOLINT
  }

  this->buffer_ = (uint8_t *) heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM);

  if (this->buffer_ == nullptr) {
    ESP_LOGE(TAG, "Could not allocate buffer for display!");
    this->mark_failed();
    return;
  }

  memset(this->buffer_, 0xFF, buffer_size);
  ESP_LOGV(TAG, "Initialize complete");
}

float ED047TC1::get_setup_priority() const { return setup_priority::PROCESSOR; }
int ED047TC1::get_width_internal() { return this->get_width(); }
int ED047TC1::get_height_internal() { return this->get_height(); }
size_t ED047TC1::get_buffer_length_() {
    return this->get_width_internal() * this->get_height_internal() / 2;
}

void ED047TC1::update() {
  this->do_update_();
  this->display();
}

void HOT ED047TC1::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || y >= this->get_height_internal() || x < 0 || y < 0)
    return;
  uint8_t gs = 255 - ((color.red * 2126 / 10000) + (color.green * 7152 / 10000) + (color.blue * 722 / 10000));
  epd_draw_pixel(x, y, gs, this->buffer_);
}

void ED047TC1::dump_config() {
  LOG_DISPLAY("", "ED047TC1", this);
  ESP_LOGCONFIG(TAG, "  Greyscale: %s", ONOFF(this->greyscale_));
}

void ED047TC1::eink_off_() {
  ESP_LOGV(TAG, "Eink off called");
  if (panel_on_ == 0)
    return;
  epd_poweroff();
  panel_on_ = 0;
}

void ED047TC1::eink_on_() {
  ESP_LOGV(TAG, "Eink on called");
  if (panel_on_ == 1)
    return;
  epd_poweron();
  panel_on_ = 1;
}

void ED047TC1::display() {
  ESP_LOGV(TAG, "Display called");
  uint32_t start_time = millis();

  epd_poweron();
  epd_clear();
  epd_draw_grayscale_image(epd_full_screen(), this->buffer_);
  epd_poweroff();

  ESP_LOGV(TAG, "Display finished (full) (%ums)", millis() - start_time);
}

}  // namespace ed047tc1
}  // namespace esphome
