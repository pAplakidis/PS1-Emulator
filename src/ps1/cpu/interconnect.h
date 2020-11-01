#include <stdio.h>
#include <stdlib.h>

#include "bios.h"
#include "map.h"

// global interconnect
class Interconnect{
public:
  Bios *bios;

  Interconnect(std::string bios_path);
  uint32_t load32(uint32_t addr);
};

