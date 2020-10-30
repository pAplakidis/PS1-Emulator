#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "bios.h"

Bios::Bios(std::string bios_path){
  FILE *file;
  const char *path = bios_path.c_str(); // convert c++ string to c const char*

  if((file = fopen(path, "rb")) == NULL){
    printf("Error opening BIOS file\n");
    exit(1);
  }

  fgets(data, BIOS_SIZE, file);
  fclose(file);
}

