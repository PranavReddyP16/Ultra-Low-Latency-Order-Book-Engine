#pragma once

#include <cstdint>
#include <chrono>
#include <thread>
#include <atomic>

#ifdef _WIN32
    #include <intrin.h>
    #define rdtscp __rdtscp
#else
    #include <x86intrin.h>
    static inline uint64_t rdtscp(uint32_t* aux) {
        return __builtin_ia32_rdtscp(aux);
    }
#endif

class TSCTimer {
public:
    TSCTimer();
    
    uint64_t now() const noexcept {
        uint32_t aux;
        return rdtscp(&aux);
    }
    
    double cycles_to_ns(uint64_t cycles) const noexcept {
        return static_cast<double>(cycles) / tsc_freq_ghz_;
    }
    
    double get_frequency_ghz() const noexcept {
        return tsc_freq_ghz_;
    }
    
    static bool is_tsc_available();
    void calibrate();
    
private:
    double tsc_freq_ghz_;
    uint64_t measure_tsc_frequency();
    void warmup_tsc();
};

class ScopedTimer {
public:
    explicit ScopedTimer(const char* name);
    ~ScopedTimer();
    
    uint64_t elapsed_cycles() const noexcept;
    double elapsed_ns() const noexcept;
    
private:
    const char* name_;
    uint64_t start_cycles_;
    static TSCTimer timer_;
};

inline ScopedTimer::ScopedTimer(const char* name) 
    : name_(name), start_cycles_(timer_.now()) {}

inline uint64_t ScopedTimer::elapsed_cycles() const noexcept {
    return timer_.now() - start_cycles_;
}

inline double ScopedTimer::elapsed_ns() const noexcept {
    return timer_.cycles_to_ns(elapsed_cycles());
}