#include <vector>
#include <stdint.h>

class Bios{
public:
  // BIOS images are 512KB long
  const uint64_t BIOS_SIZE = 512 * 1024;
  std::vector<uint8_t> data;

  Bios(std::string bios_path);

  // fetch the 32bit little endian word at 'offset'
  void load32(uint32_t offset);
};

