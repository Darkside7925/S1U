#pragma once

#include "core.hpp"
#include "window.hpp"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <gbm.h>
#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

namespace s1u {

struct DisplayMode {
    u32 width;
    u32 height;
    u32 refresh_rate;
    u32 flags;
    std::string name;
};

struct DisplayInfo {
    u32 id;
    std::string name;
    std::string connector_type;
    u32 connector_id;
    u32 encoder_id;
    u32 crtc_id;
    u32 fb_id;
    bool connected;
    bool enabled;
    Point position;
    Size size;
    DisplayMode current_mode;
    std::vector<DisplayMode> modes;
    u32 gamma_size;
    u32 max_width;
    u32 max_height;
    u32 subpixel_order;
    u32 rotation;
    bool primary;
    bool active;
    bool cloned;
    bool mirrored;
    bool extended;
    bool internal;
    bool external;
    bool builtin;
    bool removable;
    bool hotpluggable;
    bool edid_present;
    std::vector<u8> edid;
    std::string manufacturer;
    std::string model;
    std::string serial;
    std::string product;
    std::string vendor;
    std::string device;
    std::string driver;
    std::string bus;
    std::string path;
    std::string sysfs_path;
    std::string udev_path;
    std::string drm_path;
    std::string gbm_path;
    std::string egl_path;
    std::string gl_path;
    std::string vulkan_path;
    std::string wayland_path;
    std::string x11_path;
    std::string win32_path;
    std::string cocoa_path;
    std::string android_path;
    std::string ios_path;
    std::string web_path;
    std::string embedded_path;
    std::string headless_path;
    std::string virtual_path;
    std::string remote_path;
    std::string network_path;
    std::string streaming_path;
    std::string recording_path;
    std::string screenshot_path;
    std::string video_path;
    std::string audio_path;
    std::string gamepad_path;
    std::string joystick_path;
    std::string tablet_path;
    std::string pen_path;
    std::string eraser_path;
    std::string stylus_path;
};

class DisplayBuffer {
public:
    DisplayBuffer(u32 width, u32 height, u32 format);
    ~DisplayBuffer();
    
    void* get_data() const { return data_; }
    u32 get_width() const { return width_; }
    u32 get_height() const { return height_; }
    u32 get_stride() const { return stride_; }
    u32 get_size() const { return size_; }
    u32 get_format() const { return format_; }
    u32 get_fb_id() const { return fb_id_; }
    bool is_damaged() const { return damaged_; }
    void mark_damaged() { damaged_ = true; }
    void clear_damage() { damaged_ = false; }
    
private:
    void* data_;
    u32 width_;
    u32 height_;
    u32 stride_;
    u32 size_;
    u32 format_;
    u32 fb_id_;
    bool damaged_;
};

class Display {
public:
    explicit Display(const DisplayInfo& info);
    ~Display();
    
    u32 get_id() const { return info_.id; }
    const DisplayInfo& get_info() const { return info_; }
    DisplayInfo& get_info() { return info_; }
    
    bool initialize();
    void shutdown();
    
    bool connect();
    void disconnect();
    bool enable();
    void disable();
    bool activate();
    void deactivate();
    
    void set_mode(const DisplayMode& mode);
    void set_position(const Point& pos);
    void set_size(const Size& size);
    void set_rotation(u32 rotation);
    void set_gamma(const std::vector<u16>& gamma);
    void set_brightness(f32 brightness);
    void set_contrast(f32 contrast);
    void set_saturation(f32 saturation);
    void set_hue(f32 hue);
    void set_temperature(f32 temperature);
    void set_gamma_ramp(const std::vector<u16>& red, const std::vector<u16>& green, const std::vector<u16>& blue);
    
    bool is_connected() const { return info_.connected; }
    bool is_enabled() const { return info_.enabled; }
    bool is_active() const { return info_.active; }
    bool is_primary() const { return info_.primary; }
    bool is_cloned() const { return info_.cloned; }
    bool is_mirrored() const { return info_.mirrored; }
    bool is_extended() const { return info_.extended; }
    bool is_internal() const { return info_.internal; }
    bool is_external() const { return info_.external; }
    bool is_builtin() const { return info_.builtin; }
    bool is_removable() const { return info_.removable; }
    bool is_hotpluggable() const { return info_.hotpluggable; }
    bool is_edid_present() const { return info_.edid_present; }
    
    const Point& get_position() const { return info_.position; }
    const Size& get_size() const { return info_.size; }
    const DisplayMode& get_current_mode() const { return info_.current_mode; }
    const std::vector<DisplayMode>& get_modes() const { return info_.modes; }
    u32 get_gamma_size() const { return info_.gamma_size; }
    u32 get_max_width() const { return info_.max_width; }
    u32 get_max_height() const { return info_.max_height; }
    u32 get_subpixel_order() const { return info_.subpixel_order; }
    u32 get_rotation() const { return info_.rotation; }
    const std::string& get_name() const { return info_.name; }
    const std::string& get_connector_type() const { return info_.connector_type; }
    const std::string& get_manufacturer() const { return info_.manufacturer; }
    const std::string& get_model() const { return info_.model; }
    const std::string& get_serial() const { return info_.serial; }
    const std::string& get_product() const { return info_.product; }
    const std::string& get_vendor() const { return info_.vendor; }
    const std::string& get_device() const { return info_.device; }
    const std::string& get_driver() const { return info_.driver; }
    const std::string& get_bus() const { return info_.bus; }
    const std::string& get_path() const { return info_.path; }
    const std::string& get_sysfs_path() const { return info_.sysfs_path; }
    const std::string& get_udev_path() const { return info_.udev_path; }
    const std::string& get_drm_path() const { return info_.drm_path; }
    const std::string& get_gbm_path() const { return info_.gbm_path; }
    const std::string& get_egl_path() const { return info_.egl_path; }
    const std::string& get_gl_path() const { return info_.gl_path; }
    const std::string& get_vulkan_path() const { return info_.vulkan_path; }
    const std::string& get_wayland_path() const { return info_.wayland_path; }
    const std::string& get_x11_path() const { return info_.x11_path; }
    const std::string& get_win32_path() const { return info_.win32_path; }
    const std::string& get_cocoa_path() const { return info_.cocoa_path; }
    const std::string& get_android_path() const { return info_.android_path; }
    const std::string& get_ios_path() const { return info_.ios_path; }
    const std::string& get_web_path() const { return info_.web_path; }
    const std::string& get_embedded_path() const { return info_.embedded_path; }
    const std::string& get_headless_path() const { return info_.headless_path; }
    const std::string& get_virtual_path() const { return info_.virtual_path; }
    const std::string& get_remote_path() const { return info_.remote_path; }
    const std::string& get_network_path() const { return info_.network_path; }
    const std::string& get_streaming_path() const { return info_.streaming_path; }
    const std::string& get_recording_path() const { return info_.recording_path; }
    const std::string& get_screenshot_path() const { return info_.screenshot_path; }
    const std::string& get_video_path() const { return info_.video_path; }
    const std::string& get_audio_path() const { return info_.audio_path; }
    const std::string& get_gamepad_path() const { return info_.gamepad_path; }
    const std::string& get_joystick_path() const { return info_.joystick_path; }
    const std::string& get_tablet_path() const { return info_.tablet_path; }
    const std::string& get_pen_path() const { return info_.pen_path; }
    const std::string& get_eraser_path() const { return info_.eraser_path; }
    const std::string& get_stylus_path() const { return info_.stylus_path; }
    
    void render();
    void flip();
    void page_flip();
    void vsync();
    void hsync();
    void blank();
    void unblank();
    void power_on();
    void power_off();
    void suspend();
    void resume();
    void reset();
    void calibrate();
    void test();
    void diagnose();
    void optimize();
    void profile();
    void benchmark();
    void stress_test();
    void burn_in();
    void burn_out();
    
private:
    DisplayInfo info_;
    int drm_fd_;
    int gbm_fd_;
    struct gbm_device* gbm_device_;
    struct gbm_surface* gbm_surface_;
    EGLDisplay egl_display_;
    EGLContext egl_context_;
    EGLSurface egl_surface_;
    std::unique_ptr<DisplayBuffer> buffer_;
    std::mutex mutex_;
    std::atomic<bool> needs_render_;
    std::atomic<bool> needs_flip_;
    std::atomic<bool> vsync_enabled_;
    std::atomic<bool> hsync_enabled_;
    std::atomic<bool> blanked_;
    std::atomic<bool> powered_on_;
    std::atomic<bool> suspended_;
    std::atomic<bool> calibrated_;
    std::atomic<bool> tested_;
    std::atomic<bool> diagnosed_;
    std::atomic<bool> optimized_;
    std::atomic<bool> profiled_;
    std::atomic<bool> benchmarked_;
    std::atomic<bool> stress_tested_;
    std::atomic<bool> burn_in_;
    std::atomic<bool> burn_out_;
};

}
