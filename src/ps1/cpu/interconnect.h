#include "bios.h"
#include "map.h"

// global interconnect
class Interconnect{
public:
  Bios *bios;

  Interconnect(Bios *bios);
  uint32_t load32(uint32_t addr);
  void store32(uint32_t addr, uint32_t val);
  void store16(uint32_t addr, uint16_t val);
  void store8(uint32_t addr, uint8_t val);
};

