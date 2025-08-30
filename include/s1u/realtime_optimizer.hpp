#pragma once

#include "s1u/core.hpp"
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>

namespace s1u {

// Real-time optimization configuration
struct RealtimeConfig {
    bool enable_realtime_scheduling = true;
    bool enable_memory_pinning = true;
    bool enable_cpu_isolation = true;
    bool enable_interrupt_isolation = true;
    bool enable_numa_optimization = true;
    bool enable_performance_monitoring = true;
    bool enable_thermal_management = true;
    bool enable_power_management = true;
    bool enable_adaptive_optimization = true;
    
    int realtime_priority = 80;
    int process_nice_level = -20;
    int preferred_numa_node = -1; // -1 for auto
    std::string isolated_cpu_list; // e.g., "2-7,10-15"
    
    u64 target_latency_ns = 1000000; // 1ms
    u64 max_pinned_memory_mb = 1024; // 1GB
    f32 thermal_threshold_celsius = 85.0f;
    f32 power_limit_watts = 200.0f;
};

// CPU topology information
struct CPUFeatures {
    bool sse = false;
    bool sse2 = false;
    bool sse3 = false;
    bool ssse3 = false;
    bool sse4_1 = false;
    bool sse4_2 = false;
    bool avx = false;
    bool avx2 = false;
    bool avx512 = false;
    bool fma = false;
    bool aes = false;
    bool rdrand = false;
    bool rdtscp = false;
    bool invariant_tsc = false;
};

struct CacheInfo {
    u32 level = 0;
    u32 size_kb = 0;
    u32 line_size = 0;
    u32 associativity = 0;
    bool shared = false;
};

struct CPUInfo {
    int cpu_id = 0;
    int physical_id = 0;
    int core_id = 0;
    std::vector<int> siblings;
    std::vector<CacheInfo> caches;
    u64 base_frequency_khz = 0;
    u64 max_frequency_khz = 0;
    u64 current_frequency_khz = 0;
    f32 temperature_celsius = 0.0f;
    f32 utilization_percent = 0.0f;
};

// NUMA topology information
struct NUMANodeInfo {
    int node_id = 0;
    std::vector<int> cpus;
    u64 memory_size = 0;
    u64 free_memory = 0;
    std::unordered_map<int, int> distances; // node_id -> distance
};

// Performance statistics
struct RealtimeStats {
    // Latency metrics
    u64 min_latency_ns = UINT64_MAX;
    u64 max_latency_ns = 0;
    u64 avg_latency_ns = 0;
    u64 p99_latency_ns = 0;
    u64 p999_latency_ns = 0;
    
    // Scheduling metrics
    u64 context_switches = 0;
    u64 preemptions = 0;
    u64 migrations = 0;
    u64 wakeups = 0;
    
    // Memory metrics
    u64 page_faults = 0;
    u64 cache_misses = 0;
    u64 cache_hits = 0;
    u64 tlb_misses = 0;
    
    // CPU metrics
    f32 cpu_utilization = 0.0f;
    f32 ipc = 0.0f; // Instructions per cycle
    u64 instructions = 0;
    u64 cycles = 0;
    
    // Thermal metrics
    f32 max_temperature = 0.0f;
    f32 avg_temperature = 0.0f;
    bool thermal_throttling = false;
    
    // Power metrics
    f32 power_consumption_watts = 0.0f;
    f32 energy_efficiency = 0.0f;
    
    // NUMA metrics
    u64 numa_hits = 0;
    u64 numa_misses = 0;
    u64 numa_foreign = 0;
    
    // Interrupt metrics
    u64 interrupts_per_second = 0;
    u64 softirqs_per_second = 0;
    
    // Memory bandwidth
    f32 memory_bandwidth_gbps = 0.0f;
    f32 memory_latency_ns = 0.0f;
};

// Performance monitoring events
enum class PerfEvent {
    CPU_CYCLES,
    INSTRUCTIONS,
    CACHE_REFERENCES,
    CACHE_MISSES,
    BRANCH_INSTRUCTIONS,
    BRANCH_MISSES,
    BUS_CYCLES,
    STALLED_CYCLES_FRONTEND,
    STALLED_CYCLES_BACKEND,
    REF_CPU_CYCLES,
    PAGE_FAULTS,
    CONTEXT_SWITCHES,
    CPU_MIGRATIONS,
    ALIGNMENT_FAULTS,
    EMULATION_FAULTS
};

// Thermal sensor types
enum class ThermalSensorType {
    CPU_PACKAGE,
    CPU_CORE,
    GPU,
    MEMORY,
    CHIPSET,
    AMBIENT
};

struct ThermalSensor {
    ThermalSensorType type;
    int id;
    std::string name;
    f32 temperature;
    f32 critical_temp;
    f32 warning_temp;
    bool active;
};

// Power management states
enum class PowerState {
    PERFORMANCE,
    BALANCED,
    POWER_SAVE,
    CUSTOM
};

enum class CPUGovernor {
    PERFORMANCE,
    POWERSAVE,
    USERSPACE,
    ONDEMAND,
    CONSERVATIVE,
    SCHEDUTIL
};

// Main realtime optimizer class
class RealtimeOptimizer {
public:
    RealtimeOptimizer();
    ~RealtimeOptimizer();
    
    // Core functionality
    bool initialize(const RealtimeConfig& config);
    void shutdown();
    
    // Thread/process optimization
    void optimize_current_thread();
    void optimize_process(pid_t pid);
    void set_thread_realtime_priority(pid_t tid, int priority);
    void set_thread_cpu_affinity(pid_t tid, u64 cpu_mask);
    void set_thread_numa_affinity(pid_t tid, int numa_node);
    void set_thread_memory_policy(pid_t tid);
    
    // Memory optimization
    void pin_memory_region(void* addr, size_t size);
    void unpin_memory_region(void* addr, size_t size);
    void prefault_memory_region(void* addr, size_t size);
    void optimize_numa_allocation(void* addr, size_t size, int preferred_node);
    
    // CPU optimization
    void isolate_interrupts_from_cpu(int cpu);
    void set_cpu_frequency(int cpu, u64 frequency_khz);
    void set_cpu_governor(int cpu, CPUGovernor governor);
    void disable_cpu_idle_states(int cpu);
    void enable_cpu_idle_states(int cpu);
    
    // Performance monitoring
    RealtimeStats get_statistics() const;
    void reset_statistics();
    void enable_performance_monitoring(bool enabled);
    void add_performance_counter(PerfEvent event);
    void remove_performance_counter(PerfEvent event);
    
    // Configuration
    void update_target_latency(u64 latency_ns);
    void set_thermal_threshold(f32 temperature);
    void set_power_limit(f32 watts);
    void enable_adaptive_optimization(bool enabled);
    
    // Hardware queries
    const std::vector<CPUInfo>& get_cpu_topology() const { return cpu_topology_; }
    const std::vector<NUMANodeInfo>& get_numa_topology() const { return numa_topology_; }
    const CPUFeatures& get_cpu_features() const { return cpu_features_; }
    
    // Status queries
    bool is_realtime_enabled() const { return realtime_enabled_; }
    bool is_numa_enabled() const { return numa_optimization_enabled_; }
    bool is_cpu_isolation_enabled() const { return cpu_isolation_enabled_; }
    bool is_interrupt_isolation_enabled() const { return interrupt_isolation_enabled_; }
    bool is_memory_pinning_enabled() const { return memory_pinning_enabled_; }
    
    // Advanced features
    void enable_quantum_optimization(bool enabled);
    void enable_neural_optimization(bool enabled);
    void enable_predictive_optimization(bool enabled);
    void optimize_for_specific_workload(const std::string& workload_type);
    
    // Debugging and profiling
    void dump_cpu_topology() const;
    void dump_numa_topology() const;
    void dump_performance_counters() const;
    std::string get_optimization_report() const;

private:
    // Configuration
    RealtimeConfig config_;
    
    // State
    std::atomic<bool> initialized_;
    std::atomic<bool> running_;
    std::atomic<bool> realtime_enabled_;
    std::atomic<bool> numa_optimization_enabled_;
    std::atomic<bool> interrupt_isolation_enabled_;
    std::atomic<bool> memory_pinning_enabled_;
    std::atomic<bool> cpu_isolation_enabled_;
    std::atomic<bool> thermal_management_enabled_;
    std::atomic<bool> power_management_enabled_;
    std::atomic<bool> performance_monitoring_enabled_;
    std::atomic<bool> adaptive_optimization_enabled_;
    
    // Hardware information
    int cpu_count_;
    int numa_nodes_;
    CPUFeatures cpu_features_;
    std::vector<CPUInfo> cpu_topology_;
    std::vector<NUMANodeInfo> numa_topology_;
    
    // CPU masks
    u64 isolated_cpus_mask_;
    u64 realtime_cpus_mask_;
    u64 interrupt_cpus_mask_;
    
    // Performance targets
    u64 target_latency_ns_;
    
    // Memory management
    std::vector<std::pair<void*, size_t>> pinned_memory_regions_;
    std::mutex pinned_memory_mutex_;
    std::atomic<size_t> total_pinned_memory_;
    
    // Performance monitoring
    std::vector<int> perf_event_fds_;
    mutable std::mutex stats_mutex_;
    RealtimeStats stats_;
    
    // Thermal management
    std::vector<ThermalSensor> thermal_sensors_;
    std::atomic<f32> current_max_temperature_;
    
    // Power management
    PowerState current_power_state_;
    std::atomic<f32> current_power_consumption_;
    
    // Threading
    std::thread monitoring_thread_;
    std::thread thermal_thread_;
    std::thread power_thread_;
    std::thread adaptive_thread_;
    
    // Hardware detection
    void detect_hardware_capabilities();
    bool detect_cpu_topology();
    bool detect_numa_topology();
    void detect_cache_topology();
    void detect_cpu_cache_info(int cpu, CPUInfo& info);
    void detect_cpu_frequency_info(int cpu, CPUInfo& info);
    void check_realtime_capabilities();
    
    // Setup functions
    bool setup_realtime_scheduling();
    bool setup_memory_pinning();
    bool setup_cpu_isolation();
    bool setup_interrupt_isolation();
    void setup_numa_optimizations();
    bool setup_performance_monitoring();
    bool setup_thermal_management();
    bool setup_power_management();
    
    // CPU management
    void auto_select_isolated_cpus();
    void parse_cpu_list(const std::string& cpu_list, u64& cpu_mask);
    void migrate_processes_from_isolated_cpus();
    void set_irq_affinity(int irq, u64 cpu_mask);
    
    // NUMA management
    void set_process_numa_policy(pid_t pid);
    void setup_numa_balancing();
    void migrate_pages_to_node(void* addr, size_t size, int node);
    
    // Performance monitoring
    void setup_perf_events();
    void setup_cpu_performance_counters();
    void setup_memory_bandwidth_monitoring();
    void collect_performance_metrics();
    void update_statistics();
    void check_optimization_opportunities();
    
    // Thermal management
    bool initialize_thermal_sensors();
    void setup_thermal_policies();
    void read_thermal_sensors();
    void apply_thermal_policies();
    void adjust_cpu_frequencies_for_thermal();
    
    // Power management
    void setup_cpu_frequency_scaling();
    void setup_power_capping();
    void setup_idle_states();
    void monitor_power_consumption();
    void apply_power_policies();
    void optimize_idle_states();
    
    // Adaptive optimization
    void analyze_current_performance();
    void adapt_optimizations();
    void learn_from_optimizations();
    void adjust_optimizations_for_latency();
    
    // Cleanup
    void restore_original_settings();
    void cleanup_performance_monitoring();
    void cleanup_thermal_management();
    void cleanup_power_management();
    
    // Threading functions
    void start_optimization_threads();
    void stop_optimization_threads();
    void monitoring_loop();
    void thermal_management_loop();
    void power_management_loop();
    void adaptive_optimization_loop();
    
    // Utility functions
    void read_cpu_info(const std::string& path, int& value);
    void read_cpu_siblings(const std::string& path, std::vector<int>& siblings);
    void disable_aslr_for_thread(pid_t tid);
    pid_t gettid();
};

using RealtimeOptimizerPtr = std::unique_ptr<RealtimeOptimizer>;

} // namespace s1u
