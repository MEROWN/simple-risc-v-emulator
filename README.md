# Simple 64-bit RISC-V Emulator

A hobby project in C++20 involving multi-threaded programming,
custom allocators, OOP, testing, and CI/CD.

Milestones:
- [ ] `I`: basic integer arithmetic and `M`: multiplication instructions
- [ ] Graphical output through DMA
- [ ] `A`: atomic instruction support
- [ ] `FD`: float and double support
- [ ] Run DOOM

## Build & run

1. Install [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started),
    CMake 3.20+, and a C++20 compiler.
2. Run:
    ```sh
    cmake -B ./build/ -D CMAKE_BUILD_TYPE=Release -D CMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
    cmake --build ./build/ --config Release
    cmake --install ./build/ --prefix ./
    ```
3. Run the program:
    ```sh
    bin/riscv-emu path/to/program
    ```

### Development setup

1. Run this once to set up the build:
    ```sh
    cmake -B ./build/  -D SIMPLE_RISCV_EMU_BUILD_TESTS=ON -D CMAKE_BUILD_TYPE=Debug -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
    ```
2. Build:
    ```sh
    cmake --build ./build/
    ```
3. Run the app:
    ```sh
    build/riscv-emu path/to/program
    ```
4. Run the tests:
    ```sh
    build/tests
    ```
