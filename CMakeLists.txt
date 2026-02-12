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
set(RUN_DEPENDS)

foreach(example IN LISTS EXAMPLES)
    list(APPEND RUN_COMMANDS
        COMMAND ${CMAKE_COMMAND} -E echo "+++++++++++++++++++[${example}]+++++++++++++++++++"
        COMMAND $<TARGET_FILE:${example}>
    )

    list(APPEND RUN_DEPENDS ${example})
endforeach()

add_custom_target(run
    ${RUN_COMMANDS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${RUN_DEPENDS}
    COMMENT "Running all reflection examples in sequence"
)