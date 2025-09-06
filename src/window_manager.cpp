#include "s1u/window_manager.hpp"
#include "s1u/renderer.hpp"
#include <iostream>
#include <algorithm>
#include <climits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

Window::Window(const WindowProperties& properties)
    : properties_(properties)
    , created_(false)
    , focused_(false) {
}

Window::~Window() {
    destroy();
}

bool Window::create() {
    if (created_) {
        return true;
    }
    
    created_ = true;
    std::cout << "[S1U] Created window: " << properties_.title << std::endl;
    return true;
}

void Window::destroy() {
    if (!created_) {
        return;
    }
    
    created_ = false;
    std::cout << "[S1U] Destroyed window: " << properties_.title << std::endl;
}

void Window::show() {
    properties_.visible = true;
}

void Window::hide() {
    properties_.visible = false;
}

void Window::close() {
    properties_.visible = false;
    created_ = false;
}

void Window::set_title(const std::string& title) {
    properties_.title = title;
}

void Window::set_size(uint32_t width, uint32_t height) {
    properties_.width = width;
    properties_.height = height;
}

void Window::set_position(int32_t x, int32_t y) {
    properties_.x = x;
    properties_.y = y;
}

void Window::set_state(WindowState state) {
    properties_.state = state;
}

void Window::set_opacity(float opacity) {
    properties_.opacity = std::clamp(opacity, 0.0f, 1.0f);
}

void Window::render(std::shared_ptr<Renderer> renderer) {
    if (!created_ || !properties_.visible || !renderer) {
        return;
    }
    
    // Render window background
    Rect window_rect(properties_.x, properties_.y, properties_.width, properties_.height);
    Color bg_color(0.2f, 0.2f, 0.25f, properties_.opacity);
    renderer->draw_rect(window_rect, bg_color);
    
    // Render window border
    Color border_color = focused_ ? Color(0.4f, 0.6f, 1.0f, 1.0f) : Color(0.3f, 0.3f, 0.35f, 1.0f);
    renderer->draw_rect_outline(window_rect, border_color, 2.0f);
    
    // Render window title bar
    Rect title_bar_rect(properties_.x, properties_.y, properties_.width, 30);
    Color title_bg_color = focused_ ? Color(0.3f, 0.5f, 0.8f, 1.0f) : Color(0.25f, 0.25f, 0.3f, 1.0f);
    renderer->draw_rect(title_bar_rect, title_bg_color);
    
    // Render window title
    Point title_pos(properties_.x + 10, properties_.y + 8);
    Color title_color(1.0f, 1.0f, 1.0f, 1.0f);
    renderer->draw_text(properties_.title, title_pos, title_color, 14.0f);
    
    // Render child windows
    for (auto& child : child_windows_) {
        if (child && child->is_visible()) {
            child->render(renderer);
        }
    }
}

void Window::update(double delta_time) {
    if (!created_ || !properties_.visible) {
        return;
    }
    
    // Update child windows
    for (auto& child : child_windows_) {
        if (child) {
            child->update(delta_time);
        }
    }
}

void Window::on_focus() {
    focused_ = true;
}

void Window::on_lose_focus() {
    focused_ = false;
}

void Window::on_resize(uint32_t width, uint32_t height) {
    properties_.width = width;
    properties_.height = height;
}

void Window::on_move(int32_t x, int32_t y) {
    properties_.x = x;
    properties_.y = y;
}

void Window::on_close() {
    close();
}

void Window::set_su1_app_name(const std::string& app_name) {
    su1_app_name_ = app_name;
}

// WindowManager implementation
WindowManager::WindowManager()
    : next_window_id_(1) {
}

WindowManager::~WindowManager() {
    shutdown();
}

bool WindowManager::initialize() {
    std::cout << "[S1U] Initializing Window Manager..." << std::endl;
    
    // Create desktop window
    WindowProperties desktop_props;
    desktop_props.title = "Desktop";
    desktop_props.width = 1920;
    desktop_props.height = 1080;
    desktop_props.x = 0;
    desktop_props.y = 0;
    desktop_props.type = WindowType::Desktop;
    desktop_props.state = WindowState::Normal;
    desktop_props.resizable = false;
    desktop_props.movable = false;
    desktop_props.closable = false;
    desktop_props.minimizable = false;
    desktop_props.maximizable = false;
    desktop_props.always_on_top = true;
    desktop_props.opacity = 1.0f;
    desktop_props.decorated = false;
    desktop_props.visible = true;
    
    auto desktop_window = create_window(desktop_props);
    if (desktop_window) {
        desktop_window->create();
    }
    
    std::cout << "[S1U] Window Manager initialized successfully!" << std::endl;
    return true;
}

void WindowManager::shutdown() {
    std::cout << "[S1U] Shutting down Window Manager..." << std::endl;
    
    // Destroy all windows
    for (auto& [id, window] : windows_) {
        if (window) {
            window->destroy();
        }
    }
    
    windows_.clear();
    su1_windows_.clear();
    focused_window_.reset();
}

std::shared_ptr<Window> WindowManager::create_window(const WindowProperties& properties) {
    auto window = std::make_shared<Window>(properties);
    uint32_t id = generate_window_id();
    
    windows_[id] = window;
    
    if (properties.type == WindowType::Desktop) {
        // Desktop window is always visible and focused
        window->on_focus();
    }
    
    std::cout << "[S1U] Created window with ID: " << id << " - " << properties.title << std::endl;
    return window;
}

void WindowManager::destroy_window(std::shared_ptr<Window> window) {
    if (!window) return;
    
    // Remove from windows map
    for (auto it = windows_.begin(); it != windows_.end(); ++it) {
        if (it->second == window) {
            windows_.erase(it);
            break;
        }
    }
    
    // Remove from SU1 windows if it's a SU1 window
    for (auto it = su1_windows_.begin(); it != su1_windows_.end(); ++it) {
        if (it->second == window) {
            su1_windows_.erase(it);
            break;
        }
    }
    
    // Clear focus if this was the focused window
    if (focused_window_ == window) {
        focused_window_.reset();
    }
    
    window->destroy();
}

void WindowManager::destroy_window(uint32_t window_id) {
    auto it = windows_.find(window_id);
    if (it != windows_.end()) {
        destroy_window(it->second);
    }
}

std::shared_ptr<Window> WindowManager::get_window(uint32_t window_id) const {
    auto it = windows_.find(window_id);
    return (it != windows_.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Window>> WindowManager::get_all_windows() const {
    std::vector<std::shared_ptr<Window>> result;
    for (const auto& [id, window] : windows_) {
        if (window) {
            result.push_back(window);
        }
    }
    return result;
}

std::shared_ptr<Window> WindowManager::get_focused_window() const {
    return focused_window_;
}

std::shared_ptr<Window> WindowManager::get_window_at_position(int32_t x, int32_t y) const {
    // Find the topmost window at the given position
    // Convert to vector and reverse iterate to check from top to bottom
    std::vector<std::pair<uint32_t, std::shared_ptr<Window>>> window_list(windows_.begin(), windows_.end());
    for (auto it = window_list.rbegin(); it != window_list.rend(); ++it) {
        auto& window = it->second;
        if (window && window->is_visible()) {
            const auto& props = window->get_properties();
            if (x >= props.x && x < props.x + static_cast<int32_t>(props.width) &&
                y >= props.y && y < props.y + static_cast<int32_t>(props.height)) {
                return window;
            }
        }
    }
    return nullptr;
}

void WindowManager::focus_window(std::shared_ptr<Window> window) {
    if (!window) return;
    
    // Remove focus from current focused window
    if (focused_window_) {
        focused_window_->on_lose_focus();
    }
    
    // Set new focused window
    focused_window_ = window;
    window->on_focus();
    
    // Bring window to front
    bring_window_to_front(window);
}

void WindowManager::minimize_window(std::shared_ptr<Window> window) {
    if (window) {
        window->set_state(WindowState::Minimized);
    }
}

void WindowManager::maximize_window(std::shared_ptr<Window> window) {
    if (window) {
        window->set_state(WindowState::Maximized);
    }
}

void WindowManager::restore_window(std::shared_ptr<Window> window) {
    if (window) {
        window->set_state(WindowState::Normal);
    }
}

void WindowManager::close_window(std::shared_ptr<Window> window) {
    if (window) {
        window->close();
        destroy_window(window);
    }
}

void WindowManager::arrange_windows() {
    // Simple tiling arrangement
    int x = 0, y = 0;
    int max_width = 1920 / 2; // Split screen in half
    
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            window->set_position(x, y);
            window->set_size(max_width, 600);
            
            x += max_width;
            if (x >= 1920) {
                x = 0;
                y += 600;
            }
        }
    }
}

void WindowManager::tile_windows() {
    arrange_windows();
}

void WindowManager::cascade_windows() {
    int x = 50, y = 50;
    int offset = 30;
    
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            window->set_position(x, y);
            x += offset;
            y += offset;
        }
    }
}

void WindowManager::minimize_all() {
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            minimize_window(window);
        }
    }
}

void WindowManager::restore_all() {
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            restore_window(window);
        }
    }
}

std::shared_ptr<Window> WindowManager::create_su1_window(const std::string& app_name, const WindowProperties& properties) {
    auto window = create_window(properties);
    if (window) {
        window->set_su1_app_name(app_name);
        su1_windows_[app_name] = window;
        std::cout << "[S1U] Created SU1 window for application: " << app_name << std::endl;
    }
    return window;
}

void WindowManager::update_su1_windows() {
    for (auto& [name, window] : su1_windows_) {
        if (window) {
            window->update(0.016); // 60 FPS update
        }
    }
}

std::vector<std::string> WindowManager::get_su1_app_names() const {
    std::vector<std::string> names;
    for (const auto& [name, window] : su1_windows_) {
        names.push_back(name);
    }
    return names;
}

void WindowManager::render_windows(std::shared_ptr<Renderer> renderer) {
    if (!renderer) return;
    
    // Render all windows
    for (auto& [id, window] : windows_) {
        if (window && window->is_visible()) {
            window->render(renderer);
        }
    }
}

void WindowManager::update_windows(double delta_time) {
    for (auto& [id, window] : windows_) {
        if (window) {
            window->update(delta_time);
        }
    }
    
    // Update SU1 windows specifically
    update_su1_windows();
}

void WindowManager::handle_window_events() {
    // Handle window events (resize, move, etc.)
    // This would typically be called from the input manager
}

uint32_t WindowManager::generate_window_id() {
    return next_window_id_++;
}

void WindowManager::update_window_focus() {
    // Update window focus based on mouse position or other criteria
    // This is a simplified implementation
}

void WindowManager::cleanup_destroyed_windows() {
    // Remove windows that have been marked for destruction
    for (auto it = windows_.begin(); it != windows_.end();) {
        if (!it->second || !it->second->is_visible()) {
            it = windows_.erase(it);
        } else {
            ++it;
        }
    }
}

void WindowManager::bring_window_to_front(std::shared_ptr<Window> window) {
    // Bring window to front by reordering in the windows map
    // This is a simplified implementation
}

} // namespace s1u

#include <iostream>
#include <algorithm>
#include <climits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

Window::Window(const WindowProperties& properties)
    : properties_(properties)
    , created_(false)
    , focused_(false) {
}

Window::~Window() {
    destroy();
}

bool Window::create() {
    if (created_) {
        return true;
    }
    
    created_ = true;
    std::cout << "[S1U] Created window: " << properties_.title << std::endl;
    return true;
}

void Window::destroy() {
    if (!created_) {
        return;
    }
    
    created_ = false;
    std::cout << "[S1U] Destroyed window: " << properties_.title << std::endl;
}

void Window::show() {
    properties_.visible = true;
}

void Window::hide() {
    properties_.visible = false;
}

void Window::close() {
    properties_.visible = false;
    created_ = false;
}

void Window::set_title(const std::string& title) {
    properties_.title = title;
}

void Window::set_size(uint32_t width, uint32_t height) {
    properties_.width = width;
    properties_.height = height;
}

void Window::set_position(int32_t x, int32_t y) {
    properties_.x = x;
    properties_.y = y;
}

void Window::set_state(WindowState state) {
    properties_.state = state;
}

void Window::set_opacity(float opacity) {
    properties_.opacity = std::clamp(opacity, 0.0f, 1.0f);
}

void Window::render(std::shared_ptr<Renderer> renderer) {
    if (!created_ || !properties_.visible || !renderer) {
        return;
    }
    
    // Render window background
    Rect window_rect(properties_.x, properties_.y, properties_.width, properties_.height);
    Color bg_color(0.2f, 0.2f, 0.25f, properties_.opacity);
    renderer->draw_rect(window_rect, bg_color);
    
    // Render window border
    Color border_color = focused_ ? Color(0.4f, 0.6f, 1.0f, 1.0f) : Color(0.3f, 0.3f, 0.35f, 1.0f);
    renderer->draw_rect_outline(window_rect, border_color, 2.0f);
    
    // Render window title bar
    Rect title_bar_rect(properties_.x, properties_.y, properties_.width, 30);
    Color title_bg_color = focused_ ? Color(0.3f, 0.5f, 0.8f, 1.0f) : Color(0.25f, 0.25f, 0.3f, 1.0f);
    renderer->draw_rect(title_bar_rect, title_bg_color);
    
    // Render window title
    Point title_pos(properties_.x + 10, properties_.y + 8);
    Color title_color(1.0f, 1.0f, 1.0f, 1.0f);
    renderer->draw_text(properties_.title, title_pos, title_color, 14.0f);
    
    // Render child windows
    for (auto& child : child_windows_) {
        if (child && child->is_visible()) {
            child->render(renderer);
        }
    }
}

void Window::update(double delta_time) {
    if (!created_ || !properties_.visible) {
        return;
    }
    
    // Update child windows
    for (auto& child : child_windows_) {
        if (child) {
            child->update(delta_time);
        }
    }
}

void Window::on_focus() {
    focused_ = true;
}

void Window::on_lose_focus() {
    focused_ = false;
}

void Window::on_resize(uint32_t width, uint32_t height) {
    properties_.width = width;
    properties_.height = height;
}

void Window::on_move(int32_t x, int32_t y) {
    properties_.x = x;
    properties_.y = y;
}

void Window::on_close() {
    close();
}

void Window::set_su1_app_name(const std::string& app_name) {
    su1_app_name_ = app_name;
}

// WindowManager implementation
WindowManager::WindowManager()
    : next_window_id_(1) {
}

WindowManager::~WindowManager() {
    shutdown();
}

bool WindowManager::initialize() {
    std::cout << "[S1U] Initializing Window Manager..." << std::endl;
    
    // Create desktop window
    WindowProperties desktop_props;
    desktop_props.title = "Desktop";
    desktop_props.width = 1920;
    desktop_props.height = 1080;
    desktop_props.x = 0;
    desktop_props.y = 0;
    desktop_props.type = WindowType::Desktop;
    desktop_props.state = WindowState::Normal;
    desktop_props.resizable = false;
    desktop_props.movable = false;
    desktop_props.closable = false;
    desktop_props.minimizable = false;
    desktop_props.maximizable = false;
    desktop_props.always_on_top = true;
    desktop_props.opacity = 1.0f;
    desktop_props.decorated = false;
    desktop_props.visible = true;
    
    auto desktop_window = create_window(desktop_props);
    if (desktop_window) {
        desktop_window->create();
    }
    
    std::cout << "[S1U] Window Manager initialized successfully!" << std::endl;
    return true;
}

void WindowManager::shutdown() {
    std::cout << "[S1U] Shutting down Window Manager..." << std::endl;
    
    // Destroy all windows
    for (auto& [id, window] : windows_) {
        if (window) {
            window->destroy();
        }
    }
    
    windows_.clear();
    su1_windows_.clear();
    focused_window_.reset();
}

std::shared_ptr<Window> WindowManager::create_window(const WindowProperties& properties) {
    auto window = std::make_shared<Window>(properties);
    uint32_t id = generate_window_id();
    
    windows_[id] = window;
    
    if (properties.type == WindowType::Desktop) {
        // Desktop window is always visible and focused
        window->on_focus();
    }
    
    std::cout << "[S1U] Created window with ID: " << id << " - " << properties.title << std::endl;
    return window;
}

void WindowManager::destroy_window(std::shared_ptr<Window> window) {
    if (!window) return;
    
    // Remove from windows map
    for (auto it = windows_.begin(); it != windows_.end(); ++it) {
        if (it->second == window) {
            windows_.erase(it);
            break;
        }
    }
    
    // Remove from SU1 windows if it's a SU1 window
    for (auto it = su1_windows_.begin(); it != su1_windows_.end(); ++it) {
        if (it->second == window) {
            su1_windows_.erase(it);
            break;
        }
    }
    
    // Clear focus if this was the focused window
    if (focused_window_ == window) {
        focused_window_.reset();
    }
    
    window->destroy();
}

void WindowManager::destroy_window(uint32_t window_id) {
    auto it = windows_.find(window_id);
    if (it != windows_.end()) {
        destroy_window(it->second);
    }
}

std::shared_ptr<Window> WindowManager::get_window(uint32_t window_id) const {
    auto it = windows_.find(window_id);
    return (it != windows_.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Window>> WindowManager::get_all_windows() const {
    std::vector<std::shared_ptr<Window>> result;
    for (const auto& [id, window] : windows_) {
        if (window) {
            result.push_back(window);
        }
    }
    return result;
}

std::shared_ptr<Window> WindowManager::get_focused_window() const {
    return focused_window_;
}

std::shared_ptr<Window> WindowManager::get_window_at_position(int32_t x, int32_t y) const {
    // Find the topmost window at the given position
    // Convert to vector and reverse iterate to check from top to bottom
    std::vector<std::pair<uint32_t, std::shared_ptr<Window>>> window_list(windows_.begin(), windows_.end());
    for (auto it = window_list.rbegin(); it != window_list.rend(); ++it) {
        auto& window = it->second;
        if (window && window->is_visible()) {
            const auto& props = window->get_properties();
            if (x >= props.x && x < props.x + static_cast<int32_t>(props.width) &&
                y >= props.y && y < props.y + static_cast<int32_t>(props.height)) {
                return window;
            }
        }
    }
    return nullptr;
}

void WindowManager::focus_window(std::shared_ptr<Window> window) {
    if (!window) return;
    
    // Remove focus from current focused window
    if (focused_window_) {
        focused_window_->on_lose_focus();
    }
    
    // Set new focused window
    focused_window_ = window;
    window->on_focus();
    
    // Bring window to front
    bring_window_to_front(window);
}

void WindowManager::minimize_window(std::shared_ptr<Window> window) {
    if (window) {
        window->set_state(WindowState::Minimized);
    }
}

void WindowManager::maximize_window(std::shared_ptr<Window> window) {
    if (window) {
        window->set_state(WindowState::Maximized);
    }
}

void WindowManager::restore_window(std::shared_ptr<Window> window) {
    if (window) {
        window->set_state(WindowState::Normal);
    }
}

void WindowManager::close_window(std::shared_ptr<Window> window) {
    if (window) {
        window->close();
        destroy_window(window);
    }
}

void WindowManager::arrange_windows() {
    // Simple tiling arrangement
    int x = 0, y = 0;
    int max_width = 1920 / 2; // Split screen in half
    
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            window->set_position(x, y);
            window->set_size(max_width, 600);
            
            x += max_width;
            if (x >= 1920) {
                x = 0;
                y += 600;
            }
        }
    }
}

void WindowManager::tile_windows() {
    arrange_windows();
}

void WindowManager::cascade_windows() {
    int x = 50, y = 50;
    int offset = 30;
    
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            window->set_position(x, y);
            x += offset;
            y += offset;
        }
    }
}

void WindowManager::minimize_all() {
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            minimize_window(window);
        }
    }
}

void WindowManager::restore_all() {
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            restore_window(window);
        }
    }
}

std::shared_ptr<Window> WindowManager::create_su1_window(const std::string& app_name, const WindowProperties& properties) {
    auto window = create_window(properties);
    if (window) {
        window->set_su1_app_name(app_name);
        su1_windows_[app_name] = window;
        std::cout << "[S1U] Created SU1 window for application: " << app_name << std::endl;
    }
    return window;
}

void WindowManager::update_su1_windows() {
    for (auto& [name, window] : su1_windows_) {
        if (window) {
            window->update(0.016); // 60 FPS update
        }
    }
}

std::vector<std::string> WindowManager::get_su1_app_names() const {
    std::vector<std::string> names;
    for (const auto& [name, window] : su1_windows_) {
        names.push_back(name);
    }
    return names;
}

void WindowManager::render_windows(std::shared_ptr<Renderer> renderer) {
    if (!renderer) return;
    
    // Render all windows
    for (auto& [id, window] : windows_) {
        if (window && window->is_visible()) {
            window->render(renderer);
        }
    }
}

void WindowManager::update_windows(double delta_time) {
    for (auto& [id, window] : windows_) {
        if (window) {
            window->update(delta_time);
        }
    }
    
    // Update SU1 windows specifically
    update_su1_windows();
}

void WindowManager::handle_window_events() {
    // Handle window events (resize, move, etc.)
    // This would typically be called from the input manager
}

uint32_t WindowManager::generate_window_id() {
    return next_window_id_++;
}

void WindowManager::update_window_focus() {
    // Update window focus based on mouse position or other criteria
    // This is a simplified implementation
}

void WindowManager::cleanup_destroyed_windows() {
    // Remove windows that have been marked for destruction
    for (auto it = windows_.begin(); it != windows_.end();) {
        if (!it->second || !it->second->is_visible()) {
            it = windows_.erase(it);
        } else {
            ++it;
        }
    }
}

void WindowManager::bring_window_to_front(std::shared_ptr<Window> window) {
    // Bring window to front by reordering in the windows map
    // This is a simplified implementation
}

} // namespace s1u

#include <iostream>
#include <algorithm>
#include <climits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

Window::Window(const WindowProperties& properties)
    : properties_(properties)
    , created_(false)
    , focused_(false) {
}

Window::~Window() {
    destroy();
}

bool Window::create() {
    if (created_) {
        return true;
    }
    
    created_ = true;
    std::cout << "[S1U] Created window: " << properties_.title << std::endl;
    return true;
}

void Window::destroy() {
    if (!created_) {
        return;
    }
    
    created_ = false;
    std::cout << "[S1U] Destroyed window: " << properties_.title << std::endl;
}

void Window::show() {
    properties_.visible = true;
}

void Window::hide() {
    properties_.visible = false;
}

void Window::close() {
    properties_.visible = false;
    created_ = false;
}

void Window::set_title(const std::string& title) {
    properties_.title = title;
}

void Window::set_size(uint32_t width, uint32_t height) {
    properties_.width = width;
    properties_.height = height;
}

void Window::set_position(int32_t x, int32_t y) {
    properties_.x = x;
    properties_.y = y;
}

void Window::set_state(WindowState state) {
    properties_.state = state;
}

void Window::set_opacity(float opacity) {
    properties_.opacity = std::clamp(opacity, 0.0f, 1.0f);
}

void Window::render(std::shared_ptr<Renderer> renderer) {
    if (!created_ || !properties_.visible || !renderer) {
        return;
    }
    
    // Render window background
    Rect window_rect(properties_.x, properties_.y, properties_.width, properties_.height);
    Color bg_color(0.2f, 0.2f, 0.25f, properties_.opacity);
    renderer->draw_rect(window_rect, bg_color);
    
    // Render window border
    Color border_color = focused_ ? Color(0.4f, 0.6f, 1.0f, 1.0f) : Color(0.3f, 0.3f, 0.35f, 1.0f);
    renderer->draw_rect_outline(window_rect, border_color, 2.0f);
    
    // Render window title bar
    Rect title_bar_rect(properties_.x, properties_.y, properties_.width, 30);
    Color title_bg_color = focused_ ? Color(0.3f, 0.5f, 0.8f, 1.0f) : Color(0.25f, 0.25f, 0.3f, 1.0f);
    renderer->draw_rect(title_bar_rect, title_bg_color);
    
    // Render window title
    Point title_pos(properties_.x + 10, properties_.y + 8);
    Color title_color(1.0f, 1.0f, 1.0f, 1.0f);
    renderer->draw_text(properties_.title, title_pos, title_color, 14.0f);
    
    // Render child windows
    for (auto& child : child_windows_) {
        if (child && child->is_visible()) {
            child->render(renderer);
        }
    }
}

void Window::update(double delta_time) {
    if (!created_ || !properties_.visible) {
        return;
    }
    
    // Update child windows
    for (auto& child : child_windows_) {
        if (child) {
            child->update(delta_time);
        }
    }
}

void Window::on_focus() {
    focused_ = true;
}

void Window::on_lose_focus() {
    focused_ = false;
}

void Window::on_resize(uint32_t width, uint32_t height) {
    properties_.width = width;
    properties_.height = height;
}

void Window::on_move(int32_t x, int32_t y) {
    properties_.x = x;
    properties_.y = y;
}

void Window::on_close() {
    close();
}

void Window::set_su1_app_name(const std::string& app_name) {
    su1_app_name_ = app_name;
}

// WindowManager implementation
WindowManager::WindowManager()
    : next_window_id_(1) {
}

WindowManager::~WindowManager() {
    shutdown();
}

bool WindowManager::initialize() {
    std::cout << "[S1U] Initializing Window Manager..." << std::endl;
    
    // Create desktop window
    WindowProperties desktop_props;
    desktop_props.title = "Desktop";
    desktop_props.width = 1920;
    desktop_props.height = 1080;
    desktop_props.x = 0;
    desktop_props.y = 0;
    desktop_props.type = WindowType::Desktop;
    desktop_props.state = WindowState::Normal;
    desktop_props.resizable = false;
    desktop_props.movable = false;
    desktop_props.closable = false;
    desktop_props.minimizable = false;
    desktop_props.maximizable = false;
    desktop_props.always_on_top = true;
    desktop_props.opacity = 1.0f;
    desktop_props.decorated = false;
    desktop_props.visible = true;
    
    auto desktop_window = create_window(desktop_props);
    if (desktop_window) {
        desktop_window->create();
    }
    
    std::cout << "[S1U] Window Manager initialized successfully!" << std::endl;
    return true;
}

void WindowManager::shutdown() {
    std::cout << "[S1U] Shutting down Window Manager..." << std::endl;
    
    // Destroy all windows
    for (auto& [id, window] : windows_) {
        if (window) {
            window->destroy();
        }
    }
    
    windows_.clear();
    su1_windows_.clear();
    focused_window_.reset();
}

std::shared_ptr<Window> WindowManager::create_window(const WindowProperties& properties) {
    auto window = std::make_shared<Window>(properties);
    uint32_t id = generate_window_id();
    
    windows_[id] = window;
    
    if (properties.type == WindowType::Desktop) {
        // Desktop window is always visible and focused
        window->on_focus();
    }
    
    std::cout << "[S1U] Created window with ID: " << id << " - " << properties.title << std::endl;
    return window;
}

void WindowManager::destroy_window(std::shared_ptr<Window> window) {
    if (!window) return;
    
    // Remove from windows map
    for (auto it = windows_.begin(); it != windows_.end(); ++it) {
        if (it->second == window) {
            windows_.erase(it);
            break;
        }
    }
    
    // Remove from SU1 windows if it's a SU1 window
    for (auto it = su1_windows_.begin(); it != su1_windows_.end(); ++it) {
        if (it->second == window) {
            su1_windows_.erase(it);
            break;
        }
    }
    
    // Clear focus if this was the focused window
    if (focused_window_ == window) {
        focused_window_.reset();
    }
    
    window->destroy();
}

void WindowManager::destroy_window(uint32_t window_id) {
    auto it = windows_.find(window_id);
    if (it != windows_.end()) {
        destroy_window(it->second);
    }
}

std::shared_ptr<Window> WindowManager::get_window(uint32_t window_id) const {
    auto it = windows_.find(window_id);
    return (it != windows_.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Window>> WindowManager::get_all_windows() const {
    std::vector<std::shared_ptr<Window>> result;
    for (const auto& [id, window] : windows_) {
        if (window) {
            result.push_back(window);
        }
    }
    return result;
}

std::shared_ptr<Window> WindowManager::get_focused_window() const {
    return focused_window_;
}

std::shared_ptr<Window> WindowManager::get_window_at_position(int32_t x, int32_t y) const {
    // Find the topmost window at the given position
    // Convert to vector and reverse iterate to check from top to bottom
    std::vector<std::pair<uint32_t, std::shared_ptr<Window>>> window_list(windows_.begin(), windows_.end());
    for (auto it = window_list.rbegin(); it != window_list.rend(); ++it) {
        auto& window = it->second;
        if (window && window->is_visible()) {
            const auto& props = window->get_properties();
            if (x >= props.x && x < props.x + static_cast<int32_t>(props.width) &&
                y >= props.y && y < props.y + static_cast<int32_t>(props.height)) {
                return window;
            }
        }
    }
    return nullptr;
}

void WindowManager::focus_window(std::shared_ptr<Window> window) {
    if (!window) return;
    
    // Remove focus from current focused window
    if (focused_window_) {
        focused_window_->on_lose_focus();
    }
    
    // Set new focused window
    focused_window_ = window;
    window->on_focus();
    
    // Bring window to front
    bring_window_to_front(window);
}

void WindowManager::minimize_window(std::shared_ptr<Window> window) {
    if (window) {
        window->set_state(WindowState::Minimized);
    }
}

void WindowManager::maximize_window(std::shared_ptr<Window> window) {
    if (window) {
        window->set_state(WindowState::Maximized);
    }
}

void WindowManager::restore_window(std::shared_ptr<Window> window) {
    if (window) {
        window->set_state(WindowState::Normal);
    }
}

void WindowManager::close_window(std::shared_ptr<Window> window) {
    if (window) {
        window->close();
        destroy_window(window);
    }
}

void WindowManager::arrange_windows() {
    // Simple tiling arrangement
    int x = 0, y = 0;
    int max_width = 1920 / 2; // Split screen in half
    
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            window->set_position(x, y);
            window->set_size(max_width, 600);
            
            x += max_width;
            if (x >= 1920) {
                x = 0;
                y += 600;
            }
        }
    }
}

void WindowManager::tile_windows() {
    arrange_windows();
}

void WindowManager::cascade_windows() {
    int x = 50, y = 50;
    int offset = 30;
    
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            window->set_position(x, y);
            x += offset;
            y += offset;
        }
    }
}

void WindowManager::minimize_all() {
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            minimize_window(window);
        }
    }
}

void WindowManager::restore_all() {
    for (auto& [id, window] : windows_) {
        if (window && window->get_properties().type != WindowType::Desktop) {
            restore_window(window);
        }
    }
}

std::shared_ptr<Window> WindowManager::create_su1_window(const std::string& app_name, const WindowProperties& properties) {
    auto window = create_window(properties);
    if (window) {
        window->set_su1_app_name(app_name);
        su1_windows_[app_name] = window;
        std::cout << "[S1U] Created SU1 window for application: " << app_name << std::endl;
    }
    return window;
}

void WindowManager::update_su1_windows() {
    for (auto& [name, window] : su1_windows_) {
        if (window) {
            window->update(0.016); // 60 FPS update
        }
    }
}

std::vector<std::string> WindowManager::get_su1_app_names() const {
    std::vector<std::string> names;
    for (const auto& [name, window] : su1_windows_) {
        names.push_back(name);
    }
    return names;
}

void WindowManager::render_windows(std::shared_ptr<Renderer> renderer) {
    if (!renderer) return;
    
    // Render all windows
    for (auto& [id, window] : windows_) {
        if (window && window->is_visible()) {
            window->render(renderer);
        }
    }
}

void WindowManager::update_windows(double delta_time) {
    for (auto& [id, window] : windows_) {
        if (window) {
            window->update(delta_time);
        }
    }
    
    // Update SU1 windows specifically
    update_su1_windows();
}

void WindowManager::handle_window_events() {
    // Handle window events (resize, move, etc.)
    // This would typically be called from the input manager
}

uint32_t WindowManager::generate_window_id() {
    return next_window_id_++;
}

void WindowManager::update_window_focus() {
    // Update window focus based on mouse position or other criteria
    // This is a simplified implementation
}

void WindowManager::cleanup_destroyed_windows() {
    // Remove windows that have been marked for destruction
    for (auto it = windows_.begin(); it != windows_.end();) {
        if (!it->second || !it->second->is_visible()) {
            it = windows_.erase(it);
        } else {
            ++it;
        }
    }
}

void WindowManager::bring_window_to_front(std::shared_ptr<Window> window) {
    // Bring window to front by reordering in the windows map
    // This is a simplified implementation
}

} // namespace s1u
