#pragma once

#include <vector>
#include <fstream>
#include <stdint.h>

class Bios{
public:
  // BIOS images are 512KB long
  const uint64_t BIOS_SIZE = 512 * 1024;
  std::vector<uint8_t> data;

  Bios(std::string bios_path);
  uint32_t load32(uint32_t offset);
  uint8_t load8(uint32_t offset);
};

