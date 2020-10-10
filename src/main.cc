#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "ps1/ps1.h"

int main(int argc, char **argv){

  if(argc < 3){
    std::cout << "Usage: " << argv[0] << " <boot_rom> <cd_rom>\n";
    exit(1);
  }

  // open ROM files
  std::string boot_path = argv[1];
  std::string rom_path = argv[2];

  std::ifstream boot (boot_path, std::ios::binary);
  std::ifstream rom (rom_path, std::ios::binary);

  Ps1 ps1 = new Ps1(boot, rom);
  
  return 0;
}

