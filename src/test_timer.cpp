#include "TSCTimer.h"
#include <iostream>

void test_basic_timing() {
    std::cout << "=== Basic Timing Test ===\n";
    
    TSCTimer timer;
    uint64_t start = timer.now();
    
    volatile int sum = 0;
    for (int i = 0; i < 1000; ++i) {
        sum += i * i;
    }
    
    uint64_t end = timer.now();
    std::cout << "1000 iterations: " << timer.cycles_to_ns(end - start) << " ns\n\n";
}

void test_scoped_timer() {
    std::cout << "=== Scoped Timer Test ===\n";
    
    {
        ScopedTimer timer("computation_test");
        volatile double result = 0;
        for (int i = 0; i < 10000; ++i) {
            result += i * 0.5;
        }
    }
    std::cout << "\n";
}

int main() {
    std::cout << "TSC Timer Tests\n";
    std::cout << "===============\n\n";
    
    if (!TSCTimer::is_tsc_available()) {
        std::cout << "❌ TSC not available!\n";
        return 1;
    }
    
    test_basic_timing();
    test_scoped_timer();
    
    std::cout << "✅ All timer tests passed!\n";
    return 0;
}