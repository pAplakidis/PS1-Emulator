#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>

class Renderer{
public:
  int32_t sdl_context;
  SDL_Window *window;
  SDL_GLContext gl_context;

  Renderer();
};

