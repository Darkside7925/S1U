#include "s1u/core.hpp"
#include "s1u/window.hpp"
#include "s1u/display.hpp"
#include <iostream>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <poll.h>

namespace s1u {

static volatile bool running = true;

void signal_handler(int sig) {
    running = false;
}

class S1UServer {
public:
    S1UServer() : display_manager_(nullptr), window_manager_(nullptr), input_manager_(nullptr) {}
    
    bool initialize() {
        set_realtime_scheduling();
        disable_cpu_scaling();
        set_cpu_affinity(0);
        lock_memory();
        prefault_memory(1024 * 1024 * 1024);
        
        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);
        signal(SIGQUIT, signal_handler);
        
        display_manager_ = std::make_unique<DisplayManager>();
        if (!display_manager_->initialize()) {
            std::cerr << "Failed to initialize display manager" << std::endl;
            return false;
        }
        
        window_manager_ = std::make_unique<WindowManager>();
        if (!window_manager_->initialize()) {
            std::cerr << "Failed to initialize window manager" << std::endl;
            return false;
        }
        
        input_manager_ = std::make_unique<InputManager>();
        if (!input_manager_->initialize()) {
            std::cerr << "Failed to initialize input manager" << std::endl;
            return false;
        }
        
        compositor_ = std::make_unique<Compositor>();
        if (!compositor_->initialize()) {
            std::cerr << "Failed to initialize compositor" << std::endl;
            return false;
        }
        
        renderer_ = std::make_unique<Renderer>();
        if (!renderer_->initialize()) {
            std::cerr << "Failed to initialize renderer" << std::endl;
            return false;
        }
        
        protocol_server_ = std::make_unique<ProtocolServer>();
        if (!protocol_server_->initialize()) {
            std::cerr << "Failed to initialize protocol server" << std::endl;
            return false;
        }
        
        return true;
    }
    
    void run() {
        std::cout << "S1U Display Server starting..." << std::endl;
        
        while (running) {
            auto start_time = get_timestamp();
            
            input_manager_->poll_events();
            window_manager_->update();
            compositor_->composite();
            renderer_->render();
            display_manager_->flip();
            protocol_server_->process_messages();
            
            auto end_time = get_timestamp();
            auto frame_time = end_time - start_time;
            
            if (frame_time < 16667) {
                std::this_thread::sleep_for(std::chrono::microseconds(16667 - frame_time));
            }
        }
        
        shutdown();
    }
    
    void shutdown() {
        std::cout << "S1U Display Server shutting down..." << std::endl;
        
        if (protocol_server_) protocol_server_->shutdown();
        if (renderer_) renderer_->shutdown();
        if (compositor_) compositor_->shutdown();
        if (input_manager_) input_manager_->shutdown();
        if (window_manager_) window_manager_->shutdown();
        if (display_manager_) display_manager_->shutdown();
        
        enable_cpu_scaling();
        unlock_memory();
    }
    
private:
    std::unique_ptr<DisplayManager> display_manager_;
    std::unique_ptr<WindowManager> window_manager_;
    std::unique_ptr<InputManager> input_manager_;
    std::unique_ptr<Compositor> compositor_;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<ProtocolServer> protocol_server_;
};

}

int main(int argc, char* argv[]) {
    s1u::S1UServer server;
    
    if (!server.initialize()) {
        std::cerr << "Failed to initialize S1U server" << std::endl;
        return 1;
    }
    
    server.run();
    
    return 0;
}
