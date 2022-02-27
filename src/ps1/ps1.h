#pragma once

#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>

#include "debugger.h"
#include "components/cpu.h"

class Ps1{

public:
  Bios *bios;
  Interconnect *intercn;
  Debugger *debugger;
  Cpu *cpu;
  Renderer *renderer;
  Gpu *gpu;

  Ps1(std::string bios_path, std::string rom_path);
};

