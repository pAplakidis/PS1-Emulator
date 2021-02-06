#pragma once

#include <vector>
#include <stdint.h>

class Ram{
public:
  std::vector<uint8_t> data;

  Ram();
  uint32_t load32(uint32_t offset);
  void store32(uint32_t offset, uint32_t value);
};

