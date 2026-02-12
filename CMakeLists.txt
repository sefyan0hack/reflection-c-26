cmake_minimum_required(VERSION 3.30)

project(
    reflection
    DESCRIPTION "Experimenting with C++26 reflection"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# note now tested obly on (gcc tunk) probaly gcc-16

add_compile_options(-freflection)
add_executable(universal_formatter universal_formatter.cpp)

add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E echo "+++++++++++++++++++[universal_formatter]+++++++++++++++++++"
    COMMAND $<TARGET_FILE:universal_formatter>
    COMMAND ${CMAKE_COMMAND} -E echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
    DEPENDS universal_formatter
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)