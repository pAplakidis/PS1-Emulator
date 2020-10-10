# set project name
project(PS1-Emulator)

add_subdirectory(bus)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})

# add all src file needed to build executable
add_executable(main src/main.cc src/ps1/ps1.cc src/ps1/cpu/cpu.h)

