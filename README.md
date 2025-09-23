# Ultra-Low Latency Order Book Engine

High-performance order book reconstruction engine achieving sub-microsecond message processing latency.

## Build Instructions
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)