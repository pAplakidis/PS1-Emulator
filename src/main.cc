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
  
  Ps1 *ps1 = new Ps1(boot_path, rom_path);
  
  return 0;
}

