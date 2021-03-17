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

int loadshader(const char* filename, GLchar** ShaderSource, int* len){
  std::ifstream file;
  // TODO: this gives error
  file.open(filename, std::ios::in);  // opens as ASCII
  if(!file){
    printf("Error opening shader file\n");
    exit(1);
  }

  *len = getFileLength(file);

  if(*len == 0){
    printf("Error: shader length = 0\n");
    exit(-2);
  }

  *ShaderSource = (GLchar*) new char[*len+1];
  if(*ShaderSource == 0){
    printf("Error creating shader source\n");
    exit(-3);
  }
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

  const char *vs_filename = "vertex_shader.shader";
  const char *fs_filename = "fragment_shader.shader";
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

// TODO: code these
// TODO: maybe the std::string in the functions' parameters need to be const

// Compile the given shader from a file
GLuint Renderer::compile_shader(const std::string& src, GLenum shader_type){

}

// Link the program
GLuint Renderer::link_program(GLuint& shaders){

}

// Return the index of attribute "attr" in renderer's program. Panics if the index is not found
GLuint Renderer::find_program_attrib(const std::string& attr){

}

