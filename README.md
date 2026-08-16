# Simple 64-bit RISC-V Emulator

A hobby project in C++20 involving multi-threaded programming, OOP, testing,
and CI/CD.

Milestones:
- [ ] `I`: basic integer arithmetic and `M`: multiplication instructions
- [ ] Graphical output through DMA
- [ ] `A`: atomic instruction support
- [ ] `FD`: float and double support
- [ ] Run DOOM

## Build & run

1. Install [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell), CMake 3.20+, and a C++20-capable compiler toolchain
2. Run:
```sh
vcpkg install
cmake -B build
cmake --build build
cmake --install build --prefix . --config Release
```
3. Run the program:
```sh
bin/risv-emu
```

### Development setup

This applies for project development only!

```sh
cmake -B build -D CMAKE_BUILD_TYPE=Release -D CMAKE_EXPORT_COMPILE_COMMANDS=ON
```
