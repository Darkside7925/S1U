#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include "s1u/window.hpp"
#include "s1u/renderer.hpp"

namespace s1u {

// SU1 Application Integration System
class SU1Integration {
public:
    struct SU1App {
        std::string name;
        std::string path;
        std::string executable;
        std::string config_file;
        bool loaded = false;
        bool running = false;
        std::chrono::high_resolution_clock::time_point last_update;
        std::shared_ptr<Window> window;
        uint32_t pid = 0;
        std::string status;
        float cpu_usage = 0.0f;
        float memory_usage = 0.0f;
        bool glass_theme_enabled = true;
        float glass_opacity = 0.4f;
        float glass_blur = 15.0f;
    };

    struct SU1Config {
        std::string su1_install_path = "/usr/local";
        std::string su1_config_dir = "/etc/su1";
        std::string su1_plugin_dir = "/usr/local/lib/su1";
        bool enable_glass_theming = true;
        bool enable_vsync = true;
        bool enable_hardware_acceleration = true;
        uint32_t max_su1_apps = 10;
        float default_glass_opacity = 0.4f;
        float default_glass_blur = 15.0f;
        std::string default_theme = "liquid_glass";
    };

    SU1Integration();
    ~SU1Integration();

    // Initialization
    bool initialize(const SU1Config& config);
    void shutdown();

    // SU1 Application Management
    bool load_su1_application(const std::string& app_path, const std::string& app_name = "");
    bool unload_su1_application(const std::string& app_name);
    bool start_su1_application(const std::string& app_name);
    bool stop_su1_application(const std::string& app_name);
    bool restart_su1_application(const std::string& app_name);

    // Application Discovery
    std::vector<std::string> discover_su1_applications();
    std::vector<SU1App> get_loaded_applications() const;
    std::shared_ptr<SU1App> get_application(const std::string& app_name) const;

    // Glass Theming Integration
    void enable_glass_theming(bool enable);
    void set_glass_theme(const std::string& app_name, float opacity, float blur, float border = 2.0f);
    void apply_glass_effects(const std::string& app_name);

    // Rendering Integration
    void render_su1_applications(Renderer* renderer);
    void update_su1_applications();

    // Configuration
    SU1Config get_config() const { return config_; }
    void update_config(const SU1Config& config);

    // Status and Monitoring
    bool is_su1_available() const { return su1_available_; }
    std::string get_su1_version() const { return su1_version_; }
    uint32_t get_running_app_count() const;
    float get_total_cpu_usage() const;
    float get_total_memory_usage() const;

    // Event Handling
    using SU1EventCallback = std::function<void(const std::string& app_name, const std::string& event)>;
    void register_event_callback(SU1EventCallback callback);
    void unregister_event_callback();

private:
    // Internal methods
    bool check_su1_availability();
    bool validate_su1_application(const std::string& app_path);
    bool create_su1_window(const std::string& app_name);
    void update_application_stats(SU1App& app);
    void update_global_stats();
    void handle_su1_events();

    // Configuration
    SU1Config config_;
    bool su1_available_;
    std::string su1_version_;

    // Application management
    std::vector<std::shared_ptr<SU1App>> applications_;
    std::unordered_map<std::string, std::shared_ptr<SU1App>> app_map_;

    // Event handling
    SU1EventCallback event_callback_;
    std::vector<std::string> event_queue_;

    // Glass theming
    bool glass_theming_enabled_;
    std::unordered_map<std::string, std::tuple<float, float, float>> glass_themes_;

    // Performance monitoring
    std::chrono::high_resolution_clock::time_point last_stats_update_;
    float total_cpu_usage_;
    float total_memory_usage_;
};

} // namespace s1u

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include "s1u/window.hpp"
#include "s1u/renderer.hpp"

namespace s1u {

// SU1 Application Integration System
class SU1Integration {
public:
    struct SU1App {
        std::string name;
        std::string path;
        std::string executable;
        std::string config_file;
        bool loaded = false;
        bool running = false;
        std::chrono::high_resolution_clock::time_point last_update;
        std::shared_ptr<Window> window;
        uint32_t pid = 0;
        std::string status;
        float cpu_usage = 0.0f;
        float memory_usage = 0.0f;
        bool glass_theme_enabled = true;
        float glass_opacity = 0.4f;
        float glass_blur = 15.0f;
    };

    struct SU1Config {
        std::string su1_install_path = "/usr/local";
        std::string su1_config_dir = "/etc/su1";
        std::string su1_plugin_dir = "/usr/local/lib/su1";
        bool enable_glass_theming = true;
        bool enable_vsync = true;
        bool enable_hardware_acceleration = true;
        uint32_t max_su1_apps = 10;
        float default_glass_opacity = 0.4f;
        float default_glass_blur = 15.0f;
        std::string default_theme = "liquid_glass";
    };

    SU1Integration();
    ~SU1Integration();

    // Initialization
    bool initialize(const SU1Config& config);
    void shutdown();

    // SU1 Application Management
    bool load_su1_application(const std::string& app_path, const std::string& app_name = "");
    bool unload_su1_application(const std::string& app_name);
    bool start_su1_application(const std::string& app_name);
    bool stop_su1_application(const std::string& app_name);
    bool restart_su1_application(const std::string& app_name);

    // Application Discovery
    std::vector<std::string> discover_su1_applications();
    std::vector<SU1App> get_loaded_applications() const;
    std::shared_ptr<SU1App> get_application(const std::string& app_name) const;

    // Glass Theming Integration
    void enable_glass_theming(bool enable);
    void set_glass_theme(const std::string& app_name, float opacity, float blur, float border = 2.0f);
    void apply_glass_effects(const std::string& app_name);

    // Rendering Integration
    void render_su1_applications(Renderer* renderer);
    void update_su1_applications();

    // Configuration
    SU1Config get_config() const { return config_; }
    void update_config(const SU1Config& config);

    // Status and Monitoring
    bool is_su1_available() const { return su1_available_; }
    std::string get_su1_version() const { return su1_version_; }
    uint32_t get_running_app_count() const;
    float get_total_cpu_usage() const;
    float get_total_memory_usage() const;

    // Event Handling
    using SU1EventCallback = std::function<void(const std::string& app_name, const std::string& event)>;
    void register_event_callback(SU1EventCallback callback);
    void unregister_event_callback();

private:
    // Internal methods
    bool check_su1_availability();
    bool validate_su1_application(const std::string& app_path);
    bool create_su1_window(const std::string& app_name);
    void update_application_stats(SU1App& app);
    void update_global_stats();
    void handle_su1_events();

    // Configuration
    SU1Config config_;
    bool su1_available_;
    std::string su1_version_;

    // Application management
    std::vector<std::shared_ptr<SU1App>> applications_;
    std::unordered_map<std::string, std::shared_ptr<SU1App>> app_map_;

    // Event handling
    SU1EventCallback event_callback_;
    std::vector<std::string> event_queue_;

    // Glass theming
    bool glass_theming_enabled_;
    std::unordered_map<std::string, std::tuple<float, float, float>> glass_themes_;

    // Performance monitoring
    std::chrono::high_resolution_clock::time_point last_stats_update_;
    float total_cpu_usage_;
    float total_memory_usage_;
};

} // namespace s1u

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include "s1u/window.hpp"
#include "s1u/renderer.hpp"

namespace s1u {

// SU1 Application Integration System
class SU1Integration {
public:
    struct SU1App {
        std::string name;
        std::string path;
        std::string executable;
        std::string config_file;
        bool loaded = false;
        bool running = false;
        std::chrono::high_resolution_clock::time_point last_update;
        std::shared_ptr<Window> window;
        uint32_t pid = 0;
        std::string status;
        float cpu_usage = 0.0f;
        float memory_usage = 0.0f;
        bool glass_theme_enabled = true;
        float glass_opacity = 0.4f;
        float glass_blur = 15.0f;
    };

    struct SU1Config {
        std::string su1_install_path = "/usr/local";
        std::string su1_config_dir = "/etc/su1";
        std::string su1_plugin_dir = "/usr/local/lib/su1";
        bool enable_glass_theming = true;
        bool enable_vsync = true;
        bool enable_hardware_acceleration = true;
        uint32_t max_su1_apps = 10;
        float default_glass_opacity = 0.4f;
        float default_glass_blur = 15.0f;
        std::string default_theme = "liquid_glass";
    };

    SU1Integration();
    ~SU1Integration();

    // Initialization
    bool initialize(const SU1Config& config);
    void shutdown();

    // SU1 Application Management
    bool load_su1_application(const std::string& app_path, const std::string& app_name = "");
    bool unload_su1_application(const std::string& app_name);
    bool start_su1_application(const std::string& app_name);
    bool stop_su1_application(const std::string& app_name);
    bool restart_su1_application(const std::string& app_name);

    // Application Discovery
    std::vector<std::string> discover_su1_applications();
    std::vector<SU1App> get_loaded_applications() const;
    std::shared_ptr<SU1App> get_application(const std::string& app_name) const;

    // Glass Theming Integration
    void enable_glass_theming(bool enable);
    void set_glass_theme(const std::string& app_name, float opacity, float blur, float border = 2.0f);
    void apply_glass_effects(const std::string& app_name);

    // Rendering Integration
    void render_su1_applications(Renderer* renderer);
    void update_su1_applications();

    // Configuration
    SU1Config get_config() const { return config_; }
    void update_config(const SU1Config& config);

    // Status and Monitoring
    bool is_su1_available() const { return su1_available_; }
    std::string get_su1_version() const { return su1_version_; }
    uint32_t get_running_app_count() const;
    float get_total_cpu_usage() const;
    float get_total_memory_usage() const;

    // Event Handling
    using SU1EventCallback = std::function<void(const std::string& app_name, const std::string& event)>;
    void register_event_callback(SU1EventCallback callback);
    void unregister_event_callback();

private:
    // Internal methods
    bool check_su1_availability();
    bool validate_su1_application(const std::string& app_path);
    bool create_su1_window(const std::string& app_name);
    void update_application_stats(SU1App& app);
    void update_global_stats();
    void handle_su1_events();

    // Configuration
    SU1Config config_;
    bool su1_available_;
    std::string su1_version_;

    // Application management
    std::vector<std::shared_ptr<SU1App>> applications_;
    std::unordered_map<std::string, std::shared_ptr<SU1App>> app_map_;

    // Event handling
    SU1EventCallback event_callback_;
    std::vector<std::string> event_queue_;

    // Glass theming
    bool glass_theming_enabled_;
    std::unordered_map<std::string, std::tuple<float, float, float>> glass_themes_;

    // Performance monitoring
    std::chrono::high_resolution_clock::time_point last_stats_update_;
    float total_cpu_usage_;
    float total_memory_usage_;
};

} // namespace s1u
