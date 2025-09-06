#include "s1u/su1_integration.hpp"
#include "s1u/window_manager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <thread>

namespace s1u {

SU1Integration::SU1Integration()
    : su1_available_(false)
    , su1_version_("unknown")
    , glass_theming_enabled_(true)
    , total_cpu_usage_(0.0f)
    , total_memory_usage_(0.0f) {
}

SU1Integration::~SU1Integration() {
    shutdown();
}

bool SU1Integration::initialize(const SU1Config& config) {
    config_ = config;
    
    std::cout << "[S1U] Initializing SU1 Integration System..." << std::endl;
    
    // Check if SU1 is available on the system
    if (!check_su1_availability()) {
        std::cerr << "[S1U] SU1 not available on system" << std::endl;
        return false;
    }
    
    std::cout << "[S1U] SU1 Integration initialized successfully!" << std::endl;
    std::cout << "[S1U] SU1 Version: " << su1_version_ << std::endl;
    std::cout << "[S1U] Glass Theming: " << (glass_theming_enabled_ ? "Enabled" : "Disabled") << std::endl;
    
    return true;
}

void SU1Integration::shutdown() {
    // Stop all running SU1 applications
    for (auto& app : applications_) {
        if (app->running) {
            stop_su1_application(app->name);
        }
    }
    
    applications_.clear();
    app_map_.clear();
    
    std::cout << "[S1U] SU1 Integration shutdown complete" << std::endl;
}

bool SU1Integration::check_su1_availability() {
    // Check if SU1 is installed
    std::vector<std::string> su1_paths = {
        "/usr/local/bin/su1",
        "/usr/bin/su1",
        config_.su1_install_path + "/bin/su1"
    };
    
    for (const auto& path : su1_paths) {
        if (std::filesystem::exists(path)) {
            su1_available_ = true;
            
            // Try to get version
            FILE* pipe = popen((path + " --version").c_str(), "r");
            if (pipe) {
                char buffer[128];
                if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                    su1_version_ = std::string(buffer);
                    su1_version_.erase(su1_version_.find_last_not_of(" \n\r\t") + 1);
                }
                pclose(pipe);
            }
            
            std::cout << "[S1U] SU1 found at: " << path << std::endl;
            return true;
        }
    }
    
    return false;
}

bool SU1Integration::load_su1_application(const std::string& app_path, const std::string& app_name) {
    if (!su1_available_) {
        std::cerr << "[S1U] Cannot load SU1 app: SU1 not available" << std::endl;
        return false;
    }
    
    if (applications_.size() >= config_.max_su1_apps) {
        std::cerr << "[S1U] Cannot load SU1 app: Maximum apps reached" << std::endl;
        return false;
    }
    
    if (!validate_su1_application(app_path)) {
        std::cerr << "[S1U] Invalid SU1 application: " << app_path << std::endl;
        return false;
    }
    
    auto app = std::make_shared<SU1App>();
    app->name = app_name.empty() ? std::filesystem::path(app_path).stem().string() : app_name;
    app->path = app_path;
    app->executable = std::filesystem::path(app_path).filename().string();
    app->config_file = config_.su1_config_dir + "/" + app->name + ".conf";
    app->loaded = true;
    app->status = "Loaded";
    app->glass_theme_enabled = config_.enable_glass_theming;
    app->glass_opacity = config_.default_glass_opacity;
    app->glass_blur = config_.default_glass_blur;
    
    // Create window for the application
    if (!create_su1_window(app->name)) {
        std::cerr << "[S1U] Failed to create window for SU1 app: " << app->name << std::endl;
        return false;
    }
    
    applications_.push_back(app);
    app_map_[app->name] = app;
    
    std::cout << "[S1U] SU1 Application loaded: " << app->name << std::endl;
    
    if (event_callback_) {
        event_callback_(app->name, "loaded");
    }
    
    return true;
}

bool SU1Integration::unload_su1_application(const std::string& app_name) {
    auto it = app_map_.find(app_name);
    if (it == app_map_.end()) {
        std::cerr << "[S1U] SU1 Application not found: " << app_name << std::endl;
        return false;
    }
    
    auto app = it->second;
    
    if (app->running) {
        stop_su1_application(app_name);
    }
    
    // Remove from collections
    applications_.erase(std::remove(applications_.begin(), applications_.end(), app), applications_.end());
    app_map_.erase(it);
    
    std::cout << "[S1U] SU1 Application unloaded: " << app_name << std::endl;
    
    if (event_callback_) {
        event_callback_(app_name, "unloaded");
    }
    
    return true;
}

bool SU1Integration::start_su1_application(const std::string& app_name) {
    auto app = get_application(app_name);
    if (!app) {
        std::cerr << "[S1U] SU1 Application not found: " << app_name << std::endl;
        return false;
    }
    
    if (app->running) {
        std::cout << "[S1U] SU1 Application already running: " << app_name << std::endl;
        return true;
    }
    
    // Start the SU1 application process
    std::string command = app->path + " --display=:0 --theme=" + config_.default_theme;
    if (config_.enable_vsync) {
        command += " --vsync";
    }
    
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        app->running = true;
        app->status = "Running";
        app->last_update = std::chrono::high_resolution_clock::now();
        
        // Get PID (simplified)
        app->pid = getpid(); // This is simplified - in real implementation you'd get the actual PID
        
        pclose(pipe);
        
        std::cout << "[S1U] SU1 Application started: " << app_name << std::endl;
        
        if (event_callback_) {
            event_callback_(app_name, "started");
        }
        
        return true;
    }
    
    std::cerr << "[S1U] Failed to start SU1 Application: " << app_name << std::endl;
    return false;
}

bool SU1Integration::stop_su1_application(const std::string& app_name) {
    auto app = get_application(app_name);
    if (!app) {
        std::cerr << "[S1U] SU1 Application not found: " << app_name << std::endl;
        return false;
    }
    
    if (!app->running) {
        std::cout << "[S1U] SU1 Application not running: " << app_name << std::endl;
        return true;
    }
    
    // Stop the SU1 application process
    if (app->pid > 0) {
        std::string command = "kill " + std::to_string(app->pid);
        system(command.c_str());
    }
    
    app->running = false;
    app->status = "Stopped";
    app->pid = 0;
    
    std::cout << "[S1U] SU1 Application stopped: " << app_name << std::endl;
    
    if (event_callback_) {
        event_callback_(app_name, "stopped");
    }
    
    return true;
}

bool SU1Integration::restart_su1_application(const std::string& app_name) {
    if (!stop_su1_application(app_name)) {
        return false;
    }
    
    // Small delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    return start_su1_application(app_name);
}

std::vector<std::string> SU1Integration::discover_su1_applications() {
    std::vector<std::string> discovered_apps;
    
    if (!su1_available_) {
        return discovered_apps;
    }
    
    // Look for SU1 applications in common directories
    std::vector<std::string> search_paths = {
        config_.su1_install_path + "/bin",
        config_.su1_install_path + "/lib/su1",
        config_.su1_plugin_dir
    };
    
    for (const auto& path : search_paths) {
        if (std::filesystem::exists(path)) {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == "") {
                    std::string filename = entry.path().filename().string();
                    if (filename.find("su1_") == 0 || filename.find("su1-") == 0) {
                        discovered_apps.push_back(entry.path().string());
                    }
                }
            }
        }
    }
    
    return discovered_apps;
}

std::vector<SU1Integration::SU1App> SU1Integration::get_loaded_applications() const {
    std::vector<SU1App> result;
    for (const auto& app : applications_) {
        result.push_back(*app);
    }
    return result;
}

std::shared_ptr<SU1Integration::SU1App> SU1Integration::get_application(const std::string& app_name) const {
    auto it = app_map_.find(app_name);
    return (it != app_map_.end()) ? it->second : nullptr;
}

void SU1Integration::enable_glass_theming(bool enable) {
    glass_theming_enabled_ = enable;
    
    for (auto& app : applications_) {
        app->glass_theme_enabled = enable;
    }
    
    std::cout << "[S1U] Glass theming " << (enable ? "enabled" : "disabled") << std::endl;
}

void SU1Integration::set_glass_theme(const std::string& app_name, float opacity, float blur, float border) {
    auto app = get_application(app_name);
    if (app) {
        app->glass_opacity = opacity;
        app->glass_blur = blur;
        glass_themes_[app_name] = std::make_tuple(opacity, blur, border);
        
        std::cout << "[S1U] Glass theme set for " << app_name 
                  << " - Opacity: " << opacity 
                  << ", Blur: " << blur 
                  << ", Border: " << border << std::endl;
    }
}

void SU1Integration::apply_glass_effects(const std::string& app_name) {
    auto app = get_application(app_name);
    if (app && app->glass_theme_enabled) {
        // Apply glass effects to the application window
        if (app->window) {
            // In a real implementation, this would apply the glass effects
            // to the SU1 application window through the compositor
        }
    }
}

void SU1Integration::render_su1_applications(Renderer* renderer) {
    if (!renderer) return;
    
    for (auto& app : applications_) {
        if (app->loaded && app->window) {
            // Render the SU1 application window with glass effects
            if (app->glass_theme_enabled) {
                // Apply glass theming
                renderer->enable_glass_theme(true, app->glass_opacity, app->glass_blur, 2.0f);
            }
            
            // Render the application window
            // This would integrate with the window manager and compositor
        }
    }
}

void SU1Integration::update_su1_applications() {
    auto current_time = std::chrono::high_resolution_clock::now();
    
    for (auto& app : applications_) {
        if (app->running) {
            update_application_stats(*app);
            app->last_update = current_time;
        }
    }
    
    // Update global stats
    if (std::chrono::duration_cast<std::chrono::seconds>(current_time - last_stats_update_).count() >= 1) {
        update_global_stats();
        last_stats_update_ = current_time;
    }
    
    // Handle events
    handle_su1_events();
}

bool SU1Integration::validate_su1_application(const std::string& app_path) {
    if (!std::filesystem::exists(app_path)) {
        return false;
    }
    
    // Check if it's executable
    struct stat st;
    if (stat(app_path.c_str(), &st) != 0) {
        return false;
    }
    
    return (st.st_mode & S_IXUSR) != 0;
}

bool SU1Integration::create_su1_window(const std::string& app_name) {
    // Create a window for the SU1 application
    // This would integrate with the window manager
    return true;
}

void SU1Integration::update_application_stats(SU1App& app) {
    if (app.running) {
        // Update CPU and memory usage (simplified)
        app.cpu_usage = static_cast<float>(rand()) / RAND_MAX * 100.0f;
        app.memory_usage = static_cast<float>(rand()) / RAND_MAX * 512.0f;
    }
}

void SU1Integration::update_global_stats() {
    total_cpu_usage_ = 0.0f;
    total_memory_usage_ = 0.0f;
    
    for (const auto& app : applications_) {
        if (app->running) {
            total_cpu_usage_ += app->cpu_usage;
            total_memory_usage_ += app->memory_usage;
        }
    }
}

void SU1Integration::handle_su1_events() {
    // Process event queue
    for (const auto& event : event_queue_) {
        // Handle events
    }
    event_queue_.clear();
}

uint32_t SU1Integration::get_running_app_count() const {
    uint32_t count = 0;
    for (const auto& app : applications_) {
        if (app->running) count++;
    }
    return count;
}

float SU1Integration::get_total_cpu_usage() const {
    return total_cpu_usage_;
}

float SU1Integration::get_total_memory_usage() const {
    return total_memory_usage_;
}

void SU1Integration::register_event_callback(SU1EventCallback callback) {
    event_callback_ = callback;
}

void SU1Integration::unregister_event_callback() {
    event_callback_ = nullptr;
}

void SU1Integration::update_config(const SU1Config& config) {
    config_ = config;
    
    // Apply new configuration to existing applications
    for (auto& app : applications_) {
        app->glass_theme_enabled = config.enable_glass_theming;
        app->glass_opacity = config.default_glass_opacity;
        app->glass_blur = config.default_glass_blur;
    }
    
    std::cout << "[S1U] SU1 Integration configuration updated" << std::endl;
}

} // namespace s1u

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <thread>

namespace s1u {

SU1Integration::SU1Integration()
    : su1_available_(false)
    , su1_version_("unknown")
    , glass_theming_enabled_(true)
    , total_cpu_usage_(0.0f)
    , total_memory_usage_(0.0f) {
}

SU1Integration::~SU1Integration() {
    shutdown();
}

bool SU1Integration::initialize(const SU1Config& config) {
    config_ = config;
    
    std::cout << "[S1U] Initializing SU1 Integration System..." << std::endl;
    
    // Check if SU1 is available on the system
    if (!check_su1_availability()) {
        std::cerr << "[S1U] SU1 not available on system" << std::endl;
        return false;
    }
    
    std::cout << "[S1U] SU1 Integration initialized successfully!" << std::endl;
    std::cout << "[S1U] SU1 Version: " << su1_version_ << std::endl;
    std::cout << "[S1U] Glass Theming: " << (glass_theming_enabled_ ? "Enabled" : "Disabled") << std::endl;
    
    return true;
}

void SU1Integration::shutdown() {
    // Stop all running SU1 applications
    for (auto& app : applications_) {
        if (app->running) {
            stop_su1_application(app->name);
        }
    }
    
    applications_.clear();
    app_map_.clear();
    
    std::cout << "[S1U] SU1 Integration shutdown complete" << std::endl;
}

bool SU1Integration::check_su1_availability() {
    // Check if SU1 is installed
    std::vector<std::string> su1_paths = {
        "/usr/local/bin/su1",
        "/usr/bin/su1",
        config_.su1_install_path + "/bin/su1"
    };
    
    for (const auto& path : su1_paths) {
        if (std::filesystem::exists(path)) {
            su1_available_ = true;
            
            // Try to get version
            FILE* pipe = popen((path + " --version").c_str(), "r");
            if (pipe) {
                char buffer[128];
                if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                    su1_version_ = std::string(buffer);
                    su1_version_.erase(su1_version_.find_last_not_of(" \n\r\t") + 1);
                }
                pclose(pipe);
            }
            
            std::cout << "[S1U] SU1 found at: " << path << std::endl;
            return true;
        }
    }
    
    return false;
}

bool SU1Integration::load_su1_application(const std::string& app_path, const std::string& app_name) {
    if (!su1_available_) {
        std::cerr << "[S1U] Cannot load SU1 app: SU1 not available" << std::endl;
        return false;
    }
    
    if (applications_.size() >= config_.max_su1_apps) {
        std::cerr << "[S1U] Cannot load SU1 app: Maximum apps reached" << std::endl;
        return false;
    }
    
    if (!validate_su1_application(app_path)) {
        std::cerr << "[S1U] Invalid SU1 application: " << app_path << std::endl;
        return false;
    }
    
    auto app = std::make_shared<SU1App>();
    app->name = app_name.empty() ? std::filesystem::path(app_path).stem().string() : app_name;
    app->path = app_path;
    app->executable = std::filesystem::path(app_path).filename().string();
    app->config_file = config_.su1_config_dir + "/" + app->name + ".conf";
    app->loaded = true;
    app->status = "Loaded";
    app->glass_theme_enabled = config_.enable_glass_theming;
    app->glass_opacity = config_.default_glass_opacity;
    app->glass_blur = config_.default_glass_blur;
    
    // Create window for the application
    if (!create_su1_window(app->name)) {
        std::cerr << "[S1U] Failed to create window for SU1 app: " << app->name << std::endl;
        return false;
    }
    
    applications_.push_back(app);
    app_map_[app->name] = app;
    
    std::cout << "[S1U] SU1 Application loaded: " << app->name << std::endl;
    
    if (event_callback_) {
        event_callback_(app->name, "loaded");
    }
    
    return true;
}

bool SU1Integration::unload_su1_application(const std::string& app_name) {
    auto it = app_map_.find(app_name);
    if (it == app_map_.end()) {
        std::cerr << "[S1U] SU1 Application not found: " << app_name << std::endl;
        return false;
    }
    
    auto app = it->second;
    
    if (app->running) {
        stop_su1_application(app_name);
    }
    
    // Remove from collections
    applications_.erase(std::remove(applications_.begin(), applications_.end(), app), applications_.end());
    app_map_.erase(it);
    
    std::cout << "[S1U] SU1 Application unloaded: " << app_name << std::endl;
    
    if (event_callback_) {
        event_callback_(app_name, "unloaded");
    }
    
    return true;
}

bool SU1Integration::start_su1_application(const std::string& app_name) {
    auto app = get_application(app_name);
    if (!app) {
        std::cerr << "[S1U] SU1 Application not found: " << app_name << std::endl;
        return false;
    }
    
    if (app->running) {
        std::cout << "[S1U] SU1 Application already running: " << app_name << std::endl;
        return true;
    }
    
    // Start the SU1 application process
    std::string command = app->path + " --display=:0 --theme=" + config_.default_theme;
    if (config_.enable_vsync) {
        command += " --vsync";
    }
    
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        app->running = true;
        app->status = "Running";
        app->last_update = std::chrono::high_resolution_clock::now();
        
        // Get PID (simplified)
        app->pid = getpid(); // This is simplified - in real implementation you'd get the actual PID
        
        pclose(pipe);
        
        std::cout << "[S1U] SU1 Application started: " << app_name << std::endl;
        
        if (event_callback_) {
            event_callback_(app_name, "started");
        }
        
        return true;
    }
    
    std::cerr << "[S1U] Failed to start SU1 Application: " << app_name << std::endl;
    return false;
}

bool SU1Integration::stop_su1_application(const std::string& app_name) {
    auto app = get_application(app_name);
    if (!app) {
        std::cerr << "[S1U] SU1 Application not found: " << app_name << std::endl;
        return false;
    }
    
    if (!app->running) {
        std::cout << "[S1U] SU1 Application not running: " << app_name << std::endl;
        return true;
    }
    
    // Stop the SU1 application process
    if (app->pid > 0) {
        std::string command = "kill " + std::to_string(app->pid);
        system(command.c_str());
    }
    
    app->running = false;
    app->status = "Stopped";
    app->pid = 0;
    
    std::cout << "[S1U] SU1 Application stopped: " << app_name << std::endl;
    
    if (event_callback_) {
        event_callback_(app_name, "stopped");
    }
    
    return true;
}

bool SU1Integration::restart_su1_application(const std::string& app_name) {
    if (!stop_su1_application(app_name)) {
        return false;
    }
    
    // Small delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    return start_su1_application(app_name);
}

std::vector<std::string> SU1Integration::discover_su1_applications() {
    std::vector<std::string> discovered_apps;
    
    if (!su1_available_) {
        return discovered_apps;
    }
    
    // Look for SU1 applications in common directories
    std::vector<std::string> search_paths = {
        config_.su1_install_path + "/bin",
        config_.su1_install_path + "/lib/su1",
        config_.su1_plugin_dir
    };
    
    for (const auto& path : search_paths) {
        if (std::filesystem::exists(path)) {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == "") {
                    std::string filename = entry.path().filename().string();
                    if (filename.find("su1_") == 0 || filename.find("su1-") == 0) {
                        discovered_apps.push_back(entry.path().string());
                    }
                }
            }
        }
    }
    
    return discovered_apps;
}

std::vector<SU1Integration::SU1App> SU1Integration::get_loaded_applications() const {
    std::vector<SU1App> result;
    for (const auto& app : applications_) {
        result.push_back(*app);
    }
    return result;
}

std::shared_ptr<SU1Integration::SU1App> SU1Integration::get_application(const std::string& app_name) const {
    auto it = app_map_.find(app_name);
    return (it != app_map_.end()) ? it->second : nullptr;
}

void SU1Integration::enable_glass_theming(bool enable) {
    glass_theming_enabled_ = enable;
    
    for (auto& app : applications_) {
        app->glass_theme_enabled = enable;
    }
    
    std::cout << "[S1U] Glass theming " << (enable ? "enabled" : "disabled") << std::endl;
}

void SU1Integration::set_glass_theme(const std::string& app_name, float opacity, float blur, float border) {
    auto app = get_application(app_name);
    if (app) {
        app->glass_opacity = opacity;
        app->glass_blur = blur;
        glass_themes_[app_name] = std::make_tuple(opacity, blur, border);
        
        std::cout << "[S1U] Glass theme set for " << app_name 
                  << " - Opacity: " << opacity 
                  << ", Blur: " << blur 
                  << ", Border: " << border << std::endl;
    }
}

void SU1Integration::apply_glass_effects(const std::string& app_name) {
    auto app = get_application(app_name);
    if (app && app->glass_theme_enabled) {
        // Apply glass effects to the application window
        if (app->window) {
            // In a real implementation, this would apply the glass effects
            // to the SU1 application window through the compositor
        }
    }
}

void SU1Integration::render_su1_applications(Renderer* renderer) {
    if (!renderer) return;
    
    for (auto& app : applications_) {
        if (app->loaded && app->window) {
            // Render the SU1 application window with glass effects
            if (app->glass_theme_enabled) {
                // Apply glass theming
                renderer->enable_glass_theme(true, app->glass_opacity, app->glass_blur, 2.0f);
            }
            
            // Render the application window
            // This would integrate with the window manager and compositor
        }
    }
}

void SU1Integration::update_su1_applications() {
    auto current_time = std::chrono::high_resolution_clock::now();
    
    for (auto& app : applications_) {
        if (app->running) {
            update_application_stats(*app);
            app->last_update = current_time;
        }
    }
    
    // Update global stats
    if (std::chrono::duration_cast<std::chrono::seconds>(current_time - last_stats_update_).count() >= 1) {
        update_global_stats();
        last_stats_update_ = current_time;
    }
    
    // Handle events
    handle_su1_events();
}

bool SU1Integration::validate_su1_application(const std::string& app_path) {
    if (!std::filesystem::exists(app_path)) {
        return false;
    }
    
    // Check if it's executable
    struct stat st;
    if (stat(app_path.c_str(), &st) != 0) {
        return false;
    }
    
    return (st.st_mode & S_IXUSR) != 0;
}

bool SU1Integration::create_su1_window(const std::string& app_name) {
    // Create a window for the SU1 application
    // This would integrate with the window manager
    return true;
}

void SU1Integration::update_application_stats(SU1App& app) {
    if (app.running) {
        // Update CPU and memory usage (simplified)
        app.cpu_usage = static_cast<float>(rand()) / RAND_MAX * 100.0f;
        app.memory_usage = static_cast<float>(rand()) / RAND_MAX * 512.0f;
    }
}

void SU1Integration::update_global_stats() {
    total_cpu_usage_ = 0.0f;
    total_memory_usage_ = 0.0f;
    
    for (const auto& app : applications_) {
        if (app->running) {
            total_cpu_usage_ += app->cpu_usage;
            total_memory_usage_ += app->memory_usage;
        }
    }
}

void SU1Integration::handle_su1_events() {
    // Process event queue
    for (const auto& event : event_queue_) {
        // Handle events
    }
    event_queue_.clear();
}

uint32_t SU1Integration::get_running_app_count() const {
    uint32_t count = 0;
    for (const auto& app : applications_) {
        if (app->running) count++;
    }
    return count;
}

float SU1Integration::get_total_cpu_usage() const {
    return total_cpu_usage_;
}

float SU1Integration::get_total_memory_usage() const {
    return total_memory_usage_;
}

void SU1Integration::register_event_callback(SU1EventCallback callback) {
    event_callback_ = callback;
}

void SU1Integration::unregister_event_callback() {
    event_callback_ = nullptr;
}

void SU1Integration::update_config(const SU1Config& config) {
    config_ = config;
    
    // Apply new configuration to existing applications
    for (auto& app : applications_) {
        app->glass_theme_enabled = config.enable_glass_theming;
        app->glass_opacity = config.default_glass_opacity;
        app->glass_blur = config.default_glass_blur;
    }
    
    std::cout << "[S1U] SU1 Integration configuration updated" << std::endl;
}

} // namespace s1u

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <thread>

namespace s1u {

SU1Integration::SU1Integration()
    : su1_available_(false)
    , su1_version_("unknown")
    , glass_theming_enabled_(true)
    , total_cpu_usage_(0.0f)
    , total_memory_usage_(0.0f) {
}

SU1Integration::~SU1Integration() {
    shutdown();
}

bool SU1Integration::initialize(const SU1Config& config) {
    config_ = config;
    
    std::cout << "[S1U] Initializing SU1 Integration System..." << std::endl;
    
    // Check if SU1 is available on the system
    if (!check_su1_availability()) {
        std::cerr << "[S1U] SU1 not available on system" << std::endl;
        return false;
    }
    
    std::cout << "[S1U] SU1 Integration initialized successfully!" << std::endl;
    std::cout << "[S1U] SU1 Version: " << su1_version_ << std::endl;
    std::cout << "[S1U] Glass Theming: " << (glass_theming_enabled_ ? "Enabled" : "Disabled") << std::endl;
    
    return true;
}

void SU1Integration::shutdown() {
    // Stop all running SU1 applications
    for (auto& app : applications_) {
        if (app->running) {
            stop_su1_application(app->name);
        }
    }
    
    applications_.clear();
    app_map_.clear();
    
    std::cout << "[S1U] SU1 Integration shutdown complete" << std::endl;
}

bool SU1Integration::check_su1_availability() {
    // Check if SU1 is installed
    std::vector<std::string> su1_paths = {
        "/usr/local/bin/su1",
        "/usr/bin/su1",
        config_.su1_install_path + "/bin/su1"
    };
    
    for (const auto& path : su1_paths) {
        if (std::filesystem::exists(path)) {
            su1_available_ = true;
            
            // Try to get version
            FILE* pipe = popen((path + " --version").c_str(), "r");
            if (pipe) {
                char buffer[128];
                if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                    su1_version_ = std::string(buffer);
                    su1_version_.erase(su1_version_.find_last_not_of(" \n\r\t") + 1);
                }
                pclose(pipe);
            }
            
            std::cout << "[S1U] SU1 found at: " << path << std::endl;
            return true;
        }
    }
    
    return false;
}

bool SU1Integration::load_su1_application(const std::string& app_path, const std::string& app_name) {
    if (!su1_available_) {
        std::cerr << "[S1U] Cannot load SU1 app: SU1 not available" << std::endl;
        return false;
    }
    
    if (applications_.size() >= config_.max_su1_apps) {
        std::cerr << "[S1U] Cannot load SU1 app: Maximum apps reached" << std::endl;
        return false;
    }
    
    if (!validate_su1_application(app_path)) {
        std::cerr << "[S1U] Invalid SU1 application: " << app_path << std::endl;
        return false;
    }
    
    auto app = std::make_shared<SU1App>();
    app->name = app_name.empty() ? std::filesystem::path(app_path).stem().string() : app_name;
    app->path = app_path;
    app->executable = std::filesystem::path(app_path).filename().string();
    app->config_file = config_.su1_config_dir + "/" + app->name + ".conf";
    app->loaded = true;
    app->status = "Loaded";
    app->glass_theme_enabled = config_.enable_glass_theming;
    app->glass_opacity = config_.default_glass_opacity;
    app->glass_blur = config_.default_glass_blur;
    
    // Create window for the application
    if (!create_su1_window(app->name)) {
        std::cerr << "[S1U] Failed to create window for SU1 app: " << app->name << std::endl;
        return false;
    }
    
    applications_.push_back(app);
    app_map_[app->name] = app;
    
    std::cout << "[S1U] SU1 Application loaded: " << app->name << std::endl;
    
    if (event_callback_) {
        event_callback_(app->name, "loaded");
    }
    
    return true;
}

bool SU1Integration::unload_su1_application(const std::string& app_name) {
    auto it = app_map_.find(app_name);
    if (it == app_map_.end()) {
        std::cerr << "[S1U] SU1 Application not found: " << app_name << std::endl;
        return false;
    }
    
    auto app = it->second;
    
    if (app->running) {
        stop_su1_application(app_name);
    }
    
    // Remove from collections
    applications_.erase(std::remove(applications_.begin(), applications_.end(), app), applications_.end());
    app_map_.erase(it);
    
    std::cout << "[S1U] SU1 Application unloaded: " << app_name << std::endl;
    
    if (event_callback_) {
        event_callback_(app_name, "unloaded");
    }
    
    return true;
}

bool SU1Integration::start_su1_application(const std::string& app_name) {
    auto app = get_application(app_name);
    if (!app) {
        std::cerr << "[S1U] SU1 Application not found: " << app_name << std::endl;
        return false;
    }
    
    if (app->running) {
        std::cout << "[S1U] SU1 Application already running: " << app_name << std::endl;
        return true;
    }
    
    // Start the SU1 application process
    std::string command = app->path + " --display=:0 --theme=" + config_.default_theme;
    if (config_.enable_vsync) {
        command += " --vsync";
    }
    
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        app->running = true;
        app->status = "Running";
        app->last_update = std::chrono::high_resolution_clock::now();
        
        // Get PID (simplified)
        app->pid = getpid(); // This is simplified - in real implementation you'd get the actual PID
        
        pclose(pipe);
        
        std::cout << "[S1U] SU1 Application started: " << app_name << std::endl;
        
        if (event_callback_) {
            event_callback_(app_name, "started");
        }
        
        return true;
    }
    
    std::cerr << "[S1U] Failed to start SU1 Application: " << app_name << std::endl;
    return false;
}

bool SU1Integration::stop_su1_application(const std::string& app_name) {
    auto app = get_application(app_name);
    if (!app) {
        std::cerr << "[S1U] SU1 Application not found: " << app_name << std::endl;
        return false;
    }
    
    if (!app->running) {
        std::cout << "[S1U] SU1 Application not running: " << app_name << std::endl;
        return true;
    }
    
    // Stop the SU1 application process
    if (app->pid > 0) {
        std::string command = "kill " + std::to_string(app->pid);
        system(command.c_str());
    }
    
    app->running = false;
    app->status = "Stopped";
    app->pid = 0;
    
    std::cout << "[S1U] SU1 Application stopped: " << app_name << std::endl;
    
    if (event_callback_) {
        event_callback_(app_name, "stopped");
    }
    
    return true;
}

bool SU1Integration::restart_su1_application(const std::string& app_name) {
    if (!stop_su1_application(app_name)) {
        return false;
    }
    
    // Small delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    return start_su1_application(app_name);
}

std::vector<std::string> SU1Integration::discover_su1_applications() {
    std::vector<std::string> discovered_apps;
    
    if (!su1_available_) {
        return discovered_apps;
    }
    
    // Look for SU1 applications in common directories
    std::vector<std::string> search_paths = {
        config_.su1_install_path + "/bin",
        config_.su1_install_path + "/lib/su1",
        config_.su1_plugin_dir
    };
    
    for (const auto& path : search_paths) {
        if (std::filesystem::exists(path)) {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == "") {
                    std::string filename = entry.path().filename().string();
                    if (filename.find("su1_") == 0 || filename.find("su1-") == 0) {
                        discovered_apps.push_back(entry.path().string());
                    }
                }
            }
        }
    }
    
    return discovered_apps;
}

std::vector<SU1Integration::SU1App> SU1Integration::get_loaded_applications() const {
    std::vector<SU1App> result;
    for (const auto& app : applications_) {
        result.push_back(*app);
    }
    return result;
}

std::shared_ptr<SU1Integration::SU1App> SU1Integration::get_application(const std::string& app_name) const {
    auto it = app_map_.find(app_name);
    return (it != app_map_.end()) ? it->second : nullptr;
}

void SU1Integration::enable_glass_theming(bool enable) {
    glass_theming_enabled_ = enable;
    
    for (auto& app : applications_) {
        app->glass_theme_enabled = enable;
    }
    
    std::cout << "[S1U] Glass theming " << (enable ? "enabled" : "disabled") << std::endl;
}

void SU1Integration::set_glass_theme(const std::string& app_name, float opacity, float blur, float border) {
    auto app = get_application(app_name);
    if (app) {
        app->glass_opacity = opacity;
        app->glass_blur = blur;
        glass_themes_[app_name] = std::make_tuple(opacity, blur, border);
        
        std::cout << "[S1U] Glass theme set for " << app_name 
                  << " - Opacity: " << opacity 
                  << ", Blur: " << blur 
                  << ", Border: " << border << std::endl;
    }
}

void SU1Integration::apply_glass_effects(const std::string& app_name) {
    auto app = get_application(app_name);
    if (app && app->glass_theme_enabled) {
        // Apply glass effects to the application window
        if (app->window) {
            // In a real implementation, this would apply the glass effects
            // to the SU1 application window through the compositor
        }
    }
}

void SU1Integration::render_su1_applications(Renderer* renderer) {
    if (!renderer) return;
    
    for (auto& app : applications_) {
        if (app->loaded && app->window) {
            // Render the SU1 application window with glass effects
            if (app->glass_theme_enabled) {
                // Apply glass theming
                renderer->enable_glass_theme(true, app->glass_opacity, app->glass_blur, 2.0f);
            }
            
            // Render the application window
            // This would integrate with the window manager and compositor
        }
    }
}

void SU1Integration::update_su1_applications() {
    auto current_time = std::chrono::high_resolution_clock::now();
    
    for (auto& app : applications_) {
        if (app->running) {
            update_application_stats(*app);
            app->last_update = current_time;
        }
    }
    
    // Update global stats
    if (std::chrono::duration_cast<std::chrono::seconds>(current_time - last_stats_update_).count() >= 1) {
        update_global_stats();
        last_stats_update_ = current_time;
    }
    
    // Handle events
    handle_su1_events();
}

bool SU1Integration::validate_su1_application(const std::string& app_path) {
    if (!std::filesystem::exists(app_path)) {
        return false;
    }
    
    // Check if it's executable
    struct stat st;
    if (stat(app_path.c_str(), &st) != 0) {
        return false;
    }
    
    return (st.st_mode & S_IXUSR) != 0;
}

bool SU1Integration::create_su1_window(const std::string& app_name) {
    // Create a window for the SU1 application
    // This would integrate with the window manager
    return true;
}

void SU1Integration::update_application_stats(SU1App& app) {
    if (app.running) {
        // Update CPU and memory usage (simplified)
        app.cpu_usage = static_cast<float>(rand()) / RAND_MAX * 100.0f;
        app.memory_usage = static_cast<float>(rand()) / RAND_MAX * 512.0f;
    }
}

void SU1Integration::update_global_stats() {
    total_cpu_usage_ = 0.0f;
    total_memory_usage_ = 0.0f;
    
    for (const auto& app : applications_) {
        if (app->running) {
            total_cpu_usage_ += app->cpu_usage;
            total_memory_usage_ += app->memory_usage;
        }
    }
}

void SU1Integration::handle_su1_events() {
    // Process event queue
    for (const auto& event : event_queue_) {
        // Handle events
    }
    event_queue_.clear();
}

uint32_t SU1Integration::get_running_app_count() const {
    uint32_t count = 0;
    for (const auto& app : applications_) {
        if (app->running) count++;
    }
    return count;
}

float SU1Integration::get_total_cpu_usage() const {
    return total_cpu_usage_;
}

float SU1Integration::get_total_memory_usage() const {
    return total_memory_usage_;
}

void SU1Integration::register_event_callback(SU1EventCallback callback) {
    event_callback_ = callback;
}

void SU1Integration::unregister_event_callback() {
    event_callback_ = nullptr;
}

void SU1Integration::update_config(const SU1Config& config) {
    config_ = config;
    
    // Apply new configuration to existing applications
    for (auto& app : applications_) {
        app->glass_theme_enabled = config.enable_glass_theming;
        app->glass_opacity = config.default_glass_opacity;
        app->glass_blur = config.default_glass_blur;
    }
    
    std::cout << "[S1U] SU1 Integration configuration updated" << std::endl;
}

} // namespace s1u
