# Simple 64-bit RISC-V Emulator

Features:
- full RV64G architecture
- ELF executable loading (without thread-local storage sections)
- graphical output through an in-memory framebuffer
- keyboard input
- simple and portable

This is a hobby project involving modern C++20, OOP, multi-threaded programming,
custom allocators, custom linker scripts, bare-metal programming,
CMake, vcpkg package management,
testing with Google Test, and GitHub Actions CI.

Milestones:
- [X] Graphical output using a framebuffer mapped into the emulator memory
- [X] Instruction decoding
- [X] ELF executable decoding
- [ ] `I`, `M`: basic integer arithmetic and multiplication instruction support
- [ ] `F`, `D`: float and double support
- [ ] `A`: atomic instruction support
- [ ] Syscalls
- [ ] Run DOOM on this

## Build & run

1. Install [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started),
    CMake 3.20+, a C++20 compiler, and SDL3 system dependencies.
2. Run:
    ```sh
    cmake -B ./build/ -D CMAKE_BUILD_TYPE=Release -D CMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
    cmake --build ./build/ --config Release
    cmake --install ./build/ --prefix ./
    ```
3. Run the emulator:
    ```sh
    ./bin/riscv-emu path/to/riscv-executable.elf
    ```

### Development setup

1. Run this once to set up the build:
    ```sh
    cmake -B ./build/ -D SIMPLE_RISCV_EMU_BUILD_TESTS=ON -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
    ```
2. Build:
    ```sh
    cmake --build ./build/
    ```
3. Run the emulator:
    ```sh
    ./build/riscv-emu path/to/riscv-executable.elf
    ```
4. Run the tests:
    ```sh
    ./build/tests
    ```
