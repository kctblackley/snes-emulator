cmake_minimum_required(VERSION 3.16)
project(snes_emulator LANGUAGES CXX)

set(CMAKE_CXX_STASNDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB COMPONENT_SOURCES CONFIGURE_DEPENDS
    src/components/*.cpp
)

file(GLOB MISC_SOURCES CONFIGURE_DEPENDS
    src/misc/*.cpp
)


set(SNES_SOURCES
    ${COMPONENT_SOURCES}
    ${MISC_SOURCES}
    src/main.cpp
)

add_executable(snes_emulator ${SNES_SOURCES})

target_include_directories(snes_emulator PRIVATE
    ${CMAKE_SOURCE_DIR}/include/stores
    ${CMAKE_SOURCE_DIR}/include/components
    ${CMAKE_SOURCE_DIR}/include/misc
)   

target_compile_options(snes_emulator PRIVATE)



