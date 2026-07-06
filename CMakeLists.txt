cmake_minimum_required(VERSION 3.16)
project(snes_emulator LANGUAGES CXX)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB COMPONENT_SOURCES CONFIGURE_DEPENDS
    src/components/*.cpp
)

file(GLOB MISC_SOURCES CONFIGURE_DEPENDS
    src/misc/*.cpp
)

file(GLOB CPU_OPERATIONS_SOURCES CONFIGURE_DEPENDS
    src/cpu_operations/*.cpp
)

file(GLOB MAPPER_SOURCES
CONFIGURE_DEPENDS
    src/mappers/*.cpp
)

file(GLOB TESTING_SOURCES CONFIGURE_DEPENDS
    src/testing/*.cpp
)


set(SNES_SOURCES
    ${COMPONENT_SOURCES}
    ${MISC_SOURCES}
    ${CPU_OPERATIONS_SOURCES}
    ${MAPPER_SOURCES}
    ${TESTING_SOURCES}
    src/main.cpp
)

add_executable(snes_emulator ${SNES_SOURCES})

target_include_directories(snes_emulator PRIVATE
    ${CMAKE_SOURCE_DIR}/include/stores
    ${CMAKE_SOURCE_DIR}/include/components
    ${CMAKE_SOURCE_DIR}/include/misc
    ${CMAKE_SOURCE_DIR}/include/cpu_operations
    ${CMAKE_SOURCE_DIR}/include/mappers
    ${CMAKE_SOURCE_DIR}/include/testing
)   

target_compile_options(snes_emulator PRIVATE)



