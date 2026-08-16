cmake_minimum_required(VERSION 3.20)

project(simple-risc-v-emulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}
    "src/main.cpp")

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "include")

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/W4" "/permissive-")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        "-Wall"
        "-Wextra"
        "-Wpedantic")
endif()
