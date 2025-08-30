#include "s1u/realtime_optimizer.hpp"
#include "s1u/core.hpp"
#include <sched.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <numa.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/perf_event.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <cpuid.h>
#include <immintrin.h>

namespace s1u {

RealtimeOptimizer::RealtimeOptimizer()
    : initialized_(false)
    , realtime_enabled_(false)
    , numa_optimization_enabled_(false)
    , interrupt_isolation_enabled_(false)
    , memory_pinning_enabled_(false)
    , cpu_isolation_enabled_(false)
    , thermal_management_enabled_(true)
    , power_management_enabled_(true)
    , performance_monitoring_enabled_(true)
    , adaptive_optimization_enabled_(true)
    , target_latency_ns_(1000000) // 1ms
    , cpu_count_(0)
    , numa_nodes_(0)
    , isolated_cpus_mask_(0)
    , realtime_cpus_mask_(0)
    , interrupt_cpus_mask_(0) {
    
    detect_hardware_capabilities();
}

RealtimeOptimizer::~RealtimeOptimizer() {
    shutdown();
}

bool RealtimeOptimizer::initialize(const RealtimeConfig& config) {
    if (initialized_) return true;
    
    Logger::info("Initializing Realtime Optimizer...");
    
    config_ = config;
    
    // Check root privileges
    if (getuid() != 0) {
        Logger::warning("Realtime optimization requires root privileges, some features will be disabled");
    }
    
    // Initialize CPU topology detection
    if (!detect_cpu_topology()) {
        Logger::error("Failed to detect CPU topology");
        return false;
    }
    
    // Initialize NUMA topology
    if (!detect_numa_topology()) {
        Logger::warning("NUMA topology detection failed, disabling NUMA optimizations");
        numa_optimization_enabled_ = false;
    }
    
    // Setup real-time scheduling
    if (config_.enable_realtime_scheduling && !setup_realtime_scheduling()) {
        Logger::warning("Failed to setup real-time scheduling");
        realtime_enabled_ = false;
    }
    
    // Setup memory pinning
    if (config_.enable_memory_pinning && !setup_memory_pinning()) {
        Logger::warning("Failed to setup memory pinning");
        memory_pinning_enabled_ = false;
    }
    
    // Setup CPU isolation
    if (config_.enable_cpu_isolation && !setup_cpu_isolation()) {
        Logger::warning("Failed to setup CPU isolation");
        cpu_isolation_enabled_ = false;
    }
    
    // Setup interrupt handling
    if (config_.enable_interrupt_isolation && !setup_interrupt_isolation()) {
        Logger::warning("Failed to setup interrupt isolation");
        interrupt_isolation_enabled_ = false;
    }
    
    // Setup NUMA optimizations
    if (config_.enable_numa_optimization && numa_optimization_enabled_) {
        setup_numa_optimizations();
    }
    
    // Setup performance monitoring
    if (config_.enable_performance_monitoring && !setup_performance_monitoring()) {
        Logger::warning("Failed to setup performance monitoring");
        performance_monitoring_enabled_ = false;
    }
    
    // Setup thermal management
    if (config_.enable_thermal_management && !setup_thermal_management()) {
        Logger::warning("Failed to setup thermal management");
        thermal_management_enabled_ = false;
    }
    
    // Setup power management
    if (config_.enable_power_management && !setup_power_management()) {
        Logger::warning("Failed to setup power management");
        power_management_enabled_ = false;
    }
    
    // Start optimization threads
    start_optimization_threads();
    
    initialized_ = true;
    
    Logger::info("Realtime Optimizer initialized successfully");
    Logger::info("Features: RT={}, NUMA={}, CPU_ISO={}, INT_ISO={}, MEM_PIN={}, THERMAL={}, POWER={}",
                realtime_enabled_, numa_optimization_enabled_, cpu_isolation_enabled_,
                interrupt_isolation_enabled_, memory_pinning_enabled_,
                thermal_management_enabled_, power_management_enabled_);
    
    return true;
}

void RealtimeOptimizer::shutdown() {
    if (!initialized_) return;
    
    Logger::info("Shutting down Realtime Optimizer...");
    
    // Stop optimization threads
    stop_optimization_threads();
    
    // Restore original settings
    restore_original_settings();
    
    // Cleanup performance monitoring
    cleanup_performance_monitoring();
    
    // Cleanup thermal management
    cleanup_thermal_management();
    
    // Cleanup power management
    cleanup_power_management();
    
    initialized_ = false;
    
    Logger::info("Realtime Optimizer shut down");
}

void RealtimeOptimizer::optimize_current_thread() {
    if (!initialized_) return;
    
    pid_t tid = gettid();
    
    // Set real-time priority
    if (realtime_enabled_) {
        set_thread_realtime_priority(tid, config_.realtime_priority);
    }
    
    // Set CPU affinity to isolated cores
    if (cpu_isolation_enabled_) {
        set_thread_cpu_affinity(tid, realtime_cpus_mask_);
    }
    
    // Set NUMA affinity
    if (numa_optimization_enabled_) {
        set_thread_numa_affinity(tid, config_.preferred_numa_node);
    }
    
    // Set memory policy
    set_thread_memory_policy(tid);
    
    // Disable address space randomization for this thread
    disable_aslr_for_thread(tid);
    
    Logger::debug("Optimized thread {} for real-time operation", tid);
}

void RealtimeOptimizer::optimize_process(pid_t pid) {
    if (!initialized_) return;
    
    // Set process priority
    if (setpriority(PRIO_PROCESS, pid, config_.process_nice_level) != 0) {
        Logger::warning("Failed to set process priority for PID {}: {}", pid, strerror(errno));
    }
    
    // Set process CPU affinity
    if (cpu_isolation_enabled_) {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        
        for (int cpu = 0; cpu < cpu_count_; ++cpu) {
            if (realtime_cpus_mask_ & (1ULL << cpu)) {
                CPU_SET(cpu, &cpu_set);
            }
        }
        
        if (sched_setaffinity(pid, sizeof(cpu_set), &cpu_set) != 0) {
            Logger::warning("Failed to set CPU affinity for PID {}: {}", pid, strerror(errno));
        }
    }
    
    // Set NUMA memory policy for process
    if (numa_optimization_enabled_) {
        set_process_numa_policy(pid);
    }
    
    Logger::debug("Optimized process {} for real-time operation", pid);
}

void RealtimeOptimizer::pin_memory_region(void* addr, size_t size) {
    if (!memory_pinning_enabled_) return;
    
    if (mlock(addr, size) != 0) {
        Logger::warning("Failed to pin memory region at {}: {}", addr, strerror(errno));
        return;
    }
    
    // Add to tracked regions
    std::lock_guard<std::mutex> lock(pinned_memory_mutex_);
    pinned_memory_regions_.emplace_back(addr, size);
    
    total_pinned_memory_ += size;
    Logger::debug("Pinned {} bytes at {}, total pinned: {} bytes", 
                 size, addr, total_pinned_memory_);
}

void RealtimeOptimizer::unpin_memory_region(void* addr, size_t size) {
    if (!memory_pinning_enabled_) return;
    
    if (munlock(addr, size) != 0) {
        Logger::warning("Failed to unpin memory region at {}: {}", addr, strerror(errno));
        return;
    }
    
    // Remove from tracked regions
    std::lock_guard<std::mutex> lock(pinned_memory_mutex_);
    auto it = std::find_if(pinned_memory_regions_.begin(), pinned_memory_regions_.end(),
                          [addr](const auto& region) { return region.first == addr; });
    
    if (it != pinned_memory_regions_.end()) {
        total_pinned_memory_ -= it->second;
        pinned_memory_regions_.erase(it);
    }
    
    Logger::debug("Unpinned {} bytes at {}, total pinned: {} bytes", 
                 size, addr, total_pinned_memory_);
}

void RealtimeOptimizer::prefault_memory_region(void* addr, size_t size) {
    // Touch every page to force allocation
    const size_t page_size = getpagesize();
    char* ptr = static_cast<char*>(addr);
    
    for (size_t offset = 0; offset < size; offset += page_size) {
        // Read and write to force page allocation
        volatile char dummy = ptr[offset];
        ptr[offset] = dummy;
    }
    
    Logger::debug("Prefaulted {} bytes starting at {}", size, addr);
}

void RealtimeOptimizer::isolate_interrupts_from_cpu(int cpu) {
    if (!interrupt_isolation_enabled_ || getuid() != 0) return;
    
    // Move all interrupts away from specified CPU
    std::string proc_interrupts = "/proc/interrupts";
    std::ifstream file(proc_interrupts);
    std::string line;
    
    // Skip header line
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string irq_str;
        iss >> irq_str;
        
        if (irq_str.empty() || !std::isdigit(irq_str[0])) continue;
        
        int irq = std::stoi(irq_str.substr(0, irq_str.find(':')));
        set_irq_affinity(irq, interrupt_cpus_mask_ & ~(1ULL << cpu));
    }
    
    Logger::debug("Isolated interrupts from CPU {}", cpu);
}

void RealtimeOptimizer::optimize_numa_allocation(void* addr, size_t size, int preferred_node) {
    if (!numa_optimization_enabled_) return;
    
    // Set memory policy for this region
    if (mbind(addr, size, MPOL_PREFERRED, numa_get_mems_allowed(), numa_max_node() + 1, 0) != 0) {
        Logger::warning("Failed to set NUMA policy for memory region: {}", strerror(errno));
        return;
    }
    
    // Migrate pages to preferred node if needed
    migrate_pages_to_node(addr, size, preferred_node);
    
    Logger::debug("Optimized NUMA allocation for {} bytes to node {}", size, preferred_node);
}

RealtimeStats RealtimeOptimizer::get_statistics() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return stats_;
}

void RealtimeOptimizer::update_target_latency(u64 latency_ns) {
    target_latency_ns_ = latency_ns;
    
    // Adjust optimizations based on new target
    adjust_optimizations_for_latency();
    
    Logger::info("Updated target latency to {} ns", latency_ns);
}

// Private implementation methods

void RealtimeOptimizer::detect_hardware_capabilities() {
    // Detect CPU features
    cpu_count_ = sysconf(_SC_NPROCESSORS_ONLN);
    
    // Check for Intel/AMD specific features
    unsigned int eax, ebx, ecx, edx;
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        cpu_features_.sse = (edx & (1 << 25)) != 0;
        cpu_features_.sse2 = (edx & (1 << 26)) != 0;
        cpu_features_.sse3 = (ecx & (1 << 0)) != 0;
        cpu_features_.ssse3 = (ecx & (1 << 9)) != 0;
        cpu_features_.sse4_1 = (ecx & (1 << 19)) != 0;
        cpu_features_.sse4_2 = (ecx & (1 << 20)) != 0;
        cpu_features_.avx = (ecx & (1 << 28)) != 0;
    }
    
    if (__get_cpuid(7, &eax, &ebx, &ecx, &edx)) {
        cpu_features_.avx2 = (ebx & (1 << 5)) != 0;
        cpu_features_.avx512 = (ebx & (1 << 16)) != 0;
    }
    
    // Detect cache sizes
    detect_cache_topology();
    
    // Check for real-time capabilities
    check_realtime_capabilities();
    
    Logger::info("Detected {} CPUs with features: AVX={}, AVX2={}, AVX512={}",
                cpu_count_, cpu_features_.avx, cpu_features_.avx2, cpu_features_.avx512);
}

bool RealtimeOptimizer::detect_cpu_topology() {
    cpu_topology_.clear();
    
    for (int cpu = 0; cpu < cpu_count_; ++cpu) {
        CPUInfo info;
        info.cpu_id = cpu;
        
        // Read CPU info from sysfs
        std::string base_path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu);
        
        // Read physical ID
        read_cpu_info(base_path + "/topology/physical_package_id", info.physical_id);
        
        // Read core ID
        read_cpu_info(base_path + "/topology/core_id", info.core_id);
        
        // Read thread siblings
        std::string siblings_path = base_path + "/topology/thread_siblings_list";
        read_cpu_siblings(siblings_path, info.siblings);
        
        // Read cache information
        detect_cpu_cache_info(cpu, info);
        
        // Read frequency information
        detect_cpu_frequency_info(cpu, info);
        
        cpu_topology_.push_back(info);
    }
    
    return !cpu_topology_.empty();
}

bool RealtimeOptimizer::detect_numa_topology() {
    if (numa_available() < 0) {
        Logger::warning("NUMA not available on this system");
        return false;
    }
    
    numa_nodes_ = numa_max_node() + 1;
    numa_topology_.clear();
    numa_topology_.resize(numa_nodes_);
    
    for (int node = 0; node < numa_nodes_; ++node) {
        NUMANodeInfo& info = numa_topology_[node];
        info.node_id = node;
        
        // Get CPUs on this node
        struct bitmask* cpus = numa_allocate_cpumask();
        if (numa_node_to_cpus(node, cpus) == 0) {
            for (int cpu = 0; cpu < cpu_count_; ++cpu) {
                if (numa_bitmask_isbitset(cpus, cpu)) {
                    info.cpus.push_back(cpu);
                }
            }
        }
        numa_free_cpumask(cpus);
        
        // Get memory size
        info.memory_size = numa_node_size64(node, &info.free_memory);
        
        // Calculate distances to other nodes
        for (int other_node = 0; other_node < numa_nodes_; ++other_node) {
            info.distances[other_node] = numa_distance(node, other_node);
        }
    }
    
    Logger::info("Detected {} NUMA nodes", numa_nodes_);
    return true;
}

bool RealtimeOptimizer::setup_realtime_scheduling() {
    if (getuid() != 0) {
        Logger::warning("Real-time scheduling requires root privileges");
        return false;
    }
    
    // Set process scheduler to FIFO with high priority
    struct sched_param param;
    param.sched_priority = config_.realtime_priority;
    
    if (sched_setscheduler(0, SCHED_FIFO, &param) != 0) {
        Logger::error("Failed to set real-time scheduler: {}", strerror(errno));
        return false;
    }
    
    // Disable swap for this process
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
        Logger::warning("Failed to lock memory: {}", strerror(errno));
    }
    
    realtime_enabled_ = true;
    Logger::info("Real-time scheduling enabled with priority {}", config_.realtime_priority);
    return true;
}

bool RealtimeOptimizer::setup_memory_pinning() {
    // Check memory limits
    struct rlimit limit;
    if (getrlimit(RLIMIT_MEMLOCK, &limit) == 0) {
        if (limit.rlim_cur != RLIM_INFINITY) {
            Logger::info("Memory lock limit: {} bytes", limit.rlim_cur);
        }
    }
    
    memory_pinning_enabled_ = true;
    total_pinned_memory_ = 0;
    
    Logger::info("Memory pinning enabled");
    return true;
}

bool RealtimeOptimizer::setup_cpu_isolation() {
    // Determine which CPUs to isolate
    if (config_.isolated_cpu_list.empty()) {
        // Auto-select CPUs for isolation
        auto_select_isolated_cpus();
    } else {
        // Use user-specified CPU list
        parse_cpu_list(config_.isolated_cpu_list, isolated_cpus_mask_);
    }
    
    // Isolate real-time CPUs (subset of isolated CPUs)
    realtime_cpus_mask_ = isolated_cpus_mask_;
    
    // Reserve other CPUs for interrupts and background tasks
    interrupt_cpus_mask_ = ~isolated_cpus_mask_ & ((1ULL << cpu_count_) - 1);
    
    // Move existing processes away from isolated CPUs
    migrate_processes_from_isolated_cpus();
    
    cpu_isolation_enabled_ = true;
    
    Logger::info("CPU isolation enabled: isolated_mask=0x{:x}, realtime_mask=0x{:x}, interrupt_mask=0x{:x}",
                isolated_cpus_mask_, realtime_cpus_mask_, interrupt_cpus_mask_);
    return true;
}

bool RealtimeOptimizer::setup_interrupt_isolation() {
    if (getuid() != 0) {
        Logger::warning("Interrupt isolation requires root privileges");
        return false;
    }
    
    // Move all interrupts to non-isolated CPUs
    for (int cpu = 0; cpu < cpu_count_; ++cpu) {
        if (isolated_cpus_mask_ & (1ULL << cpu)) {
            isolate_interrupts_from_cpu(cpu);
        }
    }
    
    interrupt_isolation_enabled_ = true;
    Logger::info("Interrupt isolation enabled");
    return true;
}

void RealtimeOptimizer::setup_numa_optimizations() {
    if (!numa_optimization_enabled_) return;
    
    // Set default NUMA policy
    if (numa_set_localalloc() != 0) {
        Logger::warning("Failed to set local NUMA allocation policy");
    }
    
    // Setup NUMA balancing
    setup_numa_balancing();
    
    Logger::info("NUMA optimizations enabled");
}

bool RealtimeOptimizer::setup_performance_monitoring() {
    // Setup perf events for monitoring
    setup_perf_events();
    
    // Setup CPU performance counters
    setup_cpu_performance_counters();
    
    // Setup memory bandwidth monitoring
    setup_memory_bandwidth_monitoring();
    
    performance_monitoring_enabled_ = true;
    Logger::info("Performance monitoring enabled");
    return true;
}

bool RealtimeOptimizer::setup_thermal_management() {
    // Initialize thermal monitoring
    if (!initialize_thermal_sensors()) {
        return false;
    }
    
    // Setup thermal throttling policies
    setup_thermal_policies();
    
    thermal_management_enabled_ = true;
    Logger::info("Thermal management enabled");
    return true;
}

bool RealtimeOptimizer::setup_power_management() {
    // Setup CPU frequency scaling
    setup_cpu_frequency_scaling();
    
    // Setup power capping
    setup_power_capping();
    
    // Setup idle states management
    setup_idle_states();
    
    power_management_enabled_ = true;
    Logger::info("Power management enabled");
    return true;
}

void RealtimeOptimizer::start_optimization_threads() {
    running_ = true;
    
    // Start monitoring thread
    monitoring_thread_ = std::thread([this]() {
        monitoring_loop();
    });
    
    // Start thermal management thread
    if (thermal_management_enabled_) {
        thermal_thread_ = std::thread([this]() {
            thermal_management_loop();
        });
    }
    
    // Start power management thread
    if (power_management_enabled_) {
        power_thread_ = std::thread([this]() {
            power_management_loop();
        });
    }
    
    // Start adaptive optimization thread
    if (adaptive_optimization_enabled_) {
        adaptive_thread_ = std::thread([this]() {
            adaptive_optimization_loop();
        });
    }
}

void RealtimeOptimizer::stop_optimization_threads() {
    running_ = false;
    
    if (monitoring_thread_.joinable()) {
        monitoring_thread_.join();
    }
    
    if (thermal_thread_.joinable()) {
        thermal_thread_.join();
    }
    
    if (power_thread_.joinable()) {
        power_thread_.join();
    }
    
    if (adaptive_thread_.joinable()) {
        adaptive_thread_.join();
    }
}

void RealtimeOptimizer::monitoring_loop() {
    Logger::info("Performance monitoring loop started");
    
    while (running_) {
        // Collect performance metrics
        collect_performance_metrics();
        
        // Update statistics
        update_statistics();
        
        // Check for optimization opportunities
        check_optimization_opportunities();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    Logger::info("Performance monitoring loop finished");
}

void RealtimeOptimizer::thermal_management_loop() {
    Logger::info("Thermal management loop started");
    
    while (running_) {
        // Read thermal sensors
        read_thermal_sensors();
        
        // Apply thermal policies
        apply_thermal_policies();
        
        // Adjust CPU frequencies if needed
        adjust_cpu_frequencies_for_thermal();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    Logger::info("Thermal management loop finished");
}

void RealtimeOptimizer::power_management_loop() {
    Logger::info("Power management loop started");
    
    while (running_) {
        // Monitor power consumption
        monitor_power_consumption();
        
        // Apply power policies
        apply_power_policies();
        
        // Optimize idle states
        optimize_idle_states();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    Logger::info("Power management loop finished");
}

void RealtimeOptimizer::adaptive_optimization_loop() {
    Logger::info("Adaptive optimization loop started");
    
    while (running_) {
        // Analyze current performance
        analyze_current_performance();
        
        // Adapt optimizations based on workload
        adapt_optimizations();
        
        // Learn from optimization results
        learn_from_optimizations();
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    Logger::info("Adaptive optimization loop finished");
}

// Implementation stubs for helper functions
void RealtimeOptimizer::set_thread_realtime_priority(pid_t tid, int priority) {
    struct sched_param param;
    param.sched_priority = priority;
    
    if (sched_setscheduler(tid, SCHED_FIFO, &param) != 0) {
        Logger::warning("Failed to set real-time priority for thread {}: {}", tid, strerror(errno));
    }
}

void RealtimeOptimizer::set_thread_cpu_affinity(pid_t tid, u64 cpu_mask) {
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    
    for (int cpu = 0; cpu < cpu_count_; ++cpu) {
        if (cpu_mask & (1ULL << cpu)) {
            CPU_SET(cpu, &cpu_set);
        }
    }
    
    if (sched_setaffinity(tid, sizeof(cpu_set), &cpu_set) != 0) {
        Logger::warning("Failed to set CPU affinity for thread {}: {}", tid, strerror(errno));
    }
}

void RealtimeOptimizer::set_irq_affinity(int irq, u64 cpu_mask) {
    std::string affinity_path = "/proc/irq/" + std::to_string(irq) + "/smp_affinity";
    std::ofstream file(affinity_path);
    
    if (file.is_open()) {
        file << std::hex << cpu_mask;
        file.close();
    } else {
        Logger::warning("Failed to set IRQ {} affinity", irq);
    }
}

// Additional implementation stubs would go here...

} // namespace s1u
