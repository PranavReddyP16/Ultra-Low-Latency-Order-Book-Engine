# Test 1: Check CPU features
echo "=== CPU Features ==="
grep -i tsc /proc/cpuinfo | head -5
echo ""

# Test 2: Check memory
echo "=== Memory Info ==="
free -h
echo ""

# Test 3: Check NUMA
echo "=== NUMA Topology ==="
numactl --hardware 2>/dev/null || echo "NUMA not available (single node system)"
echo ""

# Test 4: Test TSC timing
echo "=== TSC Test ==="
cat > /tmp/test_tsc.cpp << 'EOF'
#include <iostream>
#include <x86intrin.h>

int main() {
    uint32_t aux;
    uint64_t start = __builtin_ia32_rdtscp(&aux);
    
    volatile int sum = 0;
    for (int i = 0; i < 1000; ++i) {
        sum += i;
    }
    
    uint64_t end = __builtin_ia32_rdtscp(&aux);
    
    std::cout << "TSC timing works! Cycles: " << (end - start) << std::endl;
    return 0;
}
EOF

g++ -O2 /tmp/test_tsc.cpp -o /tmp/test_tsc && /tmp/test_tsc
echo ""

echo "✅ WSL2 setup verification complete!"
