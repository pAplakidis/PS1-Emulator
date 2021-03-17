cmake_minimum_required(VERSION 3.10)

# set project name
project(PS1-Emulator)

# check packages
find_package(SDL2 REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})
set(CMAKE_CXX_FLAGS "-DGL_GLEXT_PROTOTYPES")

# variables
set(SRCDIR "src/")          # source code directory
set(PS1DIR "src/ps1/")      # ps1 directory
set(CMPDIR "src/ps1/components/")  # components directory

# add all src file needed to build executable
add_executable("${PROJECT_NAME}" ${SRCDIR}main.cc ${PS1DIR}ps1.cc ${CMPDIR}cpu.cc ${CMPDIR}instruction.cc ${CMPDIR}bios.cc ${CMPDIR}map.cc ${CMPDIR}interconnect.cc ${CMPDIR}ram.cc ${CMPDIR}helpers.cc ${CMPDIR}dma.cc ${CMPDIR}channel.cc ${CMPDIR}gpu.cc ${CMPDIR}renderer.cc)
target_link_libraries("${PROJECT_NAME}" SDL2main SDL2 GL)

set_target_properties("${PROJECT_NAME}" PROPERTIES CFLAGS "-m32" LDFLAGS "-m32")

