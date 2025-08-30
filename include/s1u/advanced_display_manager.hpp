#pragma once

#include "s1u/core.hpp"
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <atomic>
#include <thread>

namespace S1U {

struct DisplayConfig {
    bool enable_hdr = true;
    bool enable_variable_refresh = true;
    bool enable_adaptive_sync = true;
    bool enable_hardware_overlay = true;
    bool enable_color_correction = true;
    bool enable_gamma_correction = true;
    bool enable_brightness_control = true;
    bool auto_configure_displays = true;
    bool prefer_high_refresh = true;
    bool prefer_high_resolution = true;
    u32 max_refresh_rate = 240;
    u32 min_refresh_rate = 48;
    u32 preferred_bit_depth = 10;
    String preferred_color_space = "sRGB";
};

struct DisplayMode {
    u32 width = 0;
    u32 height = 0;
    u32 refresh_rate = 60;
    bool is_preferred = false;
    u32 pixel_clock = 0;
    u32 flags = 0;
    f32 aspect_ratio = 0.0f;
    String description;
};

enum DisplayModeFlags {
    DISPLAY_MODE_INTERLACED = 1 << 0,
    DISPLAY_MODE_DOUBLESCAN = 1 << 1,
    DISPLAY_MODE_CSYNC = 1 << 2,
    DISPLAY_MODE_PSYNC = 1 << 3,
    DISPLAY_MODE_NSYNC = 1 << 4,
    DISPLAY_MODE_HSYNC = 1 << 5,
    DISPLAY_MODE_VSYNC = 1 << 6
};

struct DisplayOutput {
    u32 connector_id = 0;
    String connector_type;
    bool is_connected = false;
    bool is_primary = false;
    u32 width_mm = 0;
    u32 height_mm = 0;
    Vector<DisplayMode> supported_modes;
    Vector<String> supported_formats;
    Vector<String> color_spaces;
    Vector<String> hdr_metadata_types;
    bool supports_hdr = false;
    bool supports_hdr10 = false;
    bool supports_dolby_vision = false;
    bool supports_wide_color = false;
    bool supports_variable_refresh = false;
    bool supports_adaptive_sync = false;
    bool supports_10bit = false;
    bool supports_12bit = false;
    u32 max_bpc = 8;
    u32 color_depth = 8;
    f32 max_luminance = 100.0f;
    f32 min_luminance = 0.1f;
    String manufacturer;
    String model;
    String serial_number;
    Vec2 position = {0.0f, 0.0f};
    f32 scale_factor = 1.0f;
    f32 rotation = 0.0f;
};

struct HDRMetadata {
    f32 display_primaries_x[3];
    f32 display_primaries_y[3];
    f32 white_point_x;
    f32 white_point_y;
    f32 max_display_mastering_luminance;
    f32 min_display_mastering_luminance;
    f32 max_cll;
    f32 max_fall;
};

struct OverlayPlane {
    u32 plane_id = 0;
    u32 possible_crtcs = 0;
    u32 gamma_size = 0;
    bool is_overlay = false;
    bool is_cursor = false;
    bool is_primary = false;
    Vector<u32> supported_formats;
    Vec2 position = {0.0f, 0.0f};
    Vec2 size = {0.0f, 0.0f};
    f32 alpha = 1.0f;
    bool enabled = false;
};

struct Matrix3 {
    f32 m[9];
    
    Matrix3() {
        for (int i = 0; i < 9; i++) m[i] = 0.0f;
    }
    
    Matrix3(f32 m00, f32 m01, f32 m02,
           f32 m10, f32 m11, f32 m12,
           f32 m20, f32 m21, f32 m22) {
        m[0] = m00; m[1] = m01; m[2] = m02;
        m[3] = m10; m[4] = m11; m[5] = m12;
        m[6] = m20; m[7] = m21; m[8] = m22;
    }
    
    static Matrix3 identity() {
        return Matrix3(1.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 1.0f);
    }
    
    Matrix3 operator*(const Matrix3& other) const {
        Matrix3 result;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.m[i*3 + j] = 0;
                for (int k = 0; k < 3; k++) {
                    result.m[i*3 + j] += m[i*3 + k] * other.m[k*3 + j];
                }
            }
        }
        return result;
    }
};

struct DisplayStats {
    std::atomic<u64> frames_presented{0};
    std::atomic<f64> average_fps{0.0};
    std::atomic<f64> frame_time_ms{0.0};
    std::atomic<f64> gpu_utilization{0.0};
    std::atomic<f64> display_latency_ms{0.0};
    bool vsync_enabled = true;
    bool hdr_enabled = false;
    bool variable_refresh_enabled = false;
    bool adaptive_sync_enabled = false;
    u32 current_refresh_rate = 60;
    u32 color_depth = 8;
    f32 brightness = 1.0f;
    f32 contrast = 1.0f;
    f32 saturation = 1.0f;
    f32 gamma_red = 1.0f;
    f32 gamma_green = 1.0f;
    f32 gamma_blue = 1.0f;
    String current_color_space = "sRGB";
    u32 connected_displays = 1;
    u64 pixel_clock_khz = 0;
    f64 memory_bandwidth_gbps = 0.0;
};

class AdvancedDisplayManager {
public:
    AdvancedDisplayManager();
    ~AdvancedDisplayManager();
    
    bool initialize(const DisplayConfig& config);
    void shutdown();
    
    bool set_display_mode(u32 width, u32 height, u32 refresh_rate);
    bool enable_display_output(u32 connector_id, bool enabled);
    bool set_display_position(u32 connector_id, Vec2 position);
    bool set_display_rotation(u32 connector_id, f32 rotation);
    bool set_display_scale(u32 connector_id, f32 scale);
    
    void enable_hdr(bool enabled);
    void enable_hdr10(bool enabled);
    void enable_dolby_vision(bool enabled);
    void set_hdr_metadata(const HDRMetadata& metadata);
    
    void enable_variable_refresh_rate(bool enabled);
    void set_refresh_rate_range(u32 min_rate, u32 max_rate);
    void enable_adaptive_sync(bool enabled);
    
    void enable_vsync(bool enabled);
    void set_swap_interval(u32 interval);
    
    void set_brightness(f32 brightness);
    void set_contrast(f32 contrast);
    void set_saturation(f32 saturation);
    void set_gamma(f32 red, f32 green, f32 blue);
    void set_color_temperature(f32 temperature);
    void set_color_space(const String& color_space);
    
    void enable_color_correction(bool enabled);
    void set_color_matrix(const Matrix3& matrix);
    void apply_color_profile(const String& profile_path);
    
    void enable_hardware_overlay(bool enabled);
    bool create_overlay_plane(const OverlayPlane& plane);
    void update_overlay_plane(u32 plane_id, const OverlayPlane& plane);
    void destroy_overlay_plane(u32 plane_id);
    
    void enable_hardware_cursor(bool enabled);
    void set_cursor_image(const void* image_data, u32 width, u32 height);
    void set_cursor_position(Vec2 position);
    void show_cursor(bool visible);
    
    void enable_page_flipping(bool enabled);
    void enable_atomic_modesetting(bool enabled);
    void enable_direct_scanout(bool enabled);
    
    Vector<DisplayMode> get_available_modes() const;
    Vector<DisplayOutput> get_connected_outputs() const;
    DisplayMode get_current_mode() const;
    DisplayStats get_display_stats() const;
    
    VkSurfaceKHR get_vulkan_surface() const;
    VkSwapchainKHR get_vulkan_swapchain() const;
    Vector<VkImage> get_swapchain_images() const;
    Vector<VkImageView> get_swapchain_image_views() const;
    
    void present_frame(VkCommandBuffer command_buffer);
    void wait_for_present();
    
    bool capture_screenshot(const String& filename);
    bool start_screen_recording(const String& filename);
    void stop_screen_recording();
    
    void calibrate_display_colors();
    void save_display_profile(const String& filename);
    void load_display_profile(const String& filename);
    
    void enable_debug_overlay(bool enabled);
    void set_debug_info_level(u32 level);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    bool initialize_drm();
    bool initialize_gbm();
    bool initialize_egl();
    bool initialize_vulkan();
    bool select_physical_device();
    bool create_logical_device();
    bool create_command_pool();
    
    bool detect_displays();
    void detect_hdr_capabilities(DisplayOutput& output);
    void detect_color_capabilities(DisplayOutput& output);
    void detect_variable_refresh_capabilities(DisplayOutput& output);
    
    bool configure_displays();
    bool create_swapchain();
    bool recreate_swapchain();
    void cleanup_swapchain();
    
    VkSurfaceFormatKHR choose_swap_surface_format(const Vector<VkSurfaceFormatKHR>& available_formats);
    VkPresentModeKHR choose_swap_present_mode(const Vector<VkPresentModeKHR>& available_modes);
    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
    bool create_image_views(VkFormat format);
    
    bool setup_advanced_features();
    void setup_hdr_metadata();
    void setup_color_management();
    void setup_overlay_planes();
    void setup_variable_refresh_rate();
    void setup_adaptive_sync();
    
    void start_display_thread();
    void stop_display_thread();
    void display_thread_loop();
    
    void update_brightness_control();
    void update_adaptive_sync();
    void monitor_display_health();
    void handle_display_connect(DisplayOutput& output);
    void handle_display_disconnect(DisplayOutput& output);
    void reconfigure_remaining_displays();
    
    void apply_color_correction();
    void apply_gamma_correction(f32 red, f32 green, f32 blue);
    void apply_brightness_change();
    
    void cleanup_drm();
    void cleanup_gbm();
    void cleanup_egl();
    void cleanup_vulkan();
};

Vector<String> get_available_color_profiles();
bool validate_display_mode(const DisplayMode& mode);
f32 calculate_display_dpi(const DisplayOutput& output, const DisplayMode& mode);
f32 calculate_pixel_density(const DisplayOutput& output, const DisplayMode& mode);
String format_display_name(const DisplayOutput& output);
String format_display_mode(const DisplayMode& mode);
bool is_hdr_capable_display(const DisplayOutput& output);
bool is_high_refresh_capable(const DisplayOutput& output);
Vec2 calculate_optimal_scaling(const DisplayOutput& output);

} // namespace S1U
