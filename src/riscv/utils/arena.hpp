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

    /** Gets raw data storage. */
    std::span<uint8_t> raw()
    {
        return storage;
    }

    size_t getFreeSpace() const
    {
        return freeSpace;
    }

    /** Gets the offset of the first free byte in the storage.
        If there is no free space, returns `raw().size()`. */
    size_t getFreeOffset() const
    {
        return storage.size() - freeSpace;
    }

    /** Gets an offset such that `raw().data() + offset` is aligned to `alignof(AlignmentType)`.
        If there is not enough free space for the aligned offset, returns `raw().size()`. */
    template <typename AlignmentType>
    size_t getFreeOffsetAlignedTo() const
    {
        auto free = freeSpace;

        // ptr is never dereferenced, so we can cast away constness here
        void *ptr = const_cast<uint8_t *>(storage.data()) + getFreeOffset();

        if (std::align(alignof(AlignmentType), sizeof(AlignmentType), ptr, free) == nullptr)
            return storage.size();

        return static_cast<size_t>(static_cast<uint8_t *>(ptr) - storage.data());
    }

    /** Allocates an object of type `Object` and constructs it with the given arguments.
        If there is not enough free space, throws `std::bad_alloc`. */
    template <typename Object, class... Args>
        requires(!std::is_array_v<Object>)
    Object *allocate(Args&&...args)
    {
        auto ptr = static_cast<Object *>(allocateRaw(sizeof(Object), alignof(Object)));
        return std::construct_at(ptr, std::forward<Args>(args)...);
    }

    /** Allocates an array of type `Arr` and default-constructs its elements.
        If there is not enough free space, throws `std::bad_alloc`. */
    template <typename Arr>
        requires(std::is_unbounded_array_v<Arr>)
    std::span<std::remove_extent_t<Arr>> allocate(size_t itemCount)
    {
        using Item = std::remove_extent_t<Arr>;

        auto ptr = static_cast<Item *>(allocateRaw(sizeof(Item) * itemCount, alignof(Item)));

        size_t i = 0;
        try
        {
            for (; i < itemCount; i++)
                std::construct_at(&ptr[i]);
        }
        catch (...)
        {
            for (; i != 0; i--)
                std::destroy_at(&ptr[i]);

            throw;
        }

        return std::span<Item> { ptr, itemCount };
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
