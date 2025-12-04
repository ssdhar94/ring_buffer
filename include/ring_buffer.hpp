// ringbuffer.h
#pragma once
#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>

template<typename T, size_t Capacity>
class ring_buffer {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2 as bitwise operation used for increment");

public:
    constexpr ring_buffer() = default;

    // Producer only
    bool push(const T& value) {
        const auto write_idx = write_index.load(std::memory_order_relaxed);
        const auto next_write = (write_idx + 1) & mask;//bitwise and operation. Performs same as below but faster.
        // const auto next_write = (write_idx + 1) % Capacity; // modulo operation to reset to 0 like a ring at the end.
        
        if (next_write == read_index.load(std::memory_order_acquire))
            return false; // Full

        buffer[write_idx] = value;
        write_index.store(next_write, std::memory_order_release);
        return true;
    }

    // Consumer only
    bool pop(T& value) {
        const auto read_idx = read_index.load(std::memory_order_relaxed);
        
        if (read_idx == write_index.load(std::memory_order_acquire))
            return false; // Empty

        value = buffer[read_idx];
        read_index.store((read_idx + 1) & mask, std::memory_order_release); //modulo operation can also be used
        return true;
    }

    // bool empty() const {
    //     return read_index.load(std::memory_order_acquire) == 
    //            write_index.load(std::memory_order_acquire);
    // }

    // bool full() const {
    //     const auto next_write = (write_index.load(std::memory_order_acquire) + 1) & mask;
    //     return next_write == read_index.load(std::memory_order_acquire);
    // }

    // size_t size() const {
    //     const auto r = read_index.load(std::memory_order_acquire);
    //     const auto w = write_index.load(std::memory_order_acquire);
    //     return (w - r) & mask;
    // }

private:
    static constexpr size_t mask = Capacity - 1;

    alignas(64) std::array<T, Capacity> buffer{};  // Prevent false sharing
    alignas(64) std::atomic<size_t> write_index{0};
    alignas(64) std::atomic<size_t> read_index{0};
};