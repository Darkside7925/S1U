#include "s1u/window.hpp"
#include <cstring>
#include <algorithm>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <malloc.h>
#define aligned_alloc(alignment, size) _aligned_malloc(size, alignment)
#define free_aligned(ptr) _aligned_free(ptr)
#else
#define free_aligned(ptr) free(ptr)
#endif

namespace s1u {

WindowBuffer::WindowBuffer(u32 width, u32 height)
    : width_(width), height_(height), stride_(width * 4), size_(width * height * 4), damaged_(false) {
    data_ = aligned_alloc(64, size_);
    if (data_) {
        memset(data_, 0, size_);
#ifdef _WIN32
        // Windows doesn't have mlock equivalent for user mode
        // VirtualLock can be used but requires special privileges
#else
        mlock(data_, size_);
#endif
    }
}

WindowBuffer::~WindowBuffer() {
    if (data_) {
#ifdef _WIN32
        // No munlock equivalent needed for Windows stub
#else
        munlock(data_, size_);
#endif
        free_aligned(data_);
    }
}

Window::Window(const WindowAttributes& attrs) 
    : attrs_(attrs), needs_update_(false), needs_render_(false) {
    buffer_ = std::make_unique<WindowBuffer>(attrs.size.width, attrs.size.height);
    set_cpu_affinity(0);
    set_thread_priority(99);
}

Window::~Window() {
    if (buffer_) {
        buffer_.reset();
    }
}

void Window::set_title(const std::string& title) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.title = title;
    needs_update_ = true;
}

void Window::set_position(const Point& pos) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.position = pos;
    needs_update_ = true;
}

void Window::set_size(const Size& size) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (size.width != attrs_.size.width || size.height != attrs_.size.height) {
        attrs_.size = size;
        buffer_ = std::make_unique<WindowBuffer>(size.width, size.height);
        needs_update_ = true;
        needs_render_ = true;
    }
}

void Window::set_opacity(f32 opacity) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.opacity = std::clamp(opacity, 0.0f, 1.0f);
    needs_render_ = true;
}

void Window::set_visible(bool visible) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.visible = visible;
    needs_update_ = true;
}

void Window::set_enabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.enabled = enabled;
    needs_update_ = true;
}

void Window::set_focusable(bool focusable) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.focusable = focusable;
    needs_update_ = true;
}

void Window::set_resizable(bool resizable) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.resizable = resizable;
    needs_update_ = true;
}

void Window::set_minimizable(bool minimizable) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.minimizable = minimizable;
    needs_update_ = true;
}

void Window::set_maximizable(bool maximizable) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.maximizable = maximizable;
    needs_update_ = true;
}

void Window::set_closable(bool closable) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.closable = closable;
    needs_update_ = true;
}

void Window::set_always_on_top(bool always_on_top) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.always_on_top = always_on_top;
    needs_update_ = true;
}

void Window::set_skip_taskbar(bool skip_taskbar) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.skip_taskbar = skip_taskbar;
    needs_update_ = true;
}

void Window::set_skip_pager(bool skip_pager) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.skip_pager = skip_pager;
    needs_update_ = true;
}

void Window::set_urgent(bool urgent) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.urgent = urgent;
    needs_update_ = true;
}

void Window::set_modal(bool modal) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.modal = modal;
    needs_update_ = true;
}

void Window::set_transient(bool transient) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.transient = transient;
    needs_update_ = true;
}

void Window::set_parent(u32 parent_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.parent_id = parent_id;
    needs_update_ = true;
}

void Window::add_child(u32 child_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (std::find(attrs_.children.begin(), attrs_.children.end(), child_id) == attrs_.children.end()) {
        attrs_.children.push_back(child_id);
        needs_update_ = true;
    }
}

void Window::remove_child(u32 child_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find(attrs_.children.begin(), attrs_.children.end(), child_id);
    if (it != attrs_.children.end()) {
        attrs_.children.erase(it);
        needs_update_ = true;
    }
}

void Window::set_state(WindowState state) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.state = state;
    needs_update_ = true;
    needs_render_ = true;
}

void Window::set_background_color(const Color& color) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.background_color = color;
    needs_render_ = true;
}

void Window::set_icon(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.icon_path = path;
    needs_update_ = true;
}

void Window::add_protocol(u32 protocol) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (std::find(attrs_.protocols.begin(), attrs_.protocols.end(), protocol) == attrs_.protocols.end()) {
        attrs_.protocols.push_back(protocol);
        needs_update_ = true;
    }
}

void Window::remove_protocol(u32 protocol) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find(attrs_.protocols.begin(), attrs_.protocols.end(), protocol);
    if (it != attrs_.protocols.end()) {
        attrs_.protocols.erase(it);
        needs_update_ = true;
    }
}

void Window::set_decorated(bool decorated) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.decorated = decorated;
    needs_update_ = true;
    needs_render_ = true;
}

void Window::set_bordered(bool bordered) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.bordered = bordered;
    needs_update_ = true;
    needs_render_ = true;
}

void Window::set_shadowed(bool shadowed) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.shadowed = shadowed;
    needs_render_ = true;
}

void Window::set_blurred(bool blurred) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.blurred = blurred;
    needs_render_ = true;
}

void Window::set_transparent(bool transparent) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.transparent = transparent;
    needs_render_ = true;
}

void Window::set_compositor(u32 compositor_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.compositor_id = compositor_id;
    needs_update_ = true;
}

void Window::set_renderer(u32 renderer_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.renderer_id = renderer_id;
    needs_update_ = true;
}

void Window::set_buffer(u32 buffer_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.buffer_id = buffer_id;
    needs_update_ = true;
}

void Window::set_damaged(bool damaged) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.damaged = damaged;
    if (buffer_) {
        if (damaged) {
            buffer_->mark_damaged();
        } else {
            buffer_->clear_damage();
        }
    }
}

void Window::set_opaque(bool opaque) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.opaque = opaque;
    needs_render_ = true;
}

void Window::set_input(u32 input_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.input_id = input_id;
    needs_update_ = true;
}

void Window::set_output(u32 output_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.output_id = output_id;
    needs_update_ = true;
}

void Window::set_cursor(u32 cursor_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.cursor_id = cursor_id;
    needs_update_ = true;
}

void Window::set_pointer(u32 pointer_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.pointer_id = pointer_id;
    needs_update_ = true;
}

void Window::set_keyboard(u32 keyboard_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.keyboard_id = keyboard_id;
    needs_update_ = true;
}

void Window::set_touch(u32 touch_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.touch_id = touch_id;
    needs_update_ = true;
}

void Window::set_gesture(u32 gesture_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.gesture_id = gesture_id;
    needs_update_ = true;
}

void Window::set_drag(u32 drag_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.drag_id = drag_id;
    needs_update_ = true;
}

void Window::set_drop(u32 drop_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.drop_id = drop_id;
    needs_update_ = true;
}

void Window::set_selection(u32 selection_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.selection_id = selection_id;
    needs_update_ = true;
}

void Window::set_clipboard(u32 clipboard_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.clipboard_id = clipboard_id;
    needs_update_ = true;
}

void Window::set_dnd(u32 dnd_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.dnd_id = dnd_id;
    needs_update_ = true;
}

void Window::set_accessibility(u32 accessibility_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.accessibility_id = accessibility_id;
    needs_update_ = true;
}

void Window::set_ime(u32 ime_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.ime_id = ime_id;
    needs_update_ = true;
}

void Window::set_vulkan(u32 vulkan_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.vulkan_id = vulkan_id;
    needs_update_ = true;
}

void Window::set_opengl(u32 opengl_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.opengl_id = opengl_id;
    needs_update_ = true;
}

void Window::set_directx(u32 directx_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.directx_id = directx_id;
    needs_update_ = true;
}

void Window::set_metal(u32 metal_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.metal_id = metal_id;
    needs_update_ = true;
}

void Window::set_wayland(u32 wayland_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.wayland_id = wayland_id;
    needs_update_ = true;
}

void Window::set_x11(u32 x11_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.x11_id = x11_id;
    needs_update_ = true;
}

void Window::set_win32(u32 win32_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.win32_id = win32_id;
    needs_update_ = true;
}

void Window::set_cocoa(u32 cocoa_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.cocoa_id = cocoa_id;
    needs_update_ = true;
}

void Window::set_android(u32 android_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.android_id = android_id;
    needs_update_ = true;
}

void Window::set_ios(u32 ios_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.ios_id = ios_id;
    needs_update_ = true;
}

void Window::set_web(u32 web_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.web_id = web_id;
    needs_update_ = true;
}

void Window::set_embedded(u32 embedded_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.embedded_id = embedded_id;
    needs_update_ = true;
}

void Window::set_headless(u32 headless_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.headless_id = headless_id;
    needs_update_ = true;
}

void Window::set_virtual(u32 virtual_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.virtual_id = virtual_id;
    needs_update_ = true;
}

void Window::set_remote(u32 remote_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.remote_id = remote_id;
    needs_update_ = true;
}

void Window::set_network(u32 network_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.network_id = network_id;
    needs_update_ = true;
}

void Window::set_streaming(u32 streaming_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.streaming_id = streaming_id;
    needs_update_ = true;
}

void Window::set_recording(u32 recording_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.recording_id = recording_id;
    needs_update_ = true;
}

void Window::set_screenshot(u32 screenshot_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.screenshot_id = screenshot_id;
    needs_update_ = true;
}

void Window::set_video(u32 video_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.video_id = video_id;
    needs_update_ = true;
}

void Window::set_audio(u32 audio_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.audio_id = audio_id;
    needs_update_ = true;
}

void Window::set_gamepad(u32 gamepad_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.gamepad_id = gamepad_id;
    needs_update_ = true;
}

void Window::set_joystick(u32 joystick_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.joystick_id = joystick_id;
    needs_update_ = true;
}

void Window::set_tablet(u32 tablet_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.tablet_id = tablet_id;
    needs_update_ = true;
}

void Window::set_pen(u32 pen_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.pen_id = pen_id;
    needs_update_ = true;
}

void Window::set_eraser(u32 eraser_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.eraser_id = eraser_id;
    needs_update_ = true;
}

void Window::set_stylus(u32 stylus_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.stylus_id = stylus_id;
    needs_update_ = true;
}

void Window::show() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.visible = true;
    attrs_.state = WindowState::Normal;
    needs_update_ = true;
}

void Window::hide() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.visible = false;
    attrs_.state = WindowState::Hidden;
    needs_update_ = true;
}

void Window::raise() {
    std::lock_guard<std::mutex> lock(mutex_);
    needs_update_ = true;
}

void Window::lower() {
    std::lock_guard<std::mutex> lock(mutex_);
    needs_update_ = true;
}

void Window::stack() {
    std::lock_guard<std::mutex> lock(mutex_);
    needs_update_ = true;
}

void Window::unstack() {
    std::lock_guard<std::mutex> lock(mutex_);
    needs_update_ = true;
}

void Window::grab() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.state = WindowState::Grabbed;
    needs_update_ = true;
}

void Window::ungrab() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.state = WindowState::Normal;
    needs_update_ = true;
}

void Window::focus() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.state = WindowState::Focused;
    needs_update_ = true;
}

void Window::unfocus() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.state = WindowState::Normal;
    needs_update_ = true;
}

void Window::minimize() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.state = WindowState::Minimized;
    needs_update_ = true;
}

void Window::maximize() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.state = WindowState::Maximized;
    needs_update_ = true;
}

void Window::restore() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.state = WindowState::Normal;
    needs_update_ = true;
}

void Window::fullscreen() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.state = WindowState::Fullscreen;
    needs_update_ = true;
}

void Window::unfullscreen() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.state = WindowState::Normal;
    needs_update_ = true;
}

void Window::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    attrs_.state = WindowState::Hidden;
    needs_update_ = true;
}

void Window::handle_event(const Event& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    switch (event.type) {
        case EventType::WindowResize:
            set_size(event.size);
            break;
        case EventType::WindowMove:
            set_position(event.position);
            break;
        case EventType::WindowFocus:
            focus();
            break;
        case EventType::WindowUnfocus:
            unfocus();
            break;
        case EventType::WindowMaximize:
            maximize();
            break;
        case EventType::WindowMinimize:
            minimize();
            break;
        case EventType::WindowRestore:
            restore();
            break;
        case EventType::WindowFullscreen:
            fullscreen();
            break;
        case EventType::WindowUnfullscreen:
            unfullscreen();
            break;
        case EventType::WindowClose:
            close();
            break;
        default:
            break;
    }
}

void Window::render() {
    if (!buffer_ || !needs_render_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    u8* data = static_cast<u8*>(buffer_->get_data());
    u32 width = buffer_->get_width();
    u32 height = buffer_->get_height();
    u32 stride = buffer_->get_stride();
    
    for (u32 y = 0; y < height; ++y) {
        for (u32 x = 0; x < width; ++x) {
            u32 offset = y * stride + x * 4;
            data[offset + 0] = attrs_.background_color.r;
            data[offset + 1] = attrs_.background_color.g;
            data[offset + 2] = attrs_.background_color.b;
            data[offset + 3] = static_cast<u8>(attrs_.background_color.a * attrs_.opacity);
        }
    }
    
    buffer_->mark_damaged();
    needs_render_ = false;
}

void Window::update() {
    if (!needs_update_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    needs_update_ = false;
}

}
