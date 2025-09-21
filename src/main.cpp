#include "Types.h"
#include "TSCTimer.h"
#include <iostream>

int main() {
    std::cout << "HFT Order Book Engine v1.0\n";
    std::cout << "==========================\n\n";
    
    // Test basic type sizes
    std::cout << "Type Sizes (bytes):\n";
    std::cout << "Price:              " << sizeof(Price) << "\n";
    std::cout << "Quantity:           " << sizeof(Quantity) << "\n";
    std::cout << "OrderId:            " << sizeof(OrderId) << "\n";
    std::cout << "\n";
    
    // Test configuration constants
    std::cout << "Configuration:\n";
    std::cout << "Ring Buffer Size:   " << Config::MESSAGE_RING_SIZE << "\n";
    std::cout << "Max Orders:         " << Config::MAX_ORDERS << "\n";
    std::cout << "Cache Line Size:    " << CACHE_LINE_SIZE << " bytes\n";
    std::cout << "\n";
    
    // Test price conversion
    Price test_price = price_to_ticks(100.25);
    double back_to_double = ticks_to_price(test_price);
    std::cout << "Price Conversion Test:\n";
    std::cout << "$100.25 -> " << test_price << " ticks -> $" 
              << std::fixed << std::setprecision(2) << back_to_double << "\n";
    std::cout << "\n";
    
    // Test TSC timer
    if (TSCTimer::is_tsc_available()) {
        std::cout << "TSC Timer Test:\n";
        TSCTimer timer;
        
        uint64_t start = timer.now();
        volatile int sum = 0;
        for (int i = 0; i < 1000; ++i) {
            sum += i;
        }
        uint64_t end = timer.now();
        
        std::cout << "1000 iterations: " << timer.cycles_to_ns(end - start) << " ns\n";
    } else {
        std::cout << "TSC not available on this system\n";
    }
    
    std::cout << "\n✅ Basic setup complete!\n";
    return 0;
}