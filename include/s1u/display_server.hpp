#pragma once

#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <functional>

namespace s1u {

// Forward declarations
class WindowManager;
class Renderer;
class InputManager;
class Compositor;
class SU1Integration;
class DisplayServer;

// Display server configuration
struct DisplayServerConfig {
    uint32_t width = 1920;
    uint32_t height = 1080;
    uint32_t refresh_rate = 60;
    bool vsync = true;
    bool fullscreen = false;
    bool borderless = false;
    std::string title = "S1U Display Server";
    std::string theme = "liquid_glass";
    bool enable_compositor = true;
    bool enable_quantum_effects = true;
    uint32_t max_fps = 144;
};

// Display server state
enum class DisplayServerState {
    Uninitialized,
    Initializing,
    Running,
    Paused,
    Stopping,
    Stopped,
    Error
};

// Main display server class
class DisplayServer {
public:
    DisplayServer();
    ~DisplayServer();

    // Initialization and shutdown
    bool initialize(const DisplayServerConfig& config = DisplayServerConfig{});
    void shutdown();

    // Main loop control
    void run();
    void stop();
    void pause();
    void resume();

    // Configuration
    void set_config(const DisplayServerConfig& config);
    const DisplayServerConfig& get_config() const;

    // State queries
    DisplayServerState get_state() const { return state_; }
    bool is_running() const { return state_ == DisplayServerState::Running; }
    bool is_paused() const { return state_ == DisplayServerState::Paused; }

    // Component access
    std::shared_ptr<WindowManager> get_window_manager() const { return window_manager_; }
    std::shared_ptr<Renderer> get_renderer() const { return renderer_; }
    std::shared_ptr<InputManager> get_input_manager() const { return input_manager_; }
    std::shared_ptr<Compositor> get_compositor() const { return compositor_; }

    // SU1 integration
    bool load_su1_application(const std::string& app_path);
    void unload_su1_application(const std::string& app_name);
    std::vector<std::string> get_loaded_su1_apps() const;

    // Performance monitoring
    double get_current_fps() const;
    uint64_t get_frame_count() const;
    double get_average_frame_time() const;

private:
    // Main loop
    void main_loop();
    void process_frame();
    void handle_events();
    void render_frame();
    void present_frame();

    // Frame timing and vsync
    void update_frame_timing();
    void wait_for_vsync();

    // SU1 application management
    void update_su1_applications();
    void render_su1_applications();

    // Configuration
    DisplayServerConfig config_;
    DisplayServerState state_;

    // Core components
    std::shared_ptr<WindowManager> window_manager_;
    std::shared_ptr<Renderer> renderer_;
    std::shared_ptr<InputManager> input_manager_;
    std::shared_ptr<Compositor> compositor_;

    // Main loop control
    std::atomic<bool> running_;
    std::thread main_thread_;

    // Frame timing
    std::chrono::high_resolution_clock::time_point last_frame_time_;
    std::chrono::high_resolution_clock::time_point frame_start_time_;
    uint64_t frame_count_;
    double current_fps_;
    double average_frame_time_;
    std::vector<double> frame_times_;

    // SU1 integration system
    std::shared_ptr<SU1Integration> su1_integration_;
};

} // namespace s1u

#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <functional>

namespace s1u {

// Forward declarations
class WindowManager;
class Renderer;
class InputManager;
class Compositor;
class SU1Integration;
class DisplayServer;

// Display server configuration
struct DisplayServerConfig {
    uint32_t width = 1920;
    uint32_t height = 1080;
    uint32_t refresh_rate = 60;
    bool vsync = true;
    bool fullscreen = false;
    bool borderless = false;
    std::string title = "S1U Display Server";
    std::string theme = "liquid_glass";
    bool enable_compositor = true;
    bool enable_quantum_effects = true;
    uint32_t max_fps = 144;
};

// Display server state
enum class DisplayServerState {
    Uninitialized,
    Initializing,
    Running,
    Paused,
    Stopping,
    Stopped,
    Error
};

// Main display server class
class DisplayServer {
public:
    DisplayServer();
    ~DisplayServer();

    // Initialization and shutdown
    bool initialize(const DisplayServerConfig& config = DisplayServerConfig{});
    void shutdown();

    // Main loop control
    void run();
    void stop();
    void pause();
    void resume();

    // Configuration
    void set_config(const DisplayServerConfig& config);
    const DisplayServerConfig& get_config() const;

    // State queries
    DisplayServerState get_state() const { return state_; }
    bool is_running() const { return state_ == DisplayServerState::Running; }
    bool is_paused() const { return state_ == DisplayServerState::Paused; }

    // Component access
    std::shared_ptr<WindowManager> get_window_manager() const { return window_manager_; }
    std::shared_ptr<Renderer> get_renderer() const { return renderer_; }
    std::shared_ptr<InputManager> get_input_manager() const { return input_manager_; }
    std::shared_ptr<Compositor> get_compositor() const { return compositor_; }

    // SU1 integration
    bool load_su1_application(const std::string& app_path);
    void unload_su1_application(const std::string& app_name);
    std::vector<std::string> get_loaded_su1_apps() const;

    // Performance monitoring
    double get_current_fps() const;
    uint64_t get_frame_count() const;
    double get_average_frame_time() const;

private:
    // Main loop
    void main_loop();
    void process_frame();
    void handle_events();
    void render_frame();
    void present_frame();

    // Frame timing and vsync
    void update_frame_timing();
    void wait_for_vsync();

    // SU1 application management
    void update_su1_applications();
    void render_su1_applications();

    // Configuration
    DisplayServerConfig config_;
    DisplayServerState state_;

    // Core components
    std::shared_ptr<WindowManager> window_manager_;
    std::shared_ptr<Renderer> renderer_;
    std::shared_ptr<InputManager> input_manager_;
    std::shared_ptr<Compositor> compositor_;

    // Main loop control
    std::atomic<bool> running_;
    std::thread main_thread_;

    // Frame timing
    std::chrono::high_resolution_clock::time_point last_frame_time_;
    std::chrono::high_resolution_clock::time_point frame_start_time_;
    uint64_t frame_count_;
    double current_fps_;
    double average_frame_time_;
    std::vector<double> frame_times_;

    // SU1 integration system
    std::shared_ptr<SU1Integration> su1_integration_;
};

} // namespace s1u

#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <functional>

namespace s1u {

// Forward declarations
class WindowManager;
class Renderer;
class InputManager;
class Compositor;
class SU1Integration;
class DisplayServer;

// Display server configuration
struct DisplayServerConfig {
    uint32_t width = 1920;
    uint32_t height = 1080;
    uint32_t refresh_rate = 60;
    bool vsync = true;
    bool fullscreen = false;
    bool borderless = false;
    std::string title = "S1U Display Server";
    std::string theme = "liquid_glass";
    bool enable_compositor = true;
    bool enable_quantum_effects = true;
    uint32_t max_fps = 144;
};

// Display server state
enum class DisplayServerState {
    Uninitialized,
    Initializing,
    Running,
    Paused,
    Stopping,
    Stopped,
    Error
};

// Main display server class
class DisplayServer {
public:
    DisplayServer();
    ~DisplayServer();

    // Initialization and shutdown
    bool initialize(const DisplayServerConfig& config = DisplayServerConfig{});
    void shutdown();

    // Main loop control
    void run();
    void stop();
    void pause();
    void resume();

    // Configuration
    void set_config(const DisplayServerConfig& config);
    const DisplayServerConfig& get_config() const;

    // State queries
    DisplayServerState get_state() const { return state_; }
    bool is_running() const { return state_ == DisplayServerState::Running; }
    bool is_paused() const { return state_ == DisplayServerState::Paused; }

    // Component access
    std::shared_ptr<WindowManager> get_window_manager() const { return window_manager_; }
    std::shared_ptr<Renderer> get_renderer() const { return renderer_; }
    std::shared_ptr<InputManager> get_input_manager() const { return input_manager_; }
    std::shared_ptr<Compositor> get_compositor() const { return compositor_; }

    // SU1 integration
    bool load_su1_application(const std::string& app_path);
    void unload_su1_application(const std::string& app_name);
    std::vector<std::string> get_loaded_su1_apps() const;

    // Performance monitoring
    double get_current_fps() const;
    uint64_t get_frame_count() const;
    double get_average_frame_time() const;

private:
    // Main loop
    void main_loop();
    void process_frame();
    void handle_events();
    void render_frame();
    void present_frame();

    // Frame timing and vsync
    void update_frame_timing();
    void wait_for_vsync();

    // SU1 application management
    void update_su1_applications();
    void render_su1_applications();

    // Configuration
    DisplayServerConfig config_;
    DisplayServerState state_;

    // Core components
    std::shared_ptr<WindowManager> window_manager_;
    std::shared_ptr<Renderer> renderer_;
    std::shared_ptr<InputManager> input_manager_;
    std::shared_ptr<Compositor> compositor_;

    // Main loop control
    std::atomic<bool> running_;
    std::thread main_thread_;

    // Frame timing
    std::chrono::high_resolution_clock::time_point last_frame_time_;
    std::chrono::high_resolution_clock::time_point frame_start_time_;
    uint64_t frame_count_;
    double current_fps_;
    double average_frame_time_;
    std::vector<double> frame_times_;

    // SU1 integration system
    std::shared_ptr<SU1Integration> su1_integration_;
};

} // namespace s1u
