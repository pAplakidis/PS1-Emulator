#include "ps1.h"

Ps1::Ps1(std::string bios_path, std::string rom_path){
  bios = new Bios(bios_path);

  // we initialize SDL before the interconnect is created since it contains the GPU and the GPU needs to create a window
  int32_t sdl_context = SDL_Init(SDL_INIT_VIDEO);

  Renderer *renderer = new Renderer(sdl_context);
  Gpu *gpu = new Gpu(renderer);
  intercn = new Interconnect(bios, gpu);
  //Debugger = new Debugger();
  cpu = new Cpu(intercn);


  // Temporary tests
  cpu->load_rom(rom_path);
  cpu->main_loop();

  // See if we should quit
  // TODO: handle SDL-quit + debugger.debug()
}

