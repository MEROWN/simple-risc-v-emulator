#include "elf.hpp"

#include <src/riscv/emulator.hpp>
#include <src/riscv/instruction.hpp>
#include <src/riscv/utils/memory.hpp>

#include <algorithm>
#include <cstring>


namespace riscv
{


struct ElfHeader
{
    uint32_t magic = 0;
    uint8_t elfClass = 0;
    uint8_t data = 0;
    uint8_t version = 0;
    uint8_t osABI = 0;
    uint16_t type = 0;
    uint16_t machine = 0;
    uint64_t entry = 0;
    uint64_t programHeaderOffset = 0;
    uint16_t programHeaderEntrySize = 0;
    uint16_t programHeaderEntryCount = 0;

    void read(std::span<uint8_t const> source)
    {
        if (source.size() < elfHeaderSize)
            throw std::runtime_error { "ELF file too small" };

        auto header = source.data();

        magic = loadU32(header + 0);
        elfClass = *(header + 4);
        data = *(header + 5);
        version = *(header + 6);
        osABI = *(header + 7);
        type = loadU16(header + 16);
        machine = loadU16(header + 18);
        entry = loadU64(header + 24);
        programHeaderOffset = loadU64(header + 32);
        programHeaderEntrySize = loadU16(header + 54);
        programHeaderEntryCount = loadU16(header + 56);

        validate(source);
    }

private:
    static constexpr size_t elfHeaderSize = 64;

    static constexpr uint32_t elfMagic = 0x464c457f; // "\x7fELF", little-endian
    static constexpr uint8_t elfClass64 = 2;
    static constexpr uint8_t elfDataLittleEndian = 1;
    static constexpr uint8_t elfVersion1 = 1;
    static constexpr uint8_t osABISystemV = 0;
    static constexpr uint8_t elfTypeExecutable = 2;
    static constexpr uint16_t elfMachineRISCV = 243;
    static constexpr uint64_t entrypointStart = 0;
    static constexpr uint16_t elfProgramHeaderEntrySize64 = 56;

    void validate(std::span<uint8_t const> source) const
    {
        if (!(magic == elfMagic && elfClass == elfClass64 && data == elfDataLittleEndian
              && version == elfVersion1 && osABI == osABISystemV && type == elfTypeExecutable
              && machine == elfMachineRISCV
              && programHeaderEntrySize == elfProgramHeaderEntrySize64))
        {
            throw std::runtime_error { "invalid ELF executable header" };
        }

        if (entry != entrypointStart)
        {
            throw std::runtime_error { "ELF executable entry point must be 0" };
        }

        if (programHeaderOffset + (size_t) programHeaderEntryCount * programHeaderEntrySize
            > source.size())
        {
            throw std::runtime_error { "ELF program headers are out of file bounds" };
        }
    }
};


struct SegmentHeader
{
    uint32_t type = 0;
    uint32_t flags = 0;
    uint64_t sourceStart = 0;
    uint64_t destinationStart = 0;
    uint64_t sourceSize = 0;
    uint64_t destinationSize = 0;

    static constexpr uint32_t typeLoad = 1;
    static constexpr uint32_t flagExecutable = 1;

    size_t sourceEnd() const
    {
        return sourceStart + sourceSize;
    }

    size_t destinationEnd() const
    {
        return destinationStart + destinationSize;
    }

    void read(std::span<uint8_t const> source, size_t offset)
    {
        auto header = source.data() + offset;

        type = loadU32(header + 0);
        flags = loadU32(header + 4);
        sourceStart = loadU64(header + 8);
        destinationStart = loadU64(header + 16);
        sourceSize = loadU64(header + 32);
        destinationSize = loadU64(header + 40);
    }
};

/** Sorted by destinationStart, ascending */
using SortedHeaders = std::span<SegmentHeader const>;


static void readSegmentHeaders(
    ElfHeader const& elfHeader,
    std::span<uint8_t const> source,
    std::vector<SegmentHeader>& dataHeaders,
    std::vector<SegmentHeader>& codeHeaders
)
{
    for (size_t i = 0; i < elfHeader.programHeaderEntryCount; i++)
    {
        size_t offset = elfHeader.programHeaderOffset + i * elfHeader.programHeaderEntrySize;

        SegmentHeader header {};
        header.read(source, offset);

        if (header.type != SegmentHeader::typeLoad)
            continue;

        if (header.sourceSize == 0 && header.destinationSize == 0)
            continue;

        if (header.sourceEnd() > source.size())
            throw std::runtime_error { "ELF segment is out of file bounds" };

        if (header.flags & SegmentHeader::flagExecutable)
            codeHeaders.push_back(header);
        else
            dataHeaders.push_back(header);
    }
}

static void sortHeadersBySegmentAddresses(std::span<SegmentHeader> headers)
{
    auto cmp = [](auto a, auto b) { return a.destinationStart < b.destinationStart; };
    std::sort(headers.begin(), headers.end(), cmp);
}

static void validateDataHeaders(SortedHeaders headers)
{
    for (auto const& header : headers)
    {
        if (header.sourceSize > header.destinationSize)
            throw std::runtime_error {
                "ELF data segments must not be truncated when loaded into memory"
            };
    }

    for (size_t i = 0, next = 1; next < headers.size(); i++, next++)
    {
        if (headers[i].destinationEnd() > headers[next].destinationStart)
            throw std::runtime_error { "ELF data segments must not overlap" };
    }
}

static void validateCodeHeaders(SortedHeaders headers)
{
    if (headers.empty())
        throw std::runtime_error { "ELF executable contains no code segments" };

    for (auto const& header : headers)
    {
        if (header.destinationSize != header.sourceSize)
            throw std::runtime_error {
                "ELF code segments must not be truncated or padded when loaded into memory"
            };
    }

    if (headers[0].destinationStart != 0)
        throw std::runtime_error {
            "ELF code segments must be placed into memory starting at offset 0"
        };

    for (size_t i = 0, next = 1; next < headers.size(); i++, next++)
    {
        if (headers[i].destinationEnd() != headers[next].destinationStart)
            throw std::runtime_error { "ELF code segments must be contiguous" };
    }
}


static void copySegments(
    SortedHeaders headers, std::span<uint8_t const> source, std::span<uint8_t> destination
)
{
    for (auto const& header : headers)
    {
        std::memcpy(
            destination.data() + header.destinationStart,
            source.data() + header.sourceStart,
            header.sourceSize
        );
    }
}


static void mapDataSegments(
    SortedHeaders headers, std::span<uint8_t const> source, Emulator& emulator
)
{
    if (headers.empty())
        return;

    size_t totalDataSize = headers.back().destinationEnd();

    std::span<uint8_t> dataMemory;
    try
    {
        dataMemory = emulator.memory.allocate<uint8_t[]>(totalDataSize);
    }
    catch (std::bad_alloc const&)
    {
        throw std::runtime_error { "could not allocate emulator memory to load ELF data segments" };
    }

    copySegments(headers, source, dataMemory);
}


static void mapCodeSegments(
    SortedHeaders headers, std::span<uint8_t const> source, Emulator& emulator
)
{
    auto totalCodeSize = headers.back().destinationEnd();

    std::vector<uint8_t> codeMemory(totalCodeSize);

    copySegments(headers, source, codeMemory);

    emulator.setInstructions(decodeInstructions(codeMemory));
}


void loadElf(std::span<uint8_t const> source, Emulator& emulator)
{
    ElfHeader elfHeader {};
    elfHeader.read(source);

    std::vector<SegmentHeader> dataHeaders {};
    std::vector<SegmentHeader> codeHeaders {};
    readSegmentHeaders(elfHeader, source, dataHeaders, codeHeaders);

    sortHeadersBySegmentAddresses(dataHeaders);
    sortHeadersBySegmentAddresses(codeHeaders);

    validateDataHeaders(dataHeaders);
    validateCodeHeaders(codeHeaders);

    mapDataSegments(dataHeaders, source, emulator);
    mapCodeSegments(codeHeaders, source, emulator);
}


} // namespace riscv
