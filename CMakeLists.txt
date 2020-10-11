cmake_minimum_required(VERSION 3.10)

# set project name
project(PS1-Emulator)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})

# add all src file needed to build executable
add_executable("${PROJECT_NAME}" src/main.cc src/ps1/ps1.cc src/ps1/cpu/cpu.cc)
set_target_properties("${PROJECT_NAME}" PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
