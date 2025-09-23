#pragma once

#include "Types.h"
#include <atomic>
#include <memory>
#include <cstring>

/**
 * Lock-Free Single Producer Single Consumer Ring Buffer
 * 
 * Key features:
 * - Zero locks, uses atomic operations only
 * - Cache line aligned to prevent false sharing
 * - Power-of-2 sizing for fast modulo with bit masks
 * - Memory ordering optimized for performance
 * 
 * Usage:
 *   SPSCRing<Message, 1024> ring;
 *   
 *   // Producer thread:
 *   ring.try_emplace(message);
 *   
 *   // Consumer thread:
 *   Message msg;
 *   if (ring.try_pop(msg)) { ... }
 */
template<typename T, size_t Size>
class SPSCRing {
    // Ensure Size is power of 2 for bit-mask optimization
    static_assert((Size & (Size - 1)) == 0, "Size must be power of 2");
    static_assert(Size >= 2, "Size must be at least 2");
    
public:
    SPSCRing();
    ~SPSCRing() = default;
    
    // Non-copyable, non-movable (contains atomics)
    SPSCRing(const SPSCRing&) = delete;
    SPSCRing& operator=(const SPSCRing&) = delete;
    SPSCRing(SPSCRing&&) = delete;
    SPSCRing& operator=(SPSCRing&&) = delete;
    
    // Producer interface (single thread only)
    bool try_emplace(const T& item) noexcept;
    bool try_emplace(T&& item) noexcept;
    
    // Consumer interface (single thread only)
    bool try_pop(T& item) noexcept;
    
    // Status queries (can be called from any thread)
    bool empty() const noexcept;
    bool full() const noexcept;
    size_t size() const noexcept;
    size_t capacity() const noexcept { return Size - 1; }  // One slot reserved
    
    // Performance monitoring
    uint64_t total_pushes() const noexcept { return push_count_.load(std::memory_order_relaxed); }
    uint64_t total_pops() const noexcept { return pop_count_.load(std::memory_order_relaxed); }
    uint64_t failed_pushes() const noexcept { return failed_push_count_.load(std::memory_order_relaxed); }
    
private:
    // Cache line alignment prevents false sharing between producer and consumer
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> head_{0};     // Producer writes here
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> tail_{0};     // Consumer reads here
    alignas(CACHE_LINE_SIZE) T buffer_[Size];                  // The actual ring buffer
    
    // Performance counters (aligned to separate cache line)
    alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> push_count_{0};
    std::atomic<uint64_t> pop_count_{0};
    std::atomic<uint64_t> failed_push_count_{0};
    
    // Bit mask for fast modulo operation
    static constexpr size_t MASK = Size - 1;
    
    // Helper functions
    size_t next_index(size_t current) const noexcept {
        return (current + 1) & MASK;
    }
};

// ============================================================================
// IMPLEMENTATION
// ============================================================================

template<typename T, size_t Size>
SPSCRing<T, Size>::SPSCRing() {
    // Initialize buffer with default-constructed objects
    for (size_t i = 0; i < Size; ++i) {
        new (&buffer_[i]) T{};
    }
}

template<typename T, size_t Size>
bool SPSCRing<T, Size>::try_emplace(const T& item) noexcept {
    // Load current head position (where we want to write)
    const size_t current_head = head_.load(std::memory_order_relaxed);
    const size_t next_head = next_index(current_head);
    
    // Check if buffer is full by comparing with tail
    // We need one empty slot to distinguish full from empty
    if (UNLIKELY(next_head == tail_.load(std::memory_order_acquire))) {
        failed_push_count_.fetch_add(1, std::memory_order_relaxed);
        return false;  // Buffer full
    }
    
    // Write the item to the buffer
    buffer_[current_head] = item;
    
    // Update head pointer - this makes the item visible to consumer
    // Use release semantics to ensure the write above happens before this
    head_.store(next_head, std::memory_order_release);
    
    push_count_.fetch_add(1, std::memory_order_relaxed);
    return true;
}

template<typename T, size_t Size>
bool SPSCRing<T, Size>::try_emplace(T&& item) noexcept {
    const size_t current_head = head_.load(std::memory_order_relaxed);
    const size_t next_head = next_index(current_head);
    
    if (UNLIKELY(next_head == tail_.load(std::memory_order_acquire))) {
        failed_push_count_.fetch_add(1, std::memory_order_relaxed);
        return false;  // Buffer full
    }
    
    // Move the item into the buffer
    buffer_[current_head] = std::move(item);
    
    head_.store(next_head, std::memory_order_release);
    push_count_.fetch_add(1, std::memory_order_relaxed);
    return true;
}

template<typename T, size_t Size>
bool SPSCRing<T, Size>::try_pop(T& item) noexcept {
    // Load current tail position (where we read from)
    const size_t current_tail = tail_.load(std::memory_order_relaxed);
    
    // Check if buffer is empty
    if (UNLIKELY(current_tail == head_.load(std::memory_order_acquire))) {
        return false;  // Buffer empty
    }
    
    // Read the item from the buffer
    item = std::move(buffer_[current_tail]);
    
    // Update tail pointer - this frees up the slot for producer
    // Use release semantics to ensure the read above happens before this
    const size_t next_tail = next_index(current_tail);
    tail_.store(next_tail, std::memory_order_release);
    
    pop_count_.fetch_add(1, std::memory_order_relaxed);
    return true;
}

template<typename T, size_t Size>
bool SPSCRing<T, Size>::empty() const noexcept {
    return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
}

template<typename T, size_t Size>
bool SPSCRing<T, Size>::full() const noexcept {
    const size_t head = head_.load(std::memory_order_acquire);
    const size_t tail = tail_.load(std::memory_order_acquire);
    return next_index(head) == tail;
}

template<typename T, size_t Size>
size_t SPSCRing<T, Size>::size() const noexcept {
    const size_t head = head_.load(std::memory_order_acquire);
    const size_t tail = tail_.load(std::memory_order_acquire);
    return (head - tail) & MASK;
}