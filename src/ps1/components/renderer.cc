#include "renderer.h"

// --------------------------------------------------------------

// Postion in VRAM and RGB color methods

// Parse position frtom GP0 parameter
void Position::from_gp0(uint32_t val){
  int16_t x = (int16_t)val;
  int16_t y = (int16_t)(val >> 16);

  pos[0] = (GLshort)x;
  pos[1] = (GLshort)y;
}

void Color::from_gp0(uint32_t val){
  uint8_t r = (uint8_t)val;
  uint8_t g = (uint8_t)(val >> 8);
  uint8_t b = (uint8_t)(val >> 16);

  rgb[0] = r;
  rgb[1] = g;
  rgb[2] = b;
}

// --------------------------------------------------------------

Renderer::Renderer(){
  sdl_context = SDL_Init(SDL_INIT_VIDEO);

  if(sdl_context != 0){
    printf("Unable to initialize SD2: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  window = SDL_CreateWindow(
    "PS1",                  // window title
    SDL_WINDOWPOS_CENTERED, // initial x position
    SDL_WINDOWPOS_CENTERED, // initial y position
    1024,                   // width, in pixels
    512,                    // height, in pixels
    SDL_WINDOW_OPENGL       // flags
  );

  if(window == NULL){
    printf("Could not create window: %s\n", SDL_GetError());
    exit(1);
  }

  gl_context = SDL_GL_CreateContext(window);

  // gl::load_with(|s| sdl2::video::gl_get_proc_address(s).unwrap() as *const c_void)
  // this is in so that we can use this: SDL_GL_GetProcAddress(s);
  // TODO: don't forget to use SDL_DestroyWindow(window) and clean up when done (SDL_Quit when the program exits)

  // Clear the window
  glClearColor(0., 0., 0., 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window);
}

