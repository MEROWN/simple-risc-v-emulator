#ifndef OPTIONS_HPP
#define OPTIONS_HPP


#include <cstdint>
#include <span>
#include <string>


struct Options
{
    std::string programPath;
    uint32_t framebufferWidth;
    uint32_t framebufferHeight;
    size_t memorySize;

    static Options parse(std::span<char *> args);
};


#endif
