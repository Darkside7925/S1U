//
// S1U NVIDIA RTX/GTX Driver Header
// Extreme performance optimizations for NVIDIA graphics cards
//

#pragma once

#include "s1u/core.hpp"
#include <memory>
#include <atomic>

namespace S1U {

struct NVIDIADriverConfig {
    bool enable_rtx = true;
    bool enable_dlss = true;
    bool enable_cuda_interop = true;
    bool enable_nvlink = true;
    bool extreme_mode = true;
    bool enable_overclocking = false;
    bool enable_multi_gpu = true;
    bool enable_async_compute = true;
    bool enable_memory_compression = true;
    
    // Performance targets
    u32 target_fps = 240;
    f32 target_frame_time_ms = 4.16f; // 240 FPS
    u32 memory_pool_size_mb = 1024;
    
    // RTX settings
    u32 rtx_ray_depth = 10;
    f32 rtx_denoiser_strength = 0.8f;
    bool rtx_global_illumination = true;
    bool rtx_reflections = true;
    bool rtx_shadows = true;
    
    // DLSS settings
    u32 dlss_quality_mode = 0; // 0=Performance, 1=Balanced, 2=Quality, 3=Ultra Performance
    bool dlss_ray_reconstruction = true;
    f32 dlss_sharpening = 0.0f;
    
    // Optimization settings
    bool prefer_dedicated_memory = true;
    bool enable_gpu_boost = true;
    bool enable_memory_overclock = false;
    u32 power_limit_watts = 400;
    u32 temperature_limit_celsius = 83;
    
    // Vulkan settings
    u32 vulkan_api_version = VK_API_VERSION_1_3;
    bool enable_vulkan_debug = false;
    bool enable_gpu_assisted_validation = false;
    
    // CUDA settings
    u32 cuda_streams = 8;
    bool cuda_unified_memory = true;
    bool cuda_memory_pools = true;
};

struct NVIDIADriverStats {
    // Performance metrics
    std::atomic<f64> gpu_utilization{0.0};
    std::atomic<f64> memory_utilization{0.0};
    std::atomic<f64> temperature{0.0};
    std::atomic<f64> power_usage{0.0};
    std::atomic<u64> frame_count{0};
    std::atomic<f64> avg_frame_time{0.0};
    std::atomic<f64> min_frame_time{999.0};
    std::atomic<f64> max_frame_time{0.0};
    
    // Memory stats
    std::atomic<u64> total_memory{0};
    std::atomic<u64> free_memory{0};
    std::atomic<u64> used_memory{0};
    std::atomic<f64> memory_bandwidth_gbps{0.0};
    
    // GPU stats
    std::atomic<u32> gpu_clock_mhz{0};
    std::atomic<u32> memory_clock_mhz{0};
    std::atomic<u32> shader_clock_mhz{0};
    std::atomic<f32> gpu_voltage{0.0f};
    
    // Feature availability
    bool rtx_enabled = false;
    bool dlss_enabled = false;
    bool cuda_interop_enabled = false;
    bool nvlink_enabled = false;
    bool multi_gpu_enabled = false;
    bool extreme_mode = false;
    
    // Driver info
    String driver_version;
    String gpu_name;
    u32 cuda_compute_capability_major = 0;
    u32 cuda_compute_capability_minor = 0;
    u32 rtx_core_count = 0;
    u32 tensor_core_count = 0;
    u32 rt_core_count = 0;
};

enum class RTXMode : u32 {
    Disabled = 0,
    Performance = 1,
    Balanced = 2,
    Quality = 3,
    Extreme = 4
};

enum class DLSSMode : u32 {
    Off = 0,
    UltraPerformance = 1,
    Performance = 2,
    Balanced = 3,
    Quality = 4,
    UltraQuality = 5
};

enum class GPUBoostLevel : u32 {
    Disabled = 0,
    Conservative = 1,
    Aggressive = 2,
    Extreme = 3,
    Maximum = 4
};

class NVIDIADriver {
public:
    NVIDIADriver();
    ~NVIDIADriver();
    
    // Core lifecycle
    bool initialize(const NVIDIADriverConfig& config);
    void shutdown();
    
    // Feature support queries
    bool supports_rtx() const;
    bool supports_dlss() const;
    bool supports_cuda_interop() const;
    bool supports_nvlink() const;
    bool supports_multi_gpu() const;
    bool supports_ray_tracing() const;
    bool supports_mesh_shaders() const;
    bool supports_variable_rate_shading() const;
    
    // Performance controls
    void enable_extreme_mode(bool enabled);
    void set_rtx_mode(RTXMode mode);
    void set_dlss_mode(DLSSMode mode);
    void set_gpu_boost_level(GPUBoostLevel level);
    void set_power_limit(u32 watts);
    void set_temperature_limit(u32 celsius);
    
    // Memory management
    void* allocate_gpu_memory(size_t size, bool device_local = true);
    void free_gpu_memory(void* ptr);
    void* allocate_unified_memory(size_t size);
    void free_unified_memory(void* ptr);
    
    // Vulkan integration
    VkDevice get_vulkan_device() const;
    VkPhysicalDevice get_vulkan_physical_device() const;
    VkInstance get_vulkan_instance() const;
    VkQueue get_graphics_queue() const;
    VkQueue get_compute_queue() const;
    VkQueue get_transfer_queue() const;
    
    // CUDA integration
    void* get_cuda_context() const;
    void* get_cuda_stream() const;
    bool copy_vulkan_to_cuda(VkBuffer vulkan_buffer, void* cuda_ptr, size_t size);
    bool copy_cuda_to_vulkan(void* cuda_ptr, VkBuffer vulkan_buffer, size_t size);
    
    // RTX operations
    bool create_rtx_pipeline(const String& shader_path);
    bool trace_rays(u32 width, u32 height, u32 depth = 1);
    void enable_denoising(bool enabled);
    void set_ray_depth(u32 depth);
    
    // DLSS operations
    bool upscale_with_dlss(VkImage input, VkImage output, u32 input_width, u32 input_height, 
                          u32 output_width, u32 output_height);
    void set_dlss_sharpening(f32 sharpening);
    
    // Performance monitoring
    NVIDIADriverStats get_performance_stats() const;
    void reset_performance_counters();
    void enable_performance_monitoring(bool enabled);
    
    // Optimization
    void optimize_for_latency();
    void optimize_for_throughput();
    void optimize_for_power_efficiency();
    void auto_optimize();
    
    // Multi-GPU
    bool setup_multi_gpu_rendering(u32 gpu_count);
    void distribute_workload(u32 gpu_index, f32 workload_percentage);
    void synchronize_gpus();
    
    // Overclocking (use with caution)
    bool enable_overclocking(bool enabled);
    void set_gpu_clock_offset(i32 mhz_offset);
    void set_memory_clock_offset(i32 mhz_offset);
    void set_voltage_offset(f32 voltage_offset);
    
    // Cooling and thermal management
    void set_fan_curve(const Vector<Point>& curve);
    void enable_aggressive_cooling(bool enabled);
    f32 get_current_temperature() const;
    void set_thermal_throttling(bool enabled);
    
    // Advanced features
    void enable_resizable_bar(bool enabled);
    void enable_gpu_scheduling(bool enabled);
    void enable_hardware_scheduling(bool enabled);
    void configure_memory_compression(bool enabled);
    
    // Debugging and profiling
    void enable_nsight_profiling(bool enabled);
    void capture_gpu_trace(const String& filename);
    void dump_gpu_state(const String& filename);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    // Internal initialization
    bool initialize_nvapi();
    bool initialize_nvml();
    bool initialize_vulkan();
    bool initialize_cuda();
    bool initialize_rtx();
    bool initialize_dlss();
    bool setup_memory_pools();
    
    // Device selection and setup
    bool select_nvidia_device();
    bool create_logical_device();
    bool setup_queues();
    bool setup_memory_types();
    
    // Optimization internals
    void apply_extreme_optimizations();
    void enable_gpu_boost();
    void optimize_memory_clocks();
    void enable_async_compute();
    void enable_memory_compression();
    void configure_power_management();
    void detect_and_enable_multi_gpu();
    void setup_nvlink();
    
    // Monitoring
    void start_performance_monitoring();
    void stop_performance_monitoring();
    void performance_monitoring_loop();
    void thermal_monitoring_loop();
    void power_monitoring_loop();
    
    // Cleanup
    void cleanup_nvapi();
    void cleanup_nvml();
    void cleanup_vulkan();
    void cleanup_cuda();
    void cleanup_rtx();
    void cleanup_dlss();
};

// Helper functions
String get_nvidia_driver_version();
Vector<String> get_available_nvidia_gpus();
bool is_rtx_gpu(const String& gpu_name);
bool is_gtx_gpu(const String& gpu_name);
u32 get_gpu_compute_capability(const String& gpu_name);
f64 estimate_gpu_performance_score(const String& gpu_name);

// RTX/GTX specific optimizations
void optimize_for_rtx_4090();
void optimize_for_rtx_4080();
void optimize_for_rtx_4070();
void optimize_for_rtx_3090();
void optimize_for_rtx_3080();
void optimize_for_rtx_3070();
void optimize_for_gtx_1660();
void optimize_for_gtx_1650();

} // namespace S1U
