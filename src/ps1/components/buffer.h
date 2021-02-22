#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glext.h>

const uint32_t VERTEX_BUFFER_LEN = 64 * 1024;

// Write-only buffer with enough size for VERTEX_BUFFER_LEN elements
template<class T>
class Buffer{
private:
  // OpenGL buffer object
  GLuint object;
  // Mapped buffer memory
  T *map;

public:
  Buffer();
  ~Buffer();
  void set(uint32_t index, T val);
};

