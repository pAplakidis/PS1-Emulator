#pragma once

#include <stdio.h>
#include <stdint.h>

#include <vector>

class Ram{
public:
  std::vector<uint8_t> data;

  Ram();
  uint32_t load32(uint32_t offset);
  uint16_t load16(uint32_t offset);
  uint8_t load8(uint32_t offset);
  void store32(uint32_t offset, uint32_t value);
  void store16(uint32_t offset, uint16_t value);
  void store8(uint32_t offset, uint8_t value);
};

