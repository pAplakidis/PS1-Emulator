#include <fstream>
#include <vector>
#include <inttypes.h>

class Cpu{
  //const uint32_t MEMORY_SIZE = 4096;    // CHANGE THIS 4KB instruction memory, 1KB data cache

public:
  std::vector<char> m_rom_data; // buffer for storing data from ROM

  Cpu();
  void load_rom(std::string rom_path);

};

