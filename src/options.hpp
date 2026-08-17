#ifndef OPTIONS_HPP
#define OPTIONS_HPP


#include <cstdint>
#include <span>
#include <string>


struct Options
{
    std::string programPath;
    int framebufferWidth;
    int framebufferHeight;
    uint64_t freeMemorySize;

    static Options parse(std::span<char *> args);
};


#endif
