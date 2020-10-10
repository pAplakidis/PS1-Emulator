cmake_minimum_required(VERSION 3.10)

# set project name
project(PS1-Emulator)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})

# add all src file needed to build executable
add_executable(main main.cc ps1/ps1.cc ps1/cpu/cpu.h)

