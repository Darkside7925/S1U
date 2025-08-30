//
// S1U Extreme Performance Optimizer Header
// Ultra-high performance optimizations for both RTX and GTX GPUs
//

#pragma once

#include "s1u/core.hpp"
#include <memory>
#include <atomic>
#include <vector>
#include <thread>

namespace S1U {

struct PerformanceConfig {
    bool extreme_mode = true;
    bool enable_simd = true;
    bool enable_cache_optimization = true;
    bool enable_memory_prefetching = true;
    bool enable_branch_prediction = true;
    bool enable_loop_unrolling = true;
    bool enable_cpu_affinity = true;
    bool enable_real_time_priority = true;
    bool enable_numa_optimization = true;
    bool enable_thermal_management = true;
    
    // Performance targets
    u32 target_fps = 240;
    f32 target_frame_time_ms = 4.16f;
    f32 cpu_utilization_target = 80.0f;
    f32 memory_utilization_target = 75.0f;
    
    // CPU optimization
    u32 cpu_affinity_mask = 0xFFFFFFFF; // Use all cores by default
    i32 process_priority = -10; // High priority
    bool isolate_cpu_cores = false;
    u32 isolated_core_mask = 0;
    
    // Memory optimization
    size_t memory_pool_size_mb = 512;
    bool enable_huge_pages = true;
    bool lock_memory_pages = true;
    bool enable_memory_compression = false;
    
    // Cache optimization
    bool optimize_for_l1_cache = true;
    bool optimize_for_l2_cache = true;
    bool optimize_for_l3_cache = true;
    size_t cache_line_size = 64;
    
    // SIMD optimization
    bool prefer_avx512 = true;
    bool prefer_avx2 = true;
    bool enable_fma = true;
    bool vectorize_loops = true;
    
    // GPU optimization
    bool optimize_for_rtx = true;
    bool optimize_for_gtx = true;
    bool enable_gpu_scheduling = true;
    bool enable_memory_bandwidth_optimization = true;
};

struct CPUInfo {
    u32 core_count = 0;
    u32 thread_count = 0;
    u32 l1_cache_size = 0;
    u32 l2_cache_size = 0;
    u32 l3_cache_size = 0;
    u32 cache_line_size = 64;
    bool supports_sse = false;
    bool supports_sse2 = false;
    bool supports_sse3 = false;
    bool supports_ssse3 = false;
    bool supports_sse41 = false;
    bool supports_sse42 = false;
    bool supports_avx = false;
    bool supports_avx2 = false;
    bool supports_avx512 = false;
    bool supports_fma = false;
    String model_name;
    u32 base_frequency_mhz = 0;
    u32 max_frequency_mhz = 0;
};

struct GPUInfo {
    String name;
    bool is_nvidia = false;
    bool is_amd = false;
    bool is_intel = false;
    bool is_rtx = false;
    bool is_gtx = false;
    u32 compute_units = 0;
    u32 memory_mb = 0;
    u32 memory_bandwidth_gbps = 0;
    u32 base_clock_mhz = 0;
    u32 boost_clock_mhz = 0;
    u32 memory_clock_mhz = 0;
    bool supports_ray_tracing = false;
    bool supports_dlss = false;
    bool supports_mesh_shaders = false;
    bool supports_variable_rate_shading = false;
};

struct MemoryInfo {
    u32 total_gb = 0;
    u32 available_gb = 0;
    u32 numa_nodes = 1;
    bool supports_numa = false;
    bool supports_huge_pages = false;
    u32 page_size_kb = 4;
    u32 huge_page_size_mb = 2;
    f64 bandwidth_gbps = 0.0;
    u32 frequency_mhz = 0;
    String type; // DDR4, DDR5, etc.
};

struct PerformanceStats {
    std::atomic<f64> cpu_utilization{0.0};
    std::atomic<f64> gpu_utilization{0.0};
    std::atomic<f64> memory_utilization{0.0};
    std::atomic<f64> frame_time_ms{0.0};
    std::atomic<f64> avg_fps{0.0};
    std::atomic<f64> min_fps{999.0};
    std::atomic<f64> max_fps{0.0};
    std::atomic<u64> frames_rendered{0};
    std::atomic<f64> cache_hit_rate{0.0};
    std::atomic<f64> branch_prediction_rate{0.0};
    std::atomic<f64> memory_bandwidth_utilization{0.0};
    std::atomic<f64> cpu_temperature{0.0};
    std::atomic<f64> gpu_temperature{0.0};
    std::atomic<u64> context_switches{0};
    std::atomic<u64> page_faults{0};
    std::atomic<u64> cache_misses{0};
    std::atomic<u64> instructions_per_cycle{0};
    
    // Optimization status
    bool extreme_mode_enabled = false;
    bool simd_optimizations = false;
    bool cache_optimizations = false;
    bool memory_prefetching = false;
    bool branch_prediction_hints = false;
    bool loop_unrolling = false;
    bool real_time_scheduling = false;
    bool numa_optimizations = false;
};

enum class OptimizationLevel : u32 {
    Disabled = 0,
    Conservative = 1,
    Balanced = 2,
    Aggressive = 3,
    Extreme = 4
};

enum class CPUGovernor : u32 {
    PowerSave = 0,
    OnDemand = 1,
    Conservative = 2,
    Performance = 3,
    UserSpace = 4
};

class ExtremePerformanceOptimizer {
public:
    ExtremePerformanceOptimizer();
    ~ExtremePerformanceOptimizer();
    
    // Core lifecycle
    bool initialize(const PerformanceConfig& config);
    void shutdown();
    
    // Hardware detection
    bool detect_hardware();
    const CPUInfo& get_cpu_info() const;
    const GPUInfo& get_gpu_info() const;
    const MemoryInfo& get_memory_info() const;
    
    // Performance modes
    void enable_extreme_mode(bool enabled);
    void set_optimization_level(OptimizationLevel level);
    OptimizationLevel get_optimization_level() const;
    
    // CPU optimizations
    void apply_cpu_optimizations();
    void set_cpu_affinity(u32 mask);
    void set_process_priority(i32 priority);
    void enable_real_time_scheduling(bool enabled);
    void set_cpu_governor(CPUGovernor governor);
    void isolate_cpu_cores(u32 core_mask);
    
    // Memory optimizations
    void apply_memory_optimizations();
    void enable_huge_pages(bool enabled);
    void lock_memory_pages(bool enabled);
    void enable_memory_prefetching(bool enabled);
    void optimize_numa_allocation(bool enabled);
    
    // Cache optimizations
    void apply_cache_optimizations();
    void prefetch_data(const void* addr, size_t size);
    void optimize_data_layout();
    void enable_cache_warming(bool enabled);
    
    // SIMD optimizations
    void enable_simd_optimizations(bool enabled);
    void simd_vector_add(const f32* a, const f32* b, f32* result, size_t count);
    void simd_vector_multiply(const f32* a, const f32* b, f32* result, size_t count);
    void simd_matrix_multiply(const f32* a, const f32* b, f32* result, size_t rows, size_t cols, size_t inner);
    void simd_vector_normalize(const f32* input, f32* output, size_t count);
    void simd_dot_product(const f32* a, const f32* b, f32* result, size_t count);
    
    // Branch prediction optimization
    void enable_branch_prediction_hints(bool enabled);
    void add_likely_branch_hint(bool condition);
    void add_unlikely_branch_hint(bool condition);
    
    // Loop optimization
    void enable_loop_unrolling(bool enabled);
    void optimize_loop_structure();
    void vectorize_loops(bool enabled);
    
    // Performance monitoring
    PerformanceStats get_performance_stats() const;
    void reset_performance_counters();
    void enable_performance_monitoring(bool enabled);
    void set_performance_targets(f64 target_fps, f64 max_frame_time);
    
    // Adaptive optimization
    void enable_adaptive_optimization(bool enabled);
    void update_optimization_strategy();
    void balance_cpu_gpu_load();
    
    // Memory management
    void* allocate_aligned_memory(size_t size, size_t alignment);
    void free_aligned_memory(void* ptr);
    void* allocate_numa_memory(size_t size, u32 node);
    void free_numa_memory(void* ptr, size_t size);
    
    // Thermal management
    void enable_thermal_management(bool enabled);
    void set_temperature_limits(f32 cpu_limit, f32 gpu_limit);
    void throttle_if_overheating();
    f32 get_cpu_temperature() const;
    f32 get_gpu_temperature() const;
    
    // Power management
    void set_power_profile(const String& profile);
    void enable_power_management(bool enabled);
    void optimize_for_battery_life(bool enabled);
    
    // Profiling and debugging
    void start_profiling(const String& output_file);
    void stop_profiling();
    void dump_performance_data(const String& filename);
    void enable_debug_mode(bool enabled);
    
    // Frame timing
    void frame_rendered();
    void set_frame_time(f64 time_ms);
    void mark_frame_start();
    void mark_frame_end();
    
    // Benchmarking
    void run_cpu_benchmark();
    void run_memory_benchmark();
    void run_cache_benchmark();
    void run_simd_benchmark();
    f64 calculate_performance_score() const;
    
    // System information
    String get_system_info() const;
    String get_optimization_report() const;
    Vector<String> get_optimization_recommendations() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    // Hardware detection
    bool detect_cpu_info();
    bool detect_gpu_info();
    bool detect_memory_info();
    void detect_cache_info();
    void detect_simd_capabilities();
    
    // Initialization helpers
    bool initialize_cpu_optimizations();
    bool initialize_memory_optimizations();
    bool initialize_simd_optimizations();
    bool initialize_cache_optimizations();
    
    // Monitoring loops
    void start_monitoring();
    void stop_monitoring();
    void performance_monitoring_loop();
    void adaptive_optimization_loop();
    void memory_management_loop();
    void cpu_optimization_loop();
    void thermal_monitoring_loop();
    
    // Performance updates
    void update_cpu_utilization();
    void update_gpu_utilization();
    void update_memory_utilization();
    void update_frame_statistics();
    void update_cache_statistics();
    void update_thermal_statistics();
    
    // Optimization helpers
    void prefetch_critical_memory();
    void cleanup_unused_memory();
    void optimize_memory_layout();
    void tune_cache_behavior();
    void adjust_cpu_frequency();
    void balance_workload();
    
    // System control
    void set_cpu_governor(const String& governor);
    void set_cpu_frequency(u32 frequency_mhz);
    void enable_cpu_boost(bool enabled);
    void configure_interrupt_affinity();
    void setup_memory_pools();
    
    // Temperature monitoring
    void monitor_cpu_temperature();
    void monitor_gpu_temperature();
    void apply_thermal_throttling();
    
    // Cleanup
    void restore_cpu_settings();
    void restore_memory_settings();
    void cleanup_memory_pools();
    void restore_system_state();
};

// Utility functions
String get_cpu_model_name();
String get_gpu_model_name();
u64 get_total_system_memory();
u32 get_cpu_core_count();
u32 get_cpu_thread_count();
bool supports_avx512();
bool supports_avx2();
bool supports_numa();
f64 measure_memory_bandwidth();
f64 measure_cache_latency();
f64 calculate_cpu_performance_score();
f64 calculate_memory_performance_score();

// Performance optimization presets
PerformanceConfig get_extreme_performance_preset();
PerformanceConfig get_balanced_performance_preset();
PerformanceConfig get_power_efficient_preset();
PerformanceConfig get_rtx_optimized_preset();
PerformanceConfig get_gtx_optimized_preset();

// SIMD detection macros
#define SIMD_ALIGNED alignas(32)
#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define PREFETCH_READ(addr) __builtin_prefetch(addr, 0, 3)
#define PREFETCH_WRITE(addr) __builtin_prefetch(addr, 1, 3)

} // namespace S1U
