#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <vector>

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
  // Index of the "offset" shader uniform
  GLint uniform_offset;

  Renderer(int32_t sdl_context);
  ~Renderer();
  GLuint compile_shader(const std::string& src, GLenum shader_type);
  GLuint link_program(GLuint& shaders);
  GLuint find_program_attrib(const char* attr);
  GLint find_program_uniform(const char* name);
  void push_triangle(Position positions[3], Color colors[3]);
  void push_quad(Position positions[4], Color colors[4]);
  void draw();
  void display();
  void check_for_errors();
  void set_draw_offset(int16_t x, int16_t y);
};

