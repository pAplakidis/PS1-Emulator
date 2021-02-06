cmake_minimum_required(VERSION 3.10)

# set project name
project(PS1-Emulator)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})

# variables for paths
set(SRCDIR "src/")
set(PS1DIR "src/ps1/")
set(CPUDIR "src/ps1/cpu/")

# add all src file needed to build executable
add_executable("${PROJECT_NAME}" ${SRCDIR}main.cc ${PS1DIR}ps1.cc ${CPUDIR}cpu.cc ${CPUDIR}instruction.cc ${CPUDIR}bios.cc ${CPUDIR}map.cc ${CPUDIR}interconnect.cc ${CPUDIR}ram.cc)

set_target_properties("${PROJECT_NAME}" PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

