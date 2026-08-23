cmake_minimum_required(VERSION 3.20)

option(SIMPLE_RISCV_EMU_BUILD_TESTS "Build tests" OFF)
option(SIMPLE_RISCV_EMU_BUILD_APP "Build the graphical application" ON)

if(SIMPLE_RISCV_EMU_BUILD_APP)
    list(APPEND VCPKG_MANIFEST_FEATURES "app")
endif()

if(SIMPLE_RISCV_EMU_BUILD_TESTS)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

project(simple-riscv-emulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options("/W4" "/permissive-")
else()
    add_compile_options("-Wall" "-Wextra" "-Wpedantic")
endif()


add_library(riscv-emu-lib STATIC)
target_include_directories(riscv-emu-lib PUBLIC ".")
target_sources(riscv-emu-lib
    PRIVATE
        "src/riscv/base.cpp"
        "src/riscv/instruction.cpp"
        "src/riscv/emulator.cpp"
)


if(SIMPLE_RISCV_EMU_BUILD_APP)
    find_package(SDL3 CONFIG REQUIRED)
    find_package(argparse CONFIG REQUIRED)

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
endif()

if(SIMPLE_RISCV_EMU_BUILD_TESTS)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)

    add_executable(tests)

    target_link_libraries(tests
        PRIVATE riscv-emu-lib GTest::gtest GTest::gtest_main
    )

    target_sources(tests
        PRIVATE
            "tests/bit_utils.cpp"
            "tests/memory_utils.cpp"
            "tests/instruction_decoding.cpp"
    )

    add_test(NAME riscv-emu-gtest-suite COMMAND tests)
endif()
