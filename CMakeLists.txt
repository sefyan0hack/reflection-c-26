cmake_minimum_required(VERSION 3.30)

project(
    reflection
    DESCRIPTION "Experimenting with C++26 reflection"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-freflection)

set(EXAMPLES
    universal_formatter
    enum_tostring
)

foreach(example IN LISTS EXAMPLES)
    add_executable(${example} ${example}.cpp)
endforeach()

enable_testing()

foreach(example IN LISTS EXAMPLES)
    add_test(NAME ${example} COMMAND $<TARGET_FILE:${example}>)
endforeach()