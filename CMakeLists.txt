cmake_minimum_required(VERSION 3.10)

# set project name
project(PS1-Emulator)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})

# variables for paths
set(SRCDIR "src/")          # source code directory
set(PS1DIR "src/ps1/")      # ps1 directory
set(CMPDIR "src/ps1/components/")  # components directory

# add all src file needed to build executable
add_executable("${PROJECT_NAME}" ${SRCDIR}main.cc ${PS1DIR}ps1.cc ${CMPDIR}cpu.cc ${CMPDIR}instruction.cc ${CMPDIR}bios.cc ${CMPDIR}map.cc ${CMPDIR}interconnect.cc ${CMPDIR}ram.cc ${CMPDIR}helpers.cc ${CMPDIR}dma.cc ${CMPDIR}channel.cc ${CMPDIR}gpu.cc ${CMPDIR}renderer.cc)

set_target_properties("${PROJECT_NAME}" PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

