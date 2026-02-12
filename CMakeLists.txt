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

set(RUN_COMMANDS)
foreach(example IN LISTS EXAMPLES)
    list(APPEND RUN_COMMANDS COMMAND ${CMAKE_COMMAND} -E echo "\t${example}\t\n")
    list(APPEND RUN_COMMANDS COMMAND $<TARGET_FILE:${example}>)
    list(APPEND RUN_COMMANDS COMMAND ${CMAKE_COMMAND} -E echo "\t----------\t\n")
endforeach()

add_custom_target(run
    ${RUN_COMMANDS}
    DEPENDS ${EXAMPLES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)