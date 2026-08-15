cmake_minimum_required(VERSION 3.20)
project(sanskrit-breakdown LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/include/*.h"
     "${CMAKE_SOURCE_DIR}/include/*.hpp"
     "${CMAKE_SOURCE_DIR}/include/*.hh"
)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/src/*.cpp"
     "${CMAKE_SOURCE_DIR}/src/*.cxx"
     "${CMAKE_SOURCE_DIR}/src/*.cc"
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    src/sandhi-splitter.cpp
    include/root-deriver.hpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
)

# Optional: if you use warnings
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)

file(COPY ${CMAKE_SOURCE_DIR}/roots.json DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/grammar_constants.json DESTINATION ${CMAKE_BINARY_DIR})

include(FetchContent)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Replace 'my_target' with your executable or library target
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)