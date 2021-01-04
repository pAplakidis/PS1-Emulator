#include "bios.h"
#include "map.h"

// global interconnect
class Interconnect{
public:
  Bios *bios;

  Interconnect(Bios *bios);
  uint32_t load32(uint32_t addr);
  void store32(uint32_t addr, uint32_t val);
};

