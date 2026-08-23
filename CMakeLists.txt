cmake_minimum_required(VERSION 3.20)

project(simple-risc-v-emulator VERSION 1.0.0 LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

if(NOT DEFINED ENV{VCPKG_ROOT})
    message(FATAL_ERROR "VCPKG_ROOT is not defined! Install vcpkg properly!")
endif()
include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options("/W4" "/permissive-")
else()
    add_compile_options("-Wall" "-Wextra" "-Wpedantic")
endif()


find_package(SDL3 CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)


add_library(riscv-emu-lib STATIC)
target_include_directories(riscv-emu-lib PUBLIC ".")
target_sources(riscv-emu-lib
    PRIVATE
        "src/riscv/base.cpp"
        "src/riscv/instruction.cpp"
        "src/riscv/emulator.cpp"
)


add_executable(riscv-emu)

target_include_directories(riscv-emu PRIVATE ".")

target_link_libraries(riscv-emu
    PRIVATE riscv-emu-lib SDL3::SDL3 argparse::argparse
)

target_sources(riscv-emu
    PRIVATE
        "src/main.cpp"
        "src/app.cpp"
        "src/options.cpp"
        "src/renderer.cpp"
)

install(TARGETS riscv-emu RUNTIME DESTINATION bin)


if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)

    add_executable(tests)

    target_link_libraries(tests
        PRIVATE riscv-emu-lib GTest::gtest GTest::gtest_main
    )

    target_sources(tests
        PRIVATE
            "src/tests/instruction_decoding.cpp"
    )

    add_test(NAME riscv-emu-gtest-suite COMMAND tests)
endif()
