#ifndef OPTIONS_HPP
#define OPTIONS_HPP


#include <cstdint>
#include <span>
#include <string>

#include <src/riscv/riscv.hpp>


struct Options
{
    std::string programPath;
    uint32_t framebufferWidth;
    uint32_t framebufferHeight;
    riscv::Size freeMemorySize;

    static Options parse(std::span<char *> args);
};


#endif
