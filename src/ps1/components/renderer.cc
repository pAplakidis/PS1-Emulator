#include "renderer.h"

// These functions help us load the shaders into memory
unsigned long getFileLength(std::ifstream& file){
  if(!file.good()) return 0;

  unsigned long pos = file.tellg();
  file.seekg(0, std::ios::end);
  unsigned long len = file.tellg();
  file.seekg(std::ios::beg);

  return len;
}

unsigned long getFileSize(const char* filename){
  struct stat stat_buf;
  int rc = stat(filename, &stat_buf);
  return rc == 0 ? stat_buf.st_size : -1;
}

int loadshader(const char* filename, GLchar** ShaderSource, int* len){
  std::ifstream file;
  file.open(filename, std::ios::in);  // opens as ASCII
  if(!file){
    printf("Error opening shader file %s\n", filename);
    exit(1);
  }

  //*len = getFileLength(file);
  *len = getFileSize(filename);

  if(*len == 0){
    printf("Error: shader length = 0\n");
    exit(-2);
  }

  *ShaderSource = (GLchar*) new char[*len+1];
  if(*ShaderSource == 0){
    printf("Error creating shader source\n");
    exit(-3);
  }
  // TODO: segfaults here!!!
  *ShaderSource[*len] = 0;
  
  unsigned int i = 0;
  while(file.good()){
    *ShaderSource[i] = file.get();  // get char from file
    if(!file.eof())
      i++;
  }

  *ShaderSource[i] = 0;
  file.close();

  return 0;
}

void unloadshader(GLubyte** ShaderSource){
  if(*ShaderSource != 0)
    delete[] *ShaderSource;
  *ShaderSource = 0;
}

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

  // Create and compile shaders (TODO: break this into functions)
  program = glCreateProgram();

  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  int vlength;
  int flength;
  char *vs_src;
  char *fs_src;

  // TODO: this requires full path
  const char *vs_filename = "/home/neo/Dev/PS1-Emulator/src/ps1/components/vertex_shader.shader";
  const char *fs_filename = "/home/neo/Dev/PS1-Emulator/src/ps1/components/fragment_shader.shader";
  loadshader(vs_filename, &vs_src, &vlength);
  loadshader(fs_filename, &fs_src, &flength);

  // TODO: check out glShaderSourceARB() + you can use nullptr instead of &vlength
  glShaderSource(vertex_shader, 1, &vs_src, &vlength);
  glShaderSource(fragment_shader, 1, &fs_src, &flength);

  glCompileShader(vertex_shader);
  glCompileShader(fragment_shader);

  // Attach shaders to program and Link it
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glValidateProgram(program);

  //glDeleteShader(vertex_shader);
  //glDeleteShader(fragment_shader);

  glUseProgram(program);

  // Generate the vertex attribute object that will hold the vertex attributes
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  // Bind the VAO
  glBindVertexArray(vao);

  // Retrieve the index of the attribute in the shader
  GLuint index = find_program_attrib("vertex_position");
  // Enable it
  glEnableVertexAttribArray(index);
  // Link the buffer and the index: 2 GLshort attributes, not normalized
  // That should send the data untouched to the vertex shader
  glVertexAttribIPointer(index, 2, GL_SHORT, 0, NULL);

  // Bind the color attribute
  index = find_program_attrib("vertex_color");
  glEnableVertexAttribArray(index);

  // Link the buffer and the index: 3 GLByte attributes, not normalized
  // That should send the data untouched to the vertex shader
  glVertexAttribIPointer(index, 3, GL_UNSIGNED_BYTE, 0, NULL);
}

Renderer::~Renderer(){
  glDeleteVertexArrays(1, &vertex_array_object);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  glDeleteProgram(program);
}

// TODO: put the code parts from above here to look cleaner
// Compile the given shader from a file
GLuint Renderer::compile_shader(const std::string& src, GLenum shader_type){

}

// Link the program
GLuint Renderer::link_program(GLuint& shaders){

}

// Return the index of attribute "attr" in renderer's program. Panics if the index is not found
GLuint Renderer::find_program_attrib(const char* attr){
  GLint index = glGetAttribLocation(program, attr);

  if(index < 0){
    printf("Attribute \"%s\" not found in program\n", attr);
    exit(1);
  }

  return index;
}

// Add a triangle to the draw buffer
void Renderer::push_triangle(Position positions[3], Color colors[3]){
  // Make sure we have enough room left to queue the vertex
  if(nvertices + 3 > VERTEX_BUFFER_LEN){
    printf("Vertex attribute buffers full, forcing draw\n");
    draw();
  }

  for(int i=0;i<3;i++){
    // Push
    this->positions.set(nvertices, positions[i]);
    this->colors.set(nvertices, colors[i]);
    nvertices++;
  }
}

// Add a quad to the draw buffer
void Renderer::push_quad(Position positions[4], Color colors[4]){
  // make sure we have enough room left to queue the vertex
  // we need to push two triangles to draw a quad, so 6 vertices
  if(nvertices + 6 > VERTEX_BUFFER_LEN)
    // the vertex attribute buffers are full, force an early draw
    draw();

  // Push the first triangle
  for(int i=0;i<3;i++){
    this->positions.set(nvertices, positions[i]);
    this->colors.set(nvertices, colors[i]);
    nvertices++;
  }

  // Push the second triangle
  for(int i=1;i<4;i++){
    this->positions.set(nvertices, positions[i]);
    this->colors.set(nvertices, colors[i]);
    nvertices++;
  }
}

// TODO: study this
// Draw the buffered commands and reset the buffers
void Renderer::draw(){
  // Make sure all the data from the persistent mappings is flushed to the buffer
  glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
  glDrawArrays(GL_TRIANGLES, 0, (GLsizei)nvertices);

  // Wait for GPU to complete
  GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

  while(1){
    GLenum r = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 10000000);

    if(r == GL_ALREADY_SIGNALED || r == GL_CONDITION_SATISFIED){
      // Drawing done
      break;
    }
  }

  // Reset the buffers
  nvertices = 0;
}

// Draw the buffered commands and display them
void Renderer::display(){
  draw();
  SDL_GL_SwapWindow(window);
}

// Check for OpenGL errors using glGetDebugMessageLog. If a severe error is encoutered this function panics
// If the OpenGL context doesn't have the DEBUG asttribute this won't work
void Renderer::check_for_errors(){
  bool fatal = false;

  while(1){
    std::vector<GLsizei> buffer;
    buffer.assign(4096, 0);
    int severity = 0;
    int source = 0;
    int message_size = 0;
    int mtype = 0;
    int id = 0;

    GLuint count = glGetDebugMessageLog(1,
                                        (GLsizei)buffer.size(),
                                        (GLenum*)&source,
                                        (GLenum*)&mtype,
                                        (GLuint*)&id,
                                        (GLenum*)&severity,
                                        (GLsizei*)&message_size,
                                        (GLchar*)&buffer);
    if(count == 0)
      // No messages left
      break;

    // TODO: this might be wrong
    buffer.resize((size_t)message_size);

    // TODO: check if message is utf-8
    /*
    switch(){
      case
    }
    */

    // TODO: finish debugging code
    // https://www.khronos.org/registry/OpenGL/extensions/KHR/KHR_debug.txt
  }
  if(fatal){
    printf("Fatal OpenGL error!\n");
    exit(1);
  }
}

