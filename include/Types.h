#pragma once

#include <cstdint>
#include <string>
#include <chrono>
#include <atomic>

// ============================================================================
// BASIC TYPES
// ============================================================================

using Price = uint32_t;        // Price in ticks (e.g., $100.25 = 10025)
using Quantity = uint32_t;     // Share quantity
using OrderId = uint64_t;      // Unique order identifier
using Timestamp = uint64_t;    // Nanoseconds since epoch or TSC cycles
using SymbolId = uint16_t;     // Numeric symbol identifier for performance

// ============================================================================
// ENUMERATIONS  
// ============================================================================

enum class Side : uint8_t {
    BUY = 0,
    SELL = 1
};

enum class MessageType : uint8_t {
    ADD_ORDER = 1,
    CANCEL_ORDER = 2,
    MODIFY_ORDER = 3,
    EXECUTE_ORDER = 4,
    TRADE = 5,
    HEARTBEAT = 6
};

// ============================================================================
// CONFIGURATION CONSTANTS
// ============================================================================

namespace Config {
    // Ring buffer sizes (must be power of 2)
    constexpr size_t MESSAGE_RING_SIZE = 65536;
    constexpr size_t OUTPUT_RING_SIZE = 32768;
    
    // Memory pool sizes
    constexpr size_t MAX_ORDERS = 1000000;
    constexpr size_t MAX_SYMBOLS = 1000;
    
    // Price level array size
    constexpr size_t MAX_PRICE_LEVELS = 65536;
    constexpr Price MIN_PRICE = 1;
    constexpr Price MAX_PRICE = MAX_PRICE_LEVELS;
    
    // Performance monitoring
    constexpr size_t LATENCY_SAMPLE_SIZE = 1000000;
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

inline Price price_to_ticks(double price_dollars, double tick_size = 0.01) {
    return static_cast<Price>(price_dollars / tick_size + 0.5);
}

inline double ticks_to_price(Price ticks, double tick_size = 0.01) {
    return ticks * tick_size;
}

constexpr bool is_power_of_2(size_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// Compiler hints for branch prediction
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

// Cache line size for alignment
constexpr size_t CACHE_LINE_SIZE = 64;
#define CACHE_ALIGNED alignas(CACHE_LINE_SIZE)
