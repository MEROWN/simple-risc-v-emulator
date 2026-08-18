cmake_minimum_required(VERSION 3.20)

project(simple-risc-v-emulator VERSION 1.0.0 LANGUAGES CXX)

if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT is not defined! Install vcpkg properly!")
endif()
include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(riscv-emu)

target_sources(riscv-emu
    PRIVATE
        "src/main.cpp"
        "src/app.cpp"
        "src/options.cpp"
        "src/renderer.cpp"
        "src/riscv/emulator.cpp"
        "src/riscv/instruction.cpp"
)

target_include_directories(riscv-emu
    PRIVATE
        "."
)

if(MSVC)
    target_compile_options(riscv-emu
        PRIVATE
            "/W4"
            "/permissive-"
    )
else()
    target_compile_options(riscv-emu
        PRIVATE
            "-Wall"
            "-Wextra"
            "-Wpedantic"
    )
endif()


find_package(SDL3 REQUIRED)
find_package(argparse REQUIRED)

target_link_libraries(riscv-emu
    PRIVATE
        SDL3::SDL3
        argparse::argparse
)


install(TARGETS riscv-emu
    RUNTIME DESTINATION bin
)
