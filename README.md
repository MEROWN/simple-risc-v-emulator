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

1. Install [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started),
   CMake 3.20+, and a C++20-capable compiler toolchain.
   Ensure that the `VCPKG_ROOT` environment variable is defined and contains
   your vcpkg installation path.
2. Run:
   ```sh
   cmake -B ./build/ -D CMAKE_BUILD_TYPE=Release
   cmake --build ./build/ --config Release
   cmake --install ./build/ --prefix ./
   ```
3. Run the program:
   ```sh
   bin/riscv-emu
   ```

### Development setup

This applies for project development only!

```sh
cmake -B ./build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build ./build/
```

Then run the program in the `build` directory.

### Test setup

```sh
cmake -B ./build/ -D BUILD_TESTING=ON -D CMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build ./build/
```

Of course, `RelWithDebInfo` is not the only configuration you can or should use.

To run the tests:
```sh
ctest --test-dir ./build/ --output-on-failure
```
