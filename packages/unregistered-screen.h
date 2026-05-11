#include "esphome.h"

inline void display_fuzzy_time(esphome::display::Display& it,
                               const esphome::ESPTime& time) {
  int w = it.get_width();
  int h = it.get_height();
  int mid = h / 2;

  int hour = time.hour;      // 0-23
  int minute = time.minute;  // 0-59

  // Convert to 12-hour
  int hour12 = hour % 12;
  int next_hour12 = (hour12 + 1) % 12;

  static const char* hour_words[] = {"twelve", "one",  "two", "three",
                                     "four",   "five", "six", "seven",
                                     "eight",  "nine", "ten", "eleven"};

  const char* relation;
  const char* hour_word;
  bool single_line = false;

  if (minute <= 2) {
    relation = "o'clock";
    hour_word = hour_words[hour12];
    single_line = true;
  } else if (minute <= 7) {
    relation = "five past";
    hour_word = hour_words[hour12];
  } else if (minute <= 12) {
    relation = "ten past";
    hour_word = hour_words[hour12];
  } else if (minute <= 17) {
    relation = "quarter past";
    hour_word = hour_words[hour12];
  } else if (minute <= 22) {
    relation = "twenty past";
    hour_word = hour_words[hour12];
  } else if (minute <= 27) {
    relation = "twenty-five past";
    hour_word = hour_words[hour12];
  } else if (minute <= 32) {
    relation = "half past";
    hour_word = hour_words[hour12];
  } else if (minute <= 37) {
    relation = "twenty-five to";
    hour_word = hour_words[next_hour12];
  } else if (minute <= 42) {
    relation = "twenty to";
    hour_word = hour_words[next_hour12];
  } else if (minute <= 47) {
    relation = "quarter to";
    hour_word = hour_words[next_hour12];
  } else if (minute <= 52) {
    relation = "ten to";
    hour_word = hour_words[next_hour12];
  } else if (minute <= 57) {
    relation = "five to";
    hour_word = hour_words[next_hour12];
  } else {
    // 58-59
    relation = "o'clock";
    hour_word = hour_words[next_hour12];
    single_line = true;
  }

  if (single_line) {
    // Whole phrase in large font, centered
    it.printf(w / 2, mid, &id(sans_serif_large), TextAlign::CENTER, "%s %s",
              hour_word, relation);
  } else {
    // Two lines: relation in normal font, hour in large font
    it.printf(w / 2, mid - 30, &id(sans_serif_normal), TextAlign::CENTER, "%s",
              relation);
    it.printf(w / 2, mid + 20, &id(sans_serif_large), TextAlign::CENTER, "%s",
              hour_word);
  }
}