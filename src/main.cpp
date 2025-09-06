#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <fstream>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <unistd.h>
#include "s1u/display_server.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::atomic<bool> running = true;

class S1UServer {
public:
    S1UServer() {}

    bool initialize() {
        std::cout << "==========================================" << std::endl;
        std::cout << "     S1U Display Server v1.0.0" << std::endl;
        std::cout << "==========================================" << std::endl;
        std::cout << std::endl;

        std::cout << "[INIT] Initializing S1U Display Server..." << std::endl;
        
        // Create display server configuration
        s1u::DisplayServerConfig config;
        config.width = 1920;
        config.height = 1080;
        config.refresh_rate = 60;
        config.vsync = true;
        config.fullscreen = false;
        config.borderless = false;
        config.title = "S1U Display Server";
        config.theme = "liquid_glass";
        config.enable_compositor = true;
        config.enable_quantum_effects = true;
        config.max_fps = 144;

        // Initialize the real display server
        display_server_ = std::make_unique<s1u::DisplayServer>();
        if (!display_server_->initialize(config)) {
            std::cerr << "[INIT] Failed to initialize display server" << std::endl;
            return false;
        }

        std::cout << "[INIT] S1U Server initialization completed!" << std::endl;
        std::cout << "[INIT] Ready for extreme performance display server operations" << std::endl;
        std::cout << std::endl;

        return true;
    }

private:
    void print_system_info() {
        struct utsname uname_data;
        if (uname(&uname_data) == 0) {
            std::cout << "[SYSTEM] Kernel: " << uname_data.sysname << " " << uname_data.release
                      << " (" << uname_data.machine << ")" << std::endl;
            std::cout << "[SYSTEM] Hostname: " << uname_data.nodename << std::endl;
        }

        struct sysinfo sys_info;
        if (sysinfo(&sys_info) == 0) {
            std::cout << "[SYSTEM] Uptime: " << sys_info.uptime << " seconds" << std::endl;
            std::cout << "[SYSTEM] RAM: " << (sys_info.totalram / 1024 / 1024) << " MB total, "
                      << (sys_info.freeram / 1024 / 1024) << " MB free" << std::endl;
        }
    }

    void detect_graphics_drivers() {
        // Check for NVIDIA drivers
        if (std::ifstream("/proc/driver/nvidia/version")) {
            std::cout << "[DRIVER] NVIDIA driver detected and loaded" << std::endl;
        } else {
            std::cout << "[DRIVER] NVIDIA driver not found" << std::endl;
        }

        // Check for AMD drivers
        if (std::ifstream("/sys/module/amdgpu")) {
            std::cout << "[DRIVER] AMDGPU driver detected and loaded" << std::endl;
        }

        // Check for Intel drivers
        if (std::ifstream("/sys/module/i915")) {
            std::cout << "[DRIVER] Intel i915 driver detected and loaded" << std::endl;
        }

        // Check for DRM/KMS
        if (std::ifstream("/sys/class/drm")) {
            std::cout << "[DRIVER] DRM/KMS driver detected and loaded" << std::endl;
        }
    }

    void detect_input_drivers() {
        DIR* dir = opendir("/sys/class/input");
        if (dir) {
            std::cout << "[DRIVER] Input subsystem driver detected and loaded" << std::endl;
            closedir(dir);
        }

        // Check for specific input drivers
        if (std::ifstream("/sys/module/hid")) {
            std::cout << "[DRIVER] HID driver detected and loaded" << std::endl;
        }

        if (std::ifstream("/sys/module/usbhid")) {
            std::cout << "[DRIVER] USB HID driver detected and loaded" << std::endl;
        }
    }

    void detect_graphics_hardware() {
        // Read GPU information from sysfs
        DIR* dir = opendir("/sys/class/drm");
        if (dir) {
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr) {
                std::string name = entry->d_name;
                if (name.find("card") == 0) {
                    std::string card_path = "/sys/class/drm/" + name;

                    // Try to read GPU name
                    std::ifstream device_file(card_path + "/device/device");
                    std::ifstream vendor_file(card_path + "/device/vendor");

                    std::string device_id, vendor_id;
                    if (device_file >> device_id && vendor_file >> vendor_id) {
                        std::cout << "[HARDWARE] Found GPU: ";

                        if (vendor_id == "0x10de") {
                            std::cout << "NVIDIA ";
                        } else if (vendor_id == "0x1002") {
                            std::cout << "AMD ";
                        } else if (vendor_id == "0x8086") {
                            std::cout << "Intel ";
                        }

                        std::cout << "(Device ID: " << device_id << ")" << std::endl;
                    }

                    // Read GPU memory if available
                    std::ifstream mem_file(card_path + "/device/mem_info_vram_total");
                    if (mem_file) {
                        std::string mem_size;
                        if (mem_file >> mem_size) {
                            long long mem_bytes = std::stoll(mem_size);
                            std::cout << "[HARDWARE] GPU Memory: " << (mem_bytes / 1024 / 1024) << " MB" << std::endl;
                        }
                    }
                }
            }
            closedir(dir);
        }
    }

    void detect_display_hardware() {
        DIR* dir = opendir("/sys/class/drm");
        if (dir) {
            struct dirent* entry;
            int display_count = 0;

            while ((entry = readdir(dir)) != nullptr) {
                std::string name = entry->d_name;

                // Look for connectors (displays)
                if (name.find("card") == 0 && name.find("-") != std::string::npos) {
                    std::string connector_path = "/sys/class/drm/" + name;

                    // Check if display is connected
                    std::ifstream status_file(connector_path + "/status");
                    if (status_file) {
                        std::string status;
                        if (status_file >> status && status == "connected") {
                            display_count++;

                            // Try to read EDID for display info
                            std::ifstream edid_file(connector_path + "/edid");
                            if (edid_file) {
                                std::cout << "[HARDWARE] Found connected display #" << display_count << std::endl;

                                // Read modes
                                std::ifstream modes_file(connector_path + "/modes");
                                if (modes_file) {
                                    std::string mode;
                                    if (std::getline(modes_file, mode)) {
                                        std::cout << "[HARDWARE] Display mode: " << mode << std::endl;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (display_count == 0) {
                std::cout << "[HARDWARE] No connected displays detected" << std::endl;
            }

            closedir(dir);
        }
    }

    void detect_input_hardware() {
        DIR* dir = opendir("/sys/class/input");
        if (dir) {
            struct dirent* entry;
            int keyboard_count = 0;
            int mouse_count = 0;

            while ((entry = readdir(dir)) != nullptr) {
                std::string name = entry->d_name;
                if (name.find("event") == 0) {
                    std::string event_path = "/sys/class/input/" + name;

                    // Check device name
                    std::ifstream name_file(event_path + "/device/name");
                    if (name_file) {
                        std::string device_name;
                        if (std::getline(name_file, device_name)) {
                            // Check if it's a keyboard
                            if (device_name.find("keyboard") != std::string::npos ||
                                device_name.find("Keyboard") != std::string::npos) {
                                keyboard_count++;
                                std::cout << "[HARDWARE] Found keyboard: " << device_name << std::endl;
                            }
                            // Check if it's a mouse
                            else if (device_name.find("mouse") != std::string::npos ||
                                     device_name.find("Mouse") != std::string::npos ||
                                     device_name.find("touchpad") != std::string::npos) {
                                mouse_count++;
                                std::cout << "[HARDWARE] Found pointing device: " << device_name << std::endl;
                            }
                        }
                    }
                }
            }

            std::cout << "[HARDWARE] Total keyboards: " << keyboard_count << std::endl;
            std::cout << "[HARDWARE] Total pointing devices: " << mouse_count << std::endl;

            closedir(dir);
        }
    }

public:
    void run() {
        std::cout << "[RUN] S1U Display Server starting main loop..." << std::endl;
        std::cout << "[RUN] Target: 144Hz refresh rate with vsync" << std::endl;
        std::cout << std::endl;

        // Start the display server
        display_server_->run();

        // Load a demo SU1 application
        std::cout << "[RUN] Loading demo SU1 application..." << std::endl;
        display_server_->load_su1_application("demo_app");

        // Main loop
        while (running) {
            // The display server runs in its own thread
            // We just monitor the status here
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            
            // Show stats
            static int stats_counter = 0;
            if (++stats_counter % 10 == 0) {
                std::cout << "[STATS] FPS: " << display_server_->get_current_fps()
                         << " | Frame: " << display_server_->get_frame_count()
                         << " | Avg Frame Time: " << display_server_->get_average_frame_time() * 1000.0 << "ms" << std::endl;
            }
        }

        shutdown();
    }

    void shutdown() {
        std::cout << std::endl;
        std::cout << "[SHUTDOWN] S1U Display Server shutting down..." << std::endl;
        
        if (display_server_) {
            display_server_->stop();
            display_server_->shutdown();
        }
        
        std::cout << "[SHUTDOWN] S1U Server shutdown complete!" << std::endl;
        std::cout << "==========================================" << std::endl;
    }

private:
    std::unique_ptr<s1u::DisplayServer> display_server_;
};

int main(int argc, char* argv[]) {
    std::cout << "Starting S1U REAL Display Server..." << std::endl;
    std::cout << "Built for 144Hz refresh rates with vsync support" << std::endl;
    std::cout << "Real OpenGL rendering with window management!" << std::endl;
    std::cout << "Supports SU1 application integration" << std::endl;
    std::cout << std::endl;

    S1UServer server;

    if (!server.initialize()) {
        std::cerr << "Failed to initialize S1U server" << std::endl;
        return 1;
    }

    server.run();

    return 0;
}
