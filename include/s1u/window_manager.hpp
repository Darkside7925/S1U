#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace s1u {

// Forward declarations
class Window;
class Renderer;

// Window types
enum class WindowType {
    Normal,
    Dialog,
    Tooltip,
    Popup,
    Menu,
    Splash,
    Utility,
    Desktop,
    Dock,
    Toolbar,
    Status,
    Dropdown,
    Notification,
    Combo,
    Modal,
    Transient,
    Override
};

// Window states
enum class WindowState {
    Normal,
    Minimized,
    Maximized,
    Fullscreen,
    Hidden
};

// Window properties
struct WindowProperties {
    std::string title;
    uint32_t width;
    uint32_t height;
    int32_t x;
    int32_t y;
    WindowType type;
    WindowState state;
    bool resizable;
    bool movable;
    bool closable;
    bool minimizable;
    bool maximizable;
    bool always_on_top;
    float opacity;
    bool decorated;
    bool visible;
    
    WindowProperties()
        : title("Window")
        , width(800)
        , height(600)
        , x(100)
        , y(100)
        , type(WindowType::Normal)
        , state(WindowState::Normal)
        , resizable(true)
        , movable(true)
        , closable(true)
        , minimizable(true)
        , maximizable(true)
        , always_on_top(false)
        , opacity(1.0f)
        , decorated(true)
        , visible(true) {}
};

// Window class
class Window {
public:
    Window(const WindowProperties& properties);
    ~Window();

    // Window management
    bool create();
    void destroy();
    void show();
    void hide();
    void close();

    // Properties
    void set_title(const std::string& title);
    void set_size(uint32_t width, uint32_t height);
    void set_position(int32_t x, int32_t y);
    void set_state(WindowState state);
    void set_opacity(float opacity);

    // Getters
    const std::string& get_title() const { return properties_.title; }
    uint32_t get_width() const { return properties_.width; }
    uint32_t get_height() const { return properties_.height; }
    int32_t get_x() const { return properties_.x; }
    int32_t get_y() const { return properties_.y; }
    WindowState get_state() const { return properties_.state; }
    float get_opacity() const { return properties_.opacity; }
    bool is_visible() const { return properties_.visible; }
    bool is_focused() const { return focused_; }

    // Rendering
    void render(std::shared_ptr<Renderer> renderer);
    void update(double delta_time);

    // Event handling
    void on_focus();
    void on_lose_focus();
    void on_resize(uint32_t width, uint32_t height);
    void on_move(int32_t x, int32_t y);
    void on_close();

    // SU1 integration
    void set_su1_app_name(const std::string& app_name);
    const std::string& get_su1_app_name() const { return su1_app_name_; }
    
    // Properties access
    const WindowProperties& get_properties() const { return properties_; }

private:
    WindowProperties properties_;
    bool created_;
    bool focused_;
    std::string su1_app_name_;
    
    // Window content
    std::vector<std::shared_ptr<Window>> child_windows_;
};

// Window manager class
class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    // Initialization
    bool initialize();
    void shutdown();

    // Window management
    std::shared_ptr<Window> create_window(const WindowProperties& properties);
    void destroy_window(std::shared_ptr<Window> window);
    void destroy_window(uint32_t window_id);

    // Window queries
    std::shared_ptr<Window> get_window(uint32_t window_id) const;
    std::vector<std::shared_ptr<Window>> get_all_windows() const;
    std::shared_ptr<Window> get_focused_window() const;
    std::shared_ptr<Window> get_window_at_position(int32_t x, int32_t y) const;

    // Window operations
    void focus_window(std::shared_ptr<Window> window);
    void minimize_window(std::shared_ptr<Window> window);
    void maximize_window(std::shared_ptr<Window> window);
    void restore_window(std::shared_ptr<Window> window);
    void close_window(std::shared_ptr<Window> window);

    // Layout management
    void arrange_windows();
    void tile_windows();
    void cascade_windows();
    void minimize_all();
    void restore_all();

    // SU1 application integration
    std::shared_ptr<Window> create_su1_window(const std::string& app_name, const WindowProperties& properties);
    void update_su1_windows();
    std::vector<std::string> get_su1_app_names() const;

    // Rendering
    void render_windows(std::shared_ptr<Renderer> renderer);
    void update_windows(double delta_time);

    // Event handling
    void handle_window_events();

private:
    // Window storage
    std::unordered_map<uint32_t, std::shared_ptr<Window>> windows_;
    std::shared_ptr<Window> focused_window_;
    uint32_t next_window_id_;

    // SU1 integration
    std::unordered_map<std::string, std::shared_ptr<Window>> su1_windows_;

    // Helper methods
    uint32_t generate_window_id();
    void update_window_focus();
    void cleanup_destroyed_windows();
    void bring_window_to_front(std::shared_ptr<Window> window);
};

} // namespace s1u

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace s1u {

// Forward declarations
class Window;
class Renderer;

// Window types
enum class WindowType {
    Normal,
    Dialog,
    Tooltip,
    Popup,
    Menu,
    Splash,
    Utility,
    Desktop,
    Dock,
    Toolbar,
    Status,
    Dropdown,
    Notification,
    Combo,
    Modal,
    Transient,
    Override
};

// Window states
enum class WindowState {
    Normal,
    Minimized,
    Maximized,
    Fullscreen,
    Hidden
};

// Window properties
struct WindowProperties {
    std::string title;
    uint32_t width;
    uint32_t height;
    int32_t x;
    int32_t y;
    WindowType type;
    WindowState state;
    bool resizable;
    bool movable;
    bool closable;
    bool minimizable;
    bool maximizable;
    bool always_on_top;
    float opacity;
    bool decorated;
    bool visible;
    
    WindowProperties()
        : title("Window")
        , width(800)
        , height(600)
        , x(100)
        , y(100)
        , type(WindowType::Normal)
        , state(WindowState::Normal)
        , resizable(true)
        , movable(true)
        , closable(true)
        , minimizable(true)
        , maximizable(true)
        , always_on_top(false)
        , opacity(1.0f)
        , decorated(true)
        , visible(true) {}
};

// Window class
class Window {
public:
    Window(const WindowProperties& properties);
    ~Window();

    // Window management
    bool create();
    void destroy();
    void show();
    void hide();
    void close();

    // Properties
    void set_title(const std::string& title);
    void set_size(uint32_t width, uint32_t height);
    void set_position(int32_t x, int32_t y);
    void set_state(WindowState state);
    void set_opacity(float opacity);

    // Getters
    const std::string& get_title() const { return properties_.title; }
    uint32_t get_width() const { return properties_.width; }
    uint32_t get_height() const { return properties_.height; }
    int32_t get_x() const { return properties_.x; }
    int32_t get_y() const { return properties_.y; }
    WindowState get_state() const { return properties_.state; }
    float get_opacity() const { return properties_.opacity; }
    bool is_visible() const { return properties_.visible; }
    bool is_focused() const { return focused_; }

    // Rendering
    void render(std::shared_ptr<Renderer> renderer);
    void update(double delta_time);

    // Event handling
    void on_focus();
    void on_lose_focus();
    void on_resize(uint32_t width, uint32_t height);
    void on_move(int32_t x, int32_t y);
    void on_close();

    // SU1 integration
    void set_su1_app_name(const std::string& app_name);
    const std::string& get_su1_app_name() const { return su1_app_name_; }
    
    // Properties access
    const WindowProperties& get_properties() const { return properties_; }

private:
    WindowProperties properties_;
    bool created_;
    bool focused_;
    std::string su1_app_name_;
    
    // Window content
    std::vector<std::shared_ptr<Window>> child_windows_;
};

// Window manager class
class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    // Initialization
    bool initialize();
    void shutdown();

    // Window management
    std::shared_ptr<Window> create_window(const WindowProperties& properties);
    void destroy_window(std::shared_ptr<Window> window);
    void destroy_window(uint32_t window_id);

    // Window queries
    std::shared_ptr<Window> get_window(uint32_t window_id) const;
    std::vector<std::shared_ptr<Window>> get_all_windows() const;
    std::shared_ptr<Window> get_focused_window() const;
    std::shared_ptr<Window> get_window_at_position(int32_t x, int32_t y) const;

    // Window operations
    void focus_window(std::shared_ptr<Window> window);
    void minimize_window(std::shared_ptr<Window> window);
    void maximize_window(std::shared_ptr<Window> window);
    void restore_window(std::shared_ptr<Window> window);
    void close_window(std::shared_ptr<Window> window);

    // Layout management
    void arrange_windows();
    void tile_windows();
    void cascade_windows();
    void minimize_all();
    void restore_all();

    // SU1 application integration
    std::shared_ptr<Window> create_su1_window(const std::string& app_name, const WindowProperties& properties);
    void update_su1_windows();
    std::vector<std::string> get_su1_app_names() const;

    // Rendering
    void render_windows(std::shared_ptr<Renderer> renderer);
    void update_windows(double delta_time);

    // Event handling
    void handle_window_events();

private:
    // Window storage
    std::unordered_map<uint32_t, std::shared_ptr<Window>> windows_;
    std::shared_ptr<Window> focused_window_;
    uint32_t next_window_id_;

    // SU1 integration
    std::unordered_map<std::string, std::shared_ptr<Window>> su1_windows_;

    // Helper methods
    uint32_t generate_window_id();
    void update_window_focus();
    void cleanup_destroyed_windows();
    void bring_window_to_front(std::shared_ptr<Window> window);
};

} // namespace s1u

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace s1u {

// Forward declarations
class Window;
class Renderer;

// Window types
enum class WindowType {
    Normal,
    Dialog,
    Tooltip,
    Popup,
    Menu,
    Splash,
    Utility,
    Desktop,
    Dock,
    Toolbar,
    Status,
    Dropdown,
    Notification,
    Combo,
    Modal,
    Transient,
    Override
};

// Window states
enum class WindowState {
    Normal,
    Minimized,
    Maximized,
    Fullscreen,
    Hidden
};

// Window properties
struct WindowProperties {
    std::string title;
    uint32_t width;
    uint32_t height;
    int32_t x;
    int32_t y;
    WindowType type;
    WindowState state;
    bool resizable;
    bool movable;
    bool closable;
    bool minimizable;
    bool maximizable;
    bool always_on_top;
    float opacity;
    bool decorated;
    bool visible;
    
    WindowProperties()
        : title("Window")
        , width(800)
        , height(600)
        , x(100)
        , y(100)
        , type(WindowType::Normal)
        , state(WindowState::Normal)
        , resizable(true)
        , movable(true)
        , closable(true)
        , minimizable(true)
        , maximizable(true)
        , always_on_top(false)
        , opacity(1.0f)
        , decorated(true)
        , visible(true) {}
};

// Window class
class Window {
public:
    Window(const WindowProperties& properties);
    ~Window();

    // Window management
    bool create();
    void destroy();
    void show();
    void hide();
    void close();

    // Properties
    void set_title(const std::string& title);
    void set_size(uint32_t width, uint32_t height);
    void set_position(int32_t x, int32_t y);
    void set_state(WindowState state);
    void set_opacity(float opacity);

    // Getters
    const std::string& get_title() const { return properties_.title; }
    uint32_t get_width() const { return properties_.width; }
    uint32_t get_height() const { return properties_.height; }
    int32_t get_x() const { return properties_.x; }
    int32_t get_y() const { return properties_.y; }
    WindowState get_state() const { return properties_.state; }
    float get_opacity() const { return properties_.opacity; }
    bool is_visible() const { return properties_.visible; }
    bool is_focused() const { return focused_; }

    // Rendering
    void render(std::shared_ptr<Renderer> renderer);
    void update(double delta_time);

    // Event handling
    void on_focus();
    void on_lose_focus();
    void on_resize(uint32_t width, uint32_t height);
    void on_move(int32_t x, int32_t y);
    void on_close();

    // SU1 integration
    void set_su1_app_name(const std::string& app_name);
    const std::string& get_su1_app_name() const { return su1_app_name_; }
    
    // Properties access
    const WindowProperties& get_properties() const { return properties_; }

private:
    WindowProperties properties_;
    bool created_;
    bool focused_;
    std::string su1_app_name_;
    
    // Window content
    std::vector<std::shared_ptr<Window>> child_windows_;
};

// Window manager class
class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    // Initialization
    bool initialize();
    void shutdown();

    // Window management
    std::shared_ptr<Window> create_window(const WindowProperties& properties);
    void destroy_window(std::shared_ptr<Window> window);
    void destroy_window(uint32_t window_id);

    // Window queries
    std::shared_ptr<Window> get_window(uint32_t window_id) const;
    std::vector<std::shared_ptr<Window>> get_all_windows() const;
    std::shared_ptr<Window> get_focused_window() const;
    std::shared_ptr<Window> get_window_at_position(int32_t x, int32_t y) const;

    // Window operations
    void focus_window(std::shared_ptr<Window> window);
    void minimize_window(std::shared_ptr<Window> window);
    void maximize_window(std::shared_ptr<Window> window);
    void restore_window(std::shared_ptr<Window> window);
    void close_window(std::shared_ptr<Window> window);

    // Layout management
    void arrange_windows();
    void tile_windows();
    void cascade_windows();
    void minimize_all();
    void restore_all();

    // SU1 application integration
    std::shared_ptr<Window> create_su1_window(const std::string& app_name, const WindowProperties& properties);
    void update_su1_windows();
    std::vector<std::string> get_su1_app_names() const;

    // Rendering
    void render_windows(std::shared_ptr<Renderer> renderer);
    void update_windows(double delta_time);

    // Event handling
    void handle_window_events();

private:
    // Window storage
    std::unordered_map<uint32_t, std::shared_ptr<Window>> windows_;
    std::shared_ptr<Window> focused_window_;
    uint32_t next_window_id_;

    // SU1 integration
    std::unordered_map<std::string, std::shared_ptr<Window>> su1_windows_;

    // Helper methods
    uint32_t generate_window_id();
    void update_window_focus();
    void cleanup_destroyed_windows();
    void bring_window_to_front(std::shared_ptr<Window> window);
};

} // namespace s1u
