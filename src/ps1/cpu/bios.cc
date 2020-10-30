#include <iostream>
#include <fstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bios.h"

Bios::Bios(std::string bios_path){
  // read BIOS into data vector
  std::ifstream instream(bios_path, std::ios::in | std::ios::binary);
  std::vector<uint8_t> temp_data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
  data = temp_data;
}

void Bios::load32(uint32_t offset){
  // byte0, byte1, byte2, byte3 (32bit = 4bytes)
  uint32_t b0 = data[offset + 0];
  uint32_t b1 = data[offset + 1];
  uint32_t b2 = data[offset + 2];
  uint32_t b3 = data[offset + 3];
}

