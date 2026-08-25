#include "options.hpp"

#include <argparse/argparse.hpp>

#include <stdexcept>
#include <vector>


Options Options::parse(std::span<char *> args)
{
    std::string programPath;
    std::vector<int32_t> framebufferSize; // argparse does not support vector<uint32_t>
    size_t memorySizeKiB;

    argparse::ArgumentParser parser("riscv-emu");

    parser.add_argument("program_path")
        .help("path to the RISC-V program binary to load")
        .store_into(programPath);

    parser.add_argument("-s", "--framebuffer-size")
        .help("framebuffer size (width height)")
        .scan<'d', int>()
        .nargs(2)
        .default_value(std::vector<int> { 640, 480 })
        .store_into(framebufferSize);

    parser.add_argument("-m", "--memory-size")
        .help("memory size (in KiB)")
        .scan<'i', size_t>()
        .default_value((size_t) 32 * 1024) /** 32 MiB */
        .store_into(memorySizeKiB);

    parser.parse_args((int) args.size(), args.data());

    if (memorySizeKiB == 0)
        throw std::runtime_error("memory size must be non-zero");

    if (framebufferSize[0] <= 0 || framebufferSize[1] <= 0)
        throw std::runtime_error("framebuffer size must be positive");

    return Options {
        .programPath = std::move(programPath),
        .framebufferWidth = (uint32_t) framebufferSize[0],
        .framebufferHeight = (uint32_t) framebufferSize[1],
        .memorySize = memorySizeKiB * 1024,
    };
}
