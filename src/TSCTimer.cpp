#include "TSCTimer.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

TSCTimer::TSCTimer() : tsc_freq_ghz_(0.0) {
    if (!is_tsc_available()) {
        std::cerr << "Warning: TSC not available or unreliable on this system\n";
    }
    calibrate();
}

bool TSCTimer::is_tsc_available() {
    uint32_t eax, ebx, ecx, edx;
    
    #ifdef _WIN32
        int cpuInfo[4];
        __cpuid(cpuInfo, 1);
        edx = cpuInfo[3];
    #else
        __asm__ __volatile__(
            "cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(1)
        );
    #endif
    
    return (edx & (1 << 4)) != 0;  // Check TSC bit
}

void TSCTimer::calibrate() {
    std::cout << "Calibrating TSC timer...\n";
    warmup_tsc();
    
    constexpr int num_measurements = 3;
    uint64_t frequencies[num_measurements];
    
    for (int i = 0; i < num_measurements; ++i) {
        frequencies[i] = measure_tsc_frequency();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::sort(frequencies, frequencies + num_measurements);
    uint64_t median_freq = frequencies[num_measurements / 2];
    
    tsc_freq_ghz_ = static_cast<double>(median_freq) / 1e9;
    
    std::cout << "TSC frequency: " << std::fixed << std::setprecision(3) 
              << tsc_freq_ghz_ << " GHz\n";
}

uint64_t TSCTimer::measure_tsc_frequency() {
    auto start_time = std::chrono::high_resolution_clock::now();
    uint64_t start_tsc = now();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    uint64_t end_tsc = now();
    auto end_time = std::chrono::high_resolution_clock::now();
    
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        end_time - start_time).count();
    
    uint64_t tsc_cycles = end_tsc - start_tsc;
    
    return static_cast<uint64_t>(
        static_cast<double>(tsc_cycles) * 1e9 / duration_ns);
}

void TSCTimer::warmup_tsc() {
    volatile uint64_t dummy = 0;
    for (int i = 0; i < 1000; ++i) {
        dummy += now();
    }
    (void)dummy;
}

TSCTimer ScopedTimer::timer_;

ScopedTimer::~ScopedTimer() {
    uint64_t end_cycles = timer_.now();
    double elapsed_ns = timer_.cycles_to_ns(end_cycles - start_cycles_);
    
    std::cout << "[TIMER] " << name_ << ": " 
              << std::fixed << std::setprecision(2) << elapsed_ns << " ns\n";
}