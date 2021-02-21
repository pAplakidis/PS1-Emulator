#include "buffer.h"

// Create a new buffer bound tothe current vertex array
template<typename T> Buffer<T>::Buffer(){
  object = 0;

  // Generate the buffer object
  glGenBuffers(1, &object);
  // Bind it
  glBindBuffer(GL_ARRAY_BUFFER, object);

  // Compute the size of the buffer
  GLsizeiptr element_size = (GLsizeiptr)sizeof(T);
  GLsizeiptr buffer_size = element_size * (GLsizeiptr)VERTEX_BUFFER_LEN;

  // Write only persistent mapping. Not coherent!
  uint32_t access = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;

  // Allocate buffer memory
  glBufferStorage(GL_ARRAY_BUFFER, buffer_size, NULL, access);

  // Remap the entire buffer
  T *memory = (T*)glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, access);

  // TODO: this iteration of memory might be wrong
  // Reset the buffer to 0 to avoid hard-to-reproduce bugs if we do something wrong with uninitialized memory
  for(int i=0;i < (size_t)VERTEX_BUFFER_LEN;i++){
    memory[i] = (T)0;
  }

  map = memory;
}

template<typename T> Buffer<T>::~Buffer(){
  glBindBuffer(GL_ARRAY_BUFFER, object);
  glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  glDeleteBuffers(1, &object);
}

// Set entry at "index" to "val" in the buffer
template<typename T> void Buffer<T>::set(uint32_t index, T val){
  if(index >= VERTEX_BUFFER_LEN){
    printf("Buffer overflow\n");
    exit(1);
  }
  
  //TODO: this might be wrong
  T *p = map->offset((size_t)index);
  *p = val;
}

