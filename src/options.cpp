#include <src/options.hpp>

#include <argparse/argparse.hpp>

#include <stdexcept>
#include <vector>


Options Options::parse(std::span<char *> args)
{
    std::string programPath;
    std::vector<int> framebufferSize;
    uint64_t freeMemorySizeKiB;

    argparse::ArgumentParser parser("riscv-emu");

    parser.add_argument("program_path")
        .help("path to the RISC-V program binary to load")
        .store_into(programPath);

    parser.add_argument("-s", "--size")
        .help("framebuffer size (width height)")
        .scan<'d', int>()
        .nargs(2)
        .default_value(std::vector<int> { 640, 480 })
        .store_into(framebufferSize);

    parser.add_argument("-m", "--memory-size")
        .help("memory size (in KiB)")
        .scan<'i', uint64_t>()
        .default_value((uint64_t) 16 * 1024) // 16 MiB
        .store_into(freeMemorySizeKiB);

    parser.parse_args((int) args.size(), args.data());

    if (freeMemorySizeKiB == 0)
        throw std::runtime_error("memory size must be non-zero");

    if (framebufferSize[0] <= 0 || framebufferSize[1] <= 0)
        throw std::runtime_error("framebuffer size must be positive");

    return Options {
        .programPath = std::move(programPath),
        .framebufferWidth = framebufferSize[0],
        .framebufferHeight = framebufferSize[1],
        .freeMemorySize = freeMemorySizeKiB * 1024,
    };
}
