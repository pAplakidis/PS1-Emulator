#include "renderer.h"

Renderer::Renderer(){
  sdl_context = SDL_Init(SDL_INIT_VIDEO);

  if(sdl_context != 0){
    printf("Unable to initialize SD2: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  window = SDL_CreateWindow(
    "PSX",                  // window title
    SDL_WINDOWPOS_CENTERED, // initial x position
    SDL_WINDOWPOS_CENTERED, // initial y position
    1024,                   // width, in pixels
    512,                    // height, in pixels
    SDL_WINDOW_OPENGL       // flags
  );

  gl_context = SDL_GL_CreateContext(window);

  // TODO: gl::load_with(|s| sdl2::video::gl_get_proc_address(s).unwrap() as *const c_void)
  
}

