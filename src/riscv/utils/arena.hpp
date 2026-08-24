#ifndef RISCV_ARENA_HPP
#define RISCV_ARENA_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <span>
#include <utility>
#include <vector>

namespace riscv
{


class Arena
{
public:
    explicit Arena(size_t capacity) : storage(capacity), freeSpace(capacity)
    {
    }

    std::span<uint8_t> data()
    {
        return storage;
    }

    size_t getFreeSpace() const
    {
        return freeSpace;
    }

    size_t getFreeOffset() const
    {
        return storage.size() - freeSpace;
    }

    /** If there is not enough free space for the aligned offset, returns an unaligned offset. */
    template <typename AlignmentType>
    size_t getFreeAlignedOffset() const
    {
        auto free = freeSpace;

        // ptr is never dereferenced, so we can cast away constness here
        void *ptr = const_cast<uint8_t *>(storage.data()) + getFreeOffset();

        if (std::align(alignof(AlignmentType), sizeof(AlignmentType), ptr, free) == nullptr)
            return getFreeOffset();

        return static_cast<size_t>(static_cast<uint8_t *>(ptr) - storage.data());
    }

    template <typename Object, class... Args>
        requires(!std::is_array_v<Object>)
    Object *allocate(Args&&...args)
    {
        auto ptr = static_cast<Object *>(allocateRaw(sizeof(Object), alignof(Object)));
        return std::construct_at(ptr, std::forward<Args>(args)...);
    }

    template <typename Arr>
        requires(std::is_unbounded_array_v<Arr>)
    std::span<std::remove_extent_t<Arr>> allocate(size_t n)
    {
        using Item = std::remove_extent_t<Arr>;

        auto ptr = static_cast<Item *>(allocateRaw(sizeof(Item) * n, alignof(Item)));

        size_t i = 0;
        try
        {
            for (; i < n; i++)
                std::construct_at(&ptr[i]);
        }
        catch (...)
        {
            for (; i != 0; i--)
                std::destroy_at(&ptr[i]);

            throw;
        }

        return std::span<Item> { ptr, n };
    }

private:
    std::vector<uint8_t> storage;
    size_t freeSpace;

    void *allocateRaw(size_t size, size_t alignment)
    {
        void *ptr = storage.data() + getFreeOffset();

        if (std::align(alignment, size, ptr, freeSpace) == nullptr)
            throw std::bad_alloc {};

        return ptr;
    }
};


} // namespace riscv

#endif
