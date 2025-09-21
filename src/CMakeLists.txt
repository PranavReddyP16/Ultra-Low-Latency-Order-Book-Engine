# Core library source files
set(CORE_SOURCES
    Types.cpp
    TSCTimer.cpp
)

# Create static library for core functionality
add_library(hft_core STATIC ${CORE_SOURCES})
target_link_libraries(hft_core Threads::Threads)

# Main executable
add_executable(orderbook_engine main.cpp)
target_link_libraries(orderbook_engine hft_core)

# Timer test executable
add_executable(test_timer test_timer.cpp)
target_link_libraries(test_timer hft_core)
