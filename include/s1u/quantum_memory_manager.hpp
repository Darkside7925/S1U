#pragma once

#include "s1u/core.hpp"
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <random>

namespace S1U {

struct MemoryConfig {
    bool enable_quantum_effects = true;
    bool enable_numa_optimization = true;
    bool enable_cache_optimization = true;
    bool enable_memory_compression = true;
    bool enable_memory_encryption = false;
    bool enable_huge_pages = true;
    bool enable_prefaulting = true;
    bool enable_memory_locking = true;
    
    size_t initial_pool_size = 1073741824; // 1GB
    size_t quantum_memory_size = 268435456; // 256MB
    size_t cache_line_size = 64;
    size_t page_size = 4096;
    size_t huge_page_size = 2097152; // 2MB
    
    f64 fragmentation_threshold = 0.3;
    f64 compression_threshold = 0.8;
    f64 quantum_coherence_time = 1000.0; // milliseconds
    f64 decoherence_rate = 0.001;
    
    u32 max_allocation_retries = 3;
    u32 compaction_interval_seconds = 5;
    u32 numa_balancing_interval_seconds = 10;
    u32 prefetch_distance = 8;
};

enum MemoryFlags : u32 {
    MEMORY_FLAG_NONE = 0,
    MEMORY_FLAG_ZERO_MEMORY = 1 << 0,
    MEMORY_FLAG_CACHE_ALIGNED = 1 << 1,
    MEMORY_FLAG_HUGE_PAGE = 1 << 2,
    MEMORY_FLAG_LOCKED = 1 << 3,
    MEMORY_FLAG_NUMA_LOCAL = 1 << 4,
    MEMORY_FLAG_QUANTUM_ENTANGLED = 1 << 5,
    MEMORY_FLAG_COMPRESSED = 1 << 6,
    MEMORY_FLAG_ENCRYPTED = 1 << 7,
    MEMORY_FLAG_PREFAULT_PAGES = 1 << 8,
    MEMORY_FLAG_NO_SWAP = 1 << 9,
    MEMORY_FLAG_SHARED = 1 << 10,
    MEMORY_FLAG_ATOMIC_ACCESS = 1 << 11
};

enum class AllocatorType : u32 {
    SystemDefault = 0,
    PoolAllocator = 1,
    StackAllocator = 2,
    FreeListAllocator = 3,
    BuddyAllocator = 4,
    SlabAllocator = 5,
    QuantumAllocator = 6,
    NUMAAware = 7,
    CacheOptimized = 8
};

enum class AllocationStrategy : u32 {
    FirstFit = 0,
    BestFit = 1,
    WorstFit = 2,
    NextFit = 3,
    QuickFit = 4,
    BuddySystem = 5,
    SegregatedFreeList = 6,
    QuantumCoherent = 7
};

enum class QuantumState : u32 {
    Superposition = 0,
    Collapsed = 1,
    Entangled = 2,
    Decoherent = 3,
    Measured = 4
};

enum class AccessPattern : u32 {
    Random = 0,
    Sequential = 1,
    Strided = 2,
    Hotspot = 3,
    Temporal = 4,
    Spatial = 5
};

struct MemoryPool {
    void* memory_start = nullptr;
    size_t block_size = 0;
    u32 total_blocks = 0;
    u32 free_blocks = 0;
    u32 allocated_blocks = 0;
    Vector<void*> free_list;
    Vector<u64> allocation_bitmap;
    bool is_locked = false;
    bool is_huge_page = false;
    u32 numa_node = 0;
    f64 utilization = 0.0;
    u64 allocation_count = 0;
    u64 deallocation_count = 0;
};

struct QuantumBlock {
    void* address = nullptr;
    size_t size = 0;
    QuantumState quantum_state = QuantumState::Superposition;
    u32 entanglement_partner = 0;
    f64 coherence_time = 1000.0;
    f32 phase = 0.0f;
    f32 amplitude = 1.0f;
    u32 measurement_count = 0;
    bool is_entangled = false;
    bool is_coherent = true;
    u64 last_access_time = 0;
    f32 quantum_fidelity = 1.0f;
};

struct CacheOptimizedRegion {
    void* memory = nullptr;
    size_t size = 0;
    size_t alignment = 64;
    AccessPattern access_pattern = AccessPattern::Sequential;
    u32 prefetch_distance = 8;
    bool is_hot = false;
    f64 access_frequency = 0.0;
    u64 last_access_time = 0;
    f32 cache_efficiency = 1.0f;
    u32 cache_level = 1;
};

struct NUMANode {
    u32 node_id = 0;
    bool is_available = false;
    size_t total_memory = 0;
    size_t free_memory = 0;
    size_t allocated_memory = 0;
    f64 utilization = 0.0;
    bool is_overloaded = false;
    bool is_underutilized = false;
    void* cpu_mask = nullptr;
    f64 access_latency = 0.0;
    f64 bandwidth = 0.0;
};

struct PrefetchHint {
    void* base_address = nullptr;
    size_t offset = 0;
    u32 locality = 3; // 0=NTA, 1=T2, 2=T1, 3=T0
    bool is_active = true;
    u64 creation_time = 0;
    f32 effectiveness = 0.0f;
    u32 usage_count = 0;
};

struct MemoryPattern {
    void* start_address = nullptr;
    size_t size = 0;
    AccessPattern pattern_type = AccessPattern::Random;
    f64 access_frequency = 0.0;
    f64 temporal_locality = 0.0;
    f64 spatial_locality = 0.0;
    Vector<u64> access_timestamps;
    bool is_predictable = false;
    f32 prediction_confidence = 0.0f;
};

struct HotSpot {
    void* address = nullptr;
    size_t size = 0;
    f32 temperature = 0.0f;
    f64 access_rate = 0.0;
    bool is_active = false;
    u64 first_access_time = 0;
    u64 last_access_time = 0;
    u32 access_count = 0;
    f32 heat_decay_rate = 0.1f;
};

struct MemoryPoolStatistics {
    size_t total_size = 0;
    size_t allocated_size = 0;
    size_t free_size = 0;
    f64 utilization_ratio = 0.0;
    f64 fragmentation_ratio = 0.0;
    u64 allocation_count = 0;
    u64 deallocation_count = 0;
    f64 average_allocation_size = 0.0;
    f64 allocation_rate = 0.0;
    f64 deallocation_rate = 0.0;
};

struct MemorySystemStatistics {
    size_t total_system_memory = 0;
    size_t available_memory = 0;
    size_t cached_memory = 0;
    size_t buffered_memory = 0;
    f64 memory_pressure = 0.0;
    u64 page_fault_count = 0;
    u64 major_page_faults = 0;
    u64 minor_page_faults = 0;
    f64 swap_usage = 0.0;
    f64 memory_bandwidth_utilization = 0.0;
};

struct MemoryStatistics {
    std::atomic<size_t> total_allocated{0};
    std::atomic<size_t> total_freed{0};
    std::atomic<size_t> peak_usage{0};
    std::atomic<size_t> current_usage{0};
    std::atomic<u64> allocation_count{0};
    std::atomic<u64> deallocation_count{0};
    std::atomic<f64> fragmentation_ratio{0.0};
    std::atomic<f64> cache_hit_rate{0.0};
    std::atomic<u64> page_fault_count{0};
    std::atomic<f32> quantum_coherence_ratio{0.0};
    std::atomic<f64> numa_efficiency{0.0};
    std::atomic<f64> compression_ratio{0.0};
    std::atomic<f64> access_locality_score{0.0};
    std::atomic<f64> memory_bandwidth_utilization{0.0};
    std::atomic<u64> quantum_entanglements{0};
    std::atomic<u64> quantum_measurements{0};
    std::atomic<f64> average_allocation_time{0.0};
    std::atomic<f64> average_deallocation_time{0.0};
    std::atomic<u32> active_pools{0};
    std::atomic<u32> numa_nodes{0};
    std::atomic<u64> cache_line_splits{0};
    std::atomic<u64> false_sharing_events{0};
};

class QuantumMemoryManager {
public:
    QuantumMemoryManager();
    ~QuantumMemoryManager();
    
    bool initialize(const MemoryConfig& config);
    void shutdown();
    
    void* allocate(size_t size, size_t alignment = 0, MemoryFlags flags = MEMORY_FLAG_NONE);
    void deallocate(void* ptr);
    void* reallocate(void* ptr, size_t new_size, size_t alignment = 0, MemoryFlags flags = MEMORY_FLAG_NONE);
    
    void* allocate_aligned(size_t size, size_t alignment);
    void* allocate_zeroed(size_t size, size_t alignment = 0);
    void* allocate_numa_local(size_t size, u32 node_id, size_t alignment = 0);
    void* allocate_quantum_entangled(size_t size, size_t alignment = 0);
    void* allocate_compressed(size_t size, size_t alignment = 0);
    void* allocate_huge_page(size_t size, size_t alignment = 0);
    
    void* allocate_from_pool(size_t size, AllocatorType allocator_type);
    void return_to_pool(void* ptr, AllocatorType allocator_type);
    
    bool resize_allocation(void* ptr, size_t new_size);
    bool move_allocation(void* ptr, u32 target_numa_node);
    bool lock_memory(void* ptr, size_t size);
    bool unlock_memory(void* ptr, size_t size);
    
    void prefault_memory(void* ptr, size_t size);
    void add_prefetch_hint(void* address, size_t offset, u32 locality);
    void remove_prefetch_hint(void* address);
    void update_access_pattern(void* ptr, AccessPattern pattern);
    
    void enable_quantum_entanglement(void* ptr1, void* ptr2);
    void disable_quantum_entanglement(void* ptr);
    QuantumState get_quantum_state(void* ptr);
    f32 measure_quantum_coherence(void* ptr);
    
    void set_allocator_strategy(AllocationStrategy strategy);
    void set_default_allocator(AllocatorType allocator);
    void set_numa_policy(u32 preferred_node);
    void enable_compression(bool enabled);
    void enable_encryption(bool enabled);
    
    void compact_memory();
    void defragment_pools();
    void balance_numa_allocation();
    void optimize_cache_usage();
    void garbage_collect();
    
    void enable_low_memory_mode(bool enabled);
    void set_memory_pressure_threshold(size_t threshold);
    void handle_out_of_memory();
    void emergency_cleanup();
    
    size_t get_allocation_size(void* ptr);
    u32 get_numa_node(void* ptr);
    bool is_quantum_entangled(void* ptr);
    bool is_memory_locked(void* ptr);
    f64 get_access_frequency(void* ptr);
    
    Vector<void*> get_allocations_in_range(void* start, void* end);
    Vector<void*> get_quantum_entangled_allocations(void* ptr);
    Vector<HotSpot> get_memory_hotspots();
    Vector<MemoryPattern> get_access_patterns();
    
    MemoryStatistics get_memory_statistics() const;
    Vector<MemoryPoolStatistics> get_pool_statistics() const;
    MemorySystemStatistics get_system_statistics() const;
    void reset_statistics();
    
    void enable_debug_mode(bool enabled);
    void dump_memory_map(const String& filename);
    void dump_quantum_state(const String& filename);
    void validate_memory_integrity();
    bool check_memory_leaks();
    
    size_t calculate_optimal_pool_size(size_t allocation_size);
    f64 predict_allocation_pattern(void* ptr);
    u32 recommend_numa_node(size_t size, AccessPattern pattern);
    f64 estimate_cache_efficiency(void* ptr, size_t size);
    
    void register_allocation_callback(void (*callback)(void*, size_t, bool));
    void register_oom_callback(void (*callback)(size_t));
    void register_quantum_decoherence_callback(void (*callback)(void*));
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    bool detect_system_capabilities();
    bool initialize_memory_pools();
    bool setup_numa_optimization();
    bool initialize_quantum_memory();
    bool setup_cache_optimization();
    bool initialize_compression_system();
    
    void start_background_threads();
    void stop_background_threads();
    void memory_compaction_loop();
    void quantum_coherence_loop();
    void memory_prefetch_loop();
    void numa_balancing_loop();
    
    void* allocate_quantum_memory(size_t size, size_t alignment);
    void deallocate_quantum_memory(void* ptr, size_t size);
    void apply_quantum_decoherence(QuantumBlock& block);
    void update_entanglement_state(QuantumBlock& block);
    void restore_quantum_entanglement(QuantumBlock& block);
    
    void* allocate_numa_memory(size_t size, size_t alignment, u32 node_id);
    void deallocate_numa_memory(void* ptr, size_t size);
    void* allocate_cache_aligned_memory(size_t size, size_t alignment);
    void deallocate_cache_aligned_memory(void* ptr, size_t size);
    
    void* allocate_from_pool(size_t size, size_t alignment);
    void deallocate_from_pool(void* ptr, size_t size);
    
    size_t align_size(size_t size, size_t alignment);
    u32 get_current_numa_node();
    u32 get_numa_node_for_address(void* ptr);
    void record_allocation(void* ptr, size_t size, MemoryFlags flags);
    
    void perform_memory_compaction();
    void compact_memory_pool(MemoryPool& pool);
    void update_fragmentation_metrics();
    void perform_emergency_cleanup();
    
    void maintain_quantum_coherence();
    void simulate_quantum_interference();
    void update_quantum_superposition();
    
    void analyze_access_patterns();
    void apply_intelligent_prefetching();
    void update_hotspot_detection();
    
    void balance_numa_allocation();
    void migrate_hot_pages();
    u32 find_optimal_numa_node();
    void update_numa_statistics();
    
    f64 calculate_numa_efficiency() const;
    
    void cleanup_quantum_memory();
    void cleanup_memory_pools();
    void cleanup_numa_resources();
    void cleanup_compression_system();
};

Vector<String> get_supported_allocator_types();
Vector<String> get_supported_allocation_strategies();
String format_memory_size(size_t size);
f64 calculate_memory_efficiency(const MemoryStatistics& stats);
bool is_power_of_two(size_t value);
size_t next_power_of_two(size_t value);
size_t align_to_page(size_t size, size_t page_size);

} // namespace S1U
