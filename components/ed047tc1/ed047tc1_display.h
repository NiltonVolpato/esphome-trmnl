#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

#include "epd_driver.h"

#include <vector>

namespace esphome {
namespace ed047tc1 {

class ED047TC1 : public display::DisplayBuffer {
 public:
  void setup() override;
  void display();
  void dump_config() override;
  void update() override;

  float get_setup_priority() const override;

  void set_greyscale(bool greyscale) { this->greyscale_ = greyscale; }
  bool get_greyscale() { return this->greyscale_; }

  display::DisplayType get_display_type() override {
    return this->greyscale_ ? display::DisplayType::DISPLAY_TYPE_GRAYSCALE
                            : display::DisplayType::DISPLAY_TYPE_BINARY;
  }

 protected:
  void eink_off_();
  void eink_on_();
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;
  size_t get_buffer_length_();

  bool greyscale_{true};
  bool panel_on_{0};
  uint8_t *buffer_{nullptr};
  uint32_t last_update_{0};
};

}  // namespace ed047tc1
}  // namespace esphome
