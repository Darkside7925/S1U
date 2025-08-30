#pragma once

#include "s1u/core.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace s1u {

class DriverManager {
public:
    struct DriverInfo {
        std::string name;
        std::string version;
        std::string vendor;
        std::string type;
        std::string description;
        std::vector<std::string> supported_hardware;
        std::vector<std::string> dependencies;
        bool is_loaded;
        bool is_active;
        bool is_builtin;
        void* handle;
        std::chrono::steady_clock::time_point load_time;
        std::unordered_map<std::string, std::string> capabilities;
        std::unordered_map<std::string, std::string> configuration;
    };

    struct HardwareDevice {
        std::string device_id;
        std::string device_name;
        std::string device_type;
        std::string vendor;
        std::string driver_name;
        bool is_supported;
        bool is_active;
        std::unordered_map<std::string, std::string> properties;
        std::chrono::steady_clock::time_point detected_time;
    };

    DriverManager();
    ~DriverManager();

    bool initialize();
    void shutdown();

    // Driver management
    bool load_driver(const std::string& driver_name);
    bool unload_driver(const std::string& driver_name);
    std::vector<std::string> get_loaded_drivers() const;
    std::vector<std::string> get_available_drivers() const;
    DriverInfo get_driver_info(const std::string& driver_name) const;
    bool is_driver_loaded(const std::string& driver_name) const;

    // Hardware detection
    std::vector<HardwareDevice> get_detected_hardware() const;

    // Driver search paths
    void add_driver_search_path(const std::string& path);
    void remove_driver_search_path(const std::string& path);

    // Auto-detection
    void set_auto_detection_enabled(bool enabled);
    void set_hotplug_enabled(bool enabled);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace s1u
