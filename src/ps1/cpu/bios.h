#include <vector>
#include <stdint.h>

class Bios{
public:
  // BIOS images are 512KB long
  const uint64_t BIOS_SIZE = 512 * 1024;
  //std::vector<uint8_t> data;  // TODO: might not be needed (char* might suffice)
  char *data;

  Bios(std::string bios_path);

};

