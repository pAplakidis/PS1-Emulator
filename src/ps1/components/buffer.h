#pragma once

#include <GL/gl.h>

// Write-only buffer with enough size for VERTEX_BUFFER_LEN elements
template<typename T> class Buffer{
private:
  // OpenGL buffer object
  GLuint object;
  // Mapped buffer memory
  T *map;

public:
  Buffer();
};

