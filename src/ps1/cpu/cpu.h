#include <fstream>
#include <vector>
#include <inttypes.h>

class Cpu{

public:
  //const uint32_t MEMORY_SIZE = 4096;    // CHANGE THIS maybe?
  static const uint32_t MEMORY_SIZE = 5120;    // 4KB instruction memory, 1KB data cache

  unsigned char m_memory[MEMORY_SIZE];

  Cpu();
  void load_rom(std::string rom_path);
  void cycle();

};

