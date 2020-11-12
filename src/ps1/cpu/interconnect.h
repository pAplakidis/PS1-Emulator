#include <stdio.h>
#include <stdlib.h>

#include "bios.h"
#include "map.h"

// global interconnect
class Interconnect{
public:
  Bios *bios;

  Interconnect(Bios *bios);
  uint32_t load32(uint32_t addr);
};

