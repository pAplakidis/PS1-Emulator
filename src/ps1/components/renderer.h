#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>

#include "buffer.h"
#include "buffer_impl.h"

// Position in VRAM
class Position{
public:
  GLshort pos[2];
  void from_gp0(uint32_t val);
};

// RGB color
class Color{
public:
  GLubyte rgb[3];
  void from_gp0(uint32_t val);
};

class Renderer{
public:
  int32_t sdl_context;
  SDL_Window *window;
  SDL_GLContext gl_context;

  // Vertex shader object
  GLuint vertex_shader;
  // Fragment shader object
  GLuint fragment_shader;
  // OpenGL Program object
  GLuint program;
  // OpenGL Vertex array object
  GLuint vertex_array_object;
  // Buffer containing the vertice positions
  Buffer<Position> positions;
  // Buffer containing the vertice colors
  Buffer<Color> colors;
  // Current number of vertices in the buffers
  uint32_t nvertices;

  Renderer();
  ~Renderer();
  GLuint compile_shader(const std::string& src, GLenum shader_type);
  GLuint link_program(GLuint& shaders);
  GLuint find_program_attrib(const char* attr);
  void push_triangle(Position positions[3], Color colors[3]);
  void draw();
  void display();
};

