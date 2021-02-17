#pragma once

#include <stdint.h>

class Gpu{
private:
  // Texture page base X coordinate (4 bits, 64 byte increment)
  uint8_t page_base_x;
  // Texture page base Y coordinate (1 bits, 256 line increment)
  uint8_t page_base_y;
};

