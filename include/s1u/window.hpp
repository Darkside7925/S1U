#pragma once

#include "core.hpp"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <gbm.h>
#include <drm/drm.h>
#include <drm/drm_mode.h>

namespace s1u {

enum class WindowState : u32 {
    Normal = 0,
    Minimized = 1,
    Maximized = 2,
    Fullscreen = 3,
    Hidden = 4,
    Modal = 5,
    Transient = 6,
    Urgent = 7,
    AlwaysOnTop = 8,
    SkipTaskbar = 9,
    SkipPager = 10,
    Sticky = 11,
    DemandsAttention = 12,
    Focused = 13,
    Grabbed = 14,
    Resizable = 15,
    Minimizable = 16,
    Maximizable = 17,
    Closable = 18,
    Shaded = 19,
    Above = 20,
    Below = 21,
    Tooltip = 22,
    Notification = 23,
    Dock = 24,
    Desktop = 25,
    Menu = 26,
    Dialog = 27,
    Splash = 28,
    Utility = 29,
    Dropdown = 30,
    Popup = 31,
    Toolbar = 32,
    Status = 33,
    Progress = 34,
    Input = 35,
    Output = 36,
    Overlay = 37,
    Background = 38,
    Cursor = 39,
    Drag = 40,
    Drop = 41,
    Selection = 42,
    Clipboard = 43,
    Dnd = 44,
    Accessibility = 45,
    Ime = 46,
    Vulkan = 47,
    OpenGL = 48,
    DirectX = 49,
    Metal = 50,
    Wayland = 51,
    X11 = 52,
    Win32 = 53,
    Cocoa = 54,
    Android = 55,
    IOS = 56,
    Web = 57,
    Embedded = 58,
    Headless = 59,
    Virtual = 60,
    Remote = 61,
    Network = 62,
    Streaming = 63,
    Recording = 64,
    Screenshot = 65,
    Video = 66,
    Audio = 67,
    Gamepad = 68,
    Joystick = 69,
    Tablet = 70,
    Pen = 71,
    Eraser = 72,
    Stylus = 73
};

struct WindowAttributes {
    u32 id;
    std::string title;
    Point position;
    Size size;
    Size min_size;
    Size max_size;
    f32 opacity;
    bool visible;
    bool enabled;
    bool focusable;
    bool resizable;
    bool minimizable;
    bool maximizable;
    bool closable;
    bool always_on_top;
    bool skip_taskbar;
    bool skip_pager;
    bool urgent;
    bool modal;
    bool transient;
    u32 parent_id;
    std::vector<u32> children;
    WindowState state;
    Color background_color;
    std::string icon_path;
    std::vector<u32> protocols;
    bool decorated;
    bool bordered;
    bool shadowed;
    bool blurred;
    bool transparent;
    u32 compositor_id;
    u32 renderer_id;
    u32 buffer_id;
    bool damaged;
    bool opaque;
    u32 input_id;
    u32 output_id;
    u32 cursor_id;
    u32 pointer_id;
    u32 keyboard_id;
    u32 touch_id;
    u32 gesture_id;
    u32 drag_id;
    u32 drop_id;
    u32 selection_id;
    u32 clipboard_id;
    u32 dnd_id;
    u32 accessibility_id;
    u32 ime_id;
    u32 vulkan_id;
    u32 opengl_id;
    u32 directx_id;
    u32 metal_id;
    u32 wayland_id;
    u32 x11_id;
    u32 win32_id;
    u32 cocoa_id;
    u32 android_id;
    u32 ios_id;
    u32 web_id;
    u32 embedded_id;
    u32 headless_id;
    u32 virtual_id;
    u32 remote_id;
    u32 network_id;
    u32 streaming_id;
    u32 recording_id;
    u32 screenshot_id;
    u32 video_id;
    u32 audio_id;
    u32 gamepad_id;
    u32 joystick_id;
    u32 tablet_id;
    u32 pen_id;
    u32 eraser_id;
    u32 stylus_id;
};

class WindowBuffer {
public:
    WindowBuffer(u32 width, u32 height);
    ~WindowBuffer();
    
    void* get_data() const { return data_; }
    u32 get_width() const { return width_; }
    u32 get_height() const { return height_; }
    u32 get_stride() const { return stride_; }
    u32 get_size() const { return size_; }
    bool is_damaged() const { return damaged_; }
    void mark_damaged() { damaged_ = true; }
    void clear_damage() { damaged_ = false; }
    
private:
    void* data_;
    u32 width_;
    u32 height_;
    u32 stride_;
    u32 size_;
    bool damaged_;
};

class Window {
public:
    explicit Window(const WindowAttributes& attrs);
    ~Window();
    
    u32 get_id() const { return attrs_.id; }
    const WindowAttributes& get_attributes() const { return attrs_; }
    WindowAttributes& get_attributes() { return attrs_; }
    
    void set_title(const std::string& title);
    void set_position(const Point& pos);
    void set_size(const Size& size);
    void set_opacity(f32 opacity);
    void set_visible(bool visible);
    void set_enabled(bool enabled);
    void set_focusable(bool focusable);
    void set_resizable(bool resizable);
    void set_minimizable(bool minimizable);
    void set_maximizable(bool maximizable);
    void set_closable(bool closable);
    void set_always_on_top(bool always_on_top);
    void set_skip_taskbar(bool skip_taskbar);
    void set_skip_pager(bool skip_pager);
    void set_urgent(bool urgent);
    void set_modal(bool modal);
    void set_transient(bool transient);
    void set_parent(u32 parent_id);
    void add_child(u32 child_id);
    void remove_child(u32 child_id);
    void set_state(WindowState state);
    void set_background_color(const Color& color);
    void set_icon(const std::string& path);
    void add_protocol(u32 protocol);
    void remove_protocol(u32 protocol);
    void set_decorated(bool decorated);
    void set_bordered(bool bordered);
    void set_shadowed(bool shadowed);
    void set_blurred(bool blurred);
    void set_transparent(bool transparent);
    void set_compositor(u32 compositor_id);
    void set_renderer(u32 renderer_id);
    void set_buffer(u32 buffer_id);
    void set_damaged(bool damaged);
    void set_opaque(bool opaque);
    void set_input(u32 input_id);
    void set_output(u32 output_id);
    void set_cursor(u32 cursor_id);
    void set_pointer(u32 pointer_id);
    void set_keyboard(u32 keyboard_id);
    void set_touch(u32 touch_id);
    void set_gesture(u32 gesture_id);
    void set_drag(u32 drag_id);
    void set_drop(u32 drop_id);
    void set_selection(u32 selection_id);
    void set_clipboard(u32 clipboard_id);
    void set_dnd(u32 dnd_id);
    void set_accessibility(u32 accessibility_id);
    void set_ime(u32 ime_id);
    void set_vulkan(u32 vulkan_id);
    void set_opengl(u32 opengl_id);
    void set_directx(u32 directx_id);
    void set_metal(u32 metal_id);
    void set_wayland(u32 wayland_id);
    void set_x11(u32 x11_id);
    void set_win32(u32 win32_id);
    void set_cocoa(u32 cocoa_id);
    void set_android(u32 android_id);
    void set_ios(u32 ios_id);
    void set_web(u32 web_id);
    void set_embedded(u32 embedded_id);
    void set_headless(u32 headless_id);
    void set_virtual(u32 virtual_id);
    void set_remote(u32 remote_id);
    void set_network(u32 network_id);
    void set_streaming(u32 streaming_id);
    void set_recording(u32 recording_id);
    void set_screenshot(u32 screenshot_id);
    void set_video(u32 video_id);
    void set_audio(u32 audio_id);
    void set_gamepad(u32 gamepad_id);
    void set_joystick(u32 joystick_id);
    void set_tablet(u32 tablet_id);
    void set_pen(u32 pen_id);
    void set_eraser(u32 eraser_id);
    void set_stylus(u32 stylus_id);
    
    void show();
    void hide();
    void raise();
    void lower();
    void stack();
    void unstack();
    void grab();
    void ungrab();
    void focus();
    void unfocus();
    void minimize();
    void maximize();
    void restore();
    void fullscreen();
    void unfullscreen();
    void close();
    
    bool is_visible() const { return attrs_.visible; }
    bool is_enabled() const { return attrs_.enabled; }
    bool is_focusable() const { return attrs_.focusable; }
    bool is_resizable() const { return attrs_.resizable; }
    bool is_minimizable() const { return attrs_.minimizable; }
    bool is_maximizable() const { return attrs_.maximizable; }
    bool is_closable() const { return attrs_.closable; }
    bool is_always_on_top() const { return attrs_.always_on_top; }
    bool is_skip_taskbar() const { return attrs_.skip_taskbar; }
    bool is_skip_pager() const { return attrs_.skip_pager; }
    bool is_urgent() const { return attrs_.urgent; }
    bool is_modal() const { return attrs_.modal; }
    bool is_transient() const { return attrs_.transient; }
    bool is_decorated() const { return attrs_.decorated; }
    bool is_bordered() const { return attrs_.bordered; }
    bool is_shadowed() const { return attrs_.shadowed; }
    bool is_blurred() const { return attrs_.blurred; }
    bool is_transparent() const { return attrs_.transparent; }
    bool is_damaged() const { return attrs_.damaged; }
    bool is_opaque() const { return attrs_.opaque; }
    
    WindowState get_state() const { return attrs_.state; }
    const Point& get_position() const { return attrs_.position; }
    const Size& get_size() const { return attrs_.size; }
    const Size& get_min_size() const { return attrs_.min_size; }
    const Size& get_max_size() const { return attrs_.max_size; }
    f32 get_opacity() const { return attrs_.opacity; }
    const Color& get_background_color() const { return attrs_.background_color; }
    const std::string& get_title() const { return attrs_.title; }
    const std::string& get_icon_path() const { return attrs_.icon_path; }
    u32 get_parent_id() const { return attrs_.parent_id; }
    const std::vector<u32>& get_children() const { return attrs_.children; }
    const std::vector<u32>& get_protocols() const { return attrs_.protocols; }
    
    void handle_event(const Event& event);
    void render();
    void update();
    
private:
    WindowAttributes attrs_;
    std::unique_ptr<WindowBuffer> buffer_;
    std::mutex mutex_;
    std::atomic<bool> needs_update_;
    std::atomic<bool> needs_render_;
};

}
