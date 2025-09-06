#include "s1u/display_server.hpp"
#include "s1u/renderer.hpp"
#include "s1u/su1_integration.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>

namespace s1u {

DisplayServer::DisplayServer()
    : running_(false)
    , frame_count_(0)
    , current_fps_(0.0)
    , average_frame_time_(0.0)
    , frame_start_time_(std::chrono::high_resolution_clock::now())
    , last_frame_time_(frame_start_time_) {
}

DisplayServer::~DisplayServer() {
    shutdown();
}

bool DisplayServer::initialize(const DisplayServerConfig& config) {
    std::cout << "[S1U] Initializing Display Server..." << std::endl;
    config_ = config;

    // Initialize renderer
    renderer_ = std::make_shared<Renderer>();
    if (!renderer_->initialize(config.width, config.height, config.title)) {
        std::cerr << "[S1U] Failed to initialize renderer" << std::endl;
        return false;
    }

    // Initialize SU1 integration
    su1_integration_ = std::make_shared<SU1Integration>();
    SU1Integration::SU1Config su1_config;
    su1_config.su1_install_path = "/usr/local";
    su1_config.enable_glass_theming = true;
    
    if (!su1_integration_->initialize(su1_config)) {
        std::cout << "[S1U] SU1 integration not available, continuing without it" << std::endl;
    }

    std::cout << "[S1U] Display Server initialized successfully!" << std::endl;
    std::cout << "[S1U] Resolution: " << config.width << "x" << config.height << std::endl;
    
    return true;
}

void DisplayServer::shutdown() {
    running_ = false;
    
    if (main_thread_.joinable()) {
        main_thread_.join();
    }

    if (su1_integration_) {
        su1_integration_->shutdown();
    }

    if (renderer_) {
        renderer_->shutdown();
    }

    std::cout << "[S1U] Display Server shutdown complete" << std::endl;
}

void DisplayServer::run() {
    if (running_) return;
    
    running_ = true;
    main_thread_ = std::thread(&DisplayServer::main_loop, this);
    
    std::cout << "[S1U] Display Server main loop started" << std::endl;
}

void DisplayServer::stop() {
    running_ = false;
    
    if (main_thread_.joinable()) {
        main_thread_.join();
    }
}

void DisplayServer::main_loop() {
    std::cout << "[S1U] Entering main render loop..." << std::endl;
    
    while (running_) {
        frame_start_time_ = std::chrono::high_resolution_clock::now();

        // Render frame
        render_frame();

        // Update frame timing
        update_frame_timing();

        frame_count_++;
        
        // Small delay to prevent excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "[S1U] Main render loop ended" << std::endl;
}

void DisplayServer::render_frame() {
    std::cout << "[DEBUG] Entering render_frame()" << std::endl;
    
    if (!renderer_ || !renderer_->get_window()) {
        std::cerr << "[ERROR] Renderer or window is null!" << std::endl;
        return;
    }

    std::cout << "[DEBUG] About to call renderer_->begin_frame()" << std::endl;
    renderer_->begin_frame();
    std::cout << "[DEBUG] renderer_->begin_frame() completed" << std::endl;

    // Clear with a dark blue background
    std::cout << "[DEBUG] About to call renderer_->clear()" << std::endl;
    renderer_->clear(Color(0.1f, 0.1f, 0.3f, 1.0f));
    std::cout << "[DEBUG] renderer_->clear() completed" << std::endl;

    // Draw a simple red rectangle - this should definitely be visible
    std::cout << "[DEBUG] About to call renderer_->draw_rect()" << std::endl;
    renderer_->draw_rect(Rect(100, 100, 200, 150), Color(1.0f, 0.0f, 0.0f, 1.0f));
    std::cout << "[DEBUG] renderer_->draw_rect() completed" << std::endl;
    
    // Draw a green rectangle outline
    renderer_->draw_rect_outline(Rect(350, 100, 200, 150), Color(0.0f, 1.0f, 0.0f, 1.0f), 3.0f);
    
    // Draw a blue filled circle (as a rectangle for now)
    renderer_->draw_rect(Rect(600, 100, 150, 150), Color(0.0f, 0.0f, 1.0f, 1.0f));
    
    // Draw some text
    renderer_->draw_text("S1U Display Server", Point(120, 120), Color(1.0f, 1.0f, 1.0f, 1.0f), 18.0f);
    renderer_->draw_text("Red Rectangle", Point(120, 270), Color(1.0f, 1.0f, 1.0f, 1.0f), 14.0f);
    renderer_->draw_text("Green Outline", Point(350, 270), Color(1.0f, 1.0f, 1.0f, 1.0f), 14.0f);
    renderer_->draw_text("Blue Circle", Point(600, 270), Color(1.0f, 1.0f, 1.0f, 1.0f), 14.0f);
    
    // Draw status bar at bottom
    renderer_->draw_rect(Rect(0, 550, 900, 50), Color(0.2f, 0.2f, 0.4f, 1.0f));
    renderer_->draw_text("FPS: " + std::to_string(static_cast<int>(current_fps_)), Point(20, 570), Color(1.0f, 1.0f, 1.0f, 1.0f), 16.0f);
    renderer_->draw_text("Frame: " + std::to_string(frame_count_), Point(200, 570), Color(1.0f, 1.0f, 1.0f, 1.0f), 16.0f);
    renderer_->draw_text("S1U Status: Ready", Point(400, 570), Color(0.0f, 1.0f, 0.0f, 1.0f), 16.0f);
    
    // Draw some colored squares in a grid pattern
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            float x = 100 + i * 150;
            float y = 300 + j * 80;
            Color square_color(
                std::fmod(i * 0.2f, 1.0f), 
                std::fmod(j * 0.3f, 1.0f), 
                std::fmod((i + j) * 0.2f, 1.0f), 
                1.0f
            );
            renderer_->draw_rect(Rect(x, y, 120, 60), square_color);
            renderer_->draw_text("Box " + std::to_string(i + j * 5), Point(x + 10, y + 40), Color(1.0f, 1.0f, 1.0f, 1.0f), 12.0f);
        }
    }

    renderer_->end_frame();
    renderer_->present();
}

void DisplayServer::update_frame_timing() {
    auto current_time = std::chrono::high_resolution_clock::now();
    auto frame_duration = std::chrono::duration<double>(current_time - frame_start_time_).count();
    
    // Update frame times history
    frame_times_.push_back(frame_duration);
    if (frame_times_.size() > 60) { // Keep last 60 frames
        frame_times_.erase(frame_times_.begin());
    }
    
    // Calculate current FPS
    if (frame_duration > 0.0) {
        current_fps_ = 1.0 / frame_duration;
    }
    
    // Calculate average frame time
    double total_time = 0.0;
    for (double time : frame_times_) {
        total_time += time;
    }
    average_frame_time_ = total_time / frame_times_.size();
    
    // Log performance every 60 frames
    if (frame_count_ % 60 == 0) {
        std::cout << "[S1U] Frame: " << frame_count_ 
                  << " | FPS: " << std::fixed << std::setprecision(1) << current_fps_
                  << " | Avg Frame Time: " << std::fixed << std::setprecision(3) << (average_frame_time_ * 1000.0) << "ms"
                  << " | Draw Calls: " << renderer_->get_draw_calls() << std::endl;
    }
}

bool DisplayServer::load_su1_application(const std::string& app_path) {
    std::cout << "[S1U] Loading SU1 application: " << app_path << std::endl;
    
    if (su1_integration_) {
        return su1_integration_->load_su1_application(app_path);
    }
    
    std::cerr << "[S1U] SU1 integration not available" << std::endl;
    return false;
}

void DisplayServer::unload_su1_application(const std::string& app_name) {
    if (su1_integration_) {
        su1_integration_->unload_su1_application(app_name);
        std::cout << "[S1U] Unloaded SU1 application: " << app_name << std::endl;
    }
}

std::vector<std::string> DisplayServer::get_loaded_su1_apps() const {
    if (su1_integration_) {
        auto apps = su1_integration_->get_loaded_applications();
        std::vector<std::string> app_names;
        for (const auto& app : apps) {
            app_names.push_back(app.name);
        }
        return app_names;
    }
    return {};
}

void DisplayServer::update_su1_applications() {
    if (su1_integration_) {
        su1_integration_->update_su1_applications();
    }
}

// The missing methods that were causing build errors
double DisplayServer::get_current_fps() const {
    return current_fps_;
}

uint64_t DisplayServer::get_frame_count() const {
    return frame_count_;
}

double DisplayServer::get_average_frame_time() const {
    return average_frame_time_;
}

} // namespace s1u

        renderer_->shutdown();
    }

    std::cout << "[S1U] Display Server shutdown complete" << std::endl;
}

void DisplayServer::run() {
    if (running_) return;
    
    running_ = true;
    main_thread_ = std::thread(&DisplayServer::main_loop, this);
    
    std::cout << "[S1U] Display Server main loop started" << std::endl;
}

void DisplayServer::stop() {
    running_ = false;
    
    if (main_thread_.joinable()) {
        main_thread_.join();
    }
}

void DisplayServer::main_loop() {
    std::cout << "[S1U] Entering main render loop..." << std::endl;
    
    while (running_) {
        frame_start_time_ = std::chrono::high_resolution_clock::now();

        // Render frame
        render_frame();

        // Update frame timing
        update_frame_timing();

        frame_count_++;
        
        // Small delay to prevent excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "[S1U] Main render loop ended" << std::endl;
}

void DisplayServer::render_frame() {
    std::cout << "[DEBUG] Entering render_frame()" << std::endl;
    
    if (!renderer_ || !renderer_->get_window()) {
        std::cerr << "[ERROR] Renderer or window is null!" << std::endl;
        return;
    }

    std::cout << "[DEBUG] About to call renderer_->begin_frame()" << std::endl;
    renderer_->begin_frame();
    std::cout << "[DEBUG] renderer_->begin_frame() completed" << std::endl;

    // Clear with a dark blue background
    std::cout << "[DEBUG] About to call renderer_->clear()" << std::endl;
    renderer_->clear(Color(0.1f, 0.1f, 0.3f, 1.0f));
    std::cout << "[DEBUG] renderer_->clear() completed" << std::endl;

    // Draw a simple red rectangle - this should definitely be visible
    std::cout << "[DEBUG] About to call renderer_->draw_rect()" << std::endl;
    renderer_->draw_rect(Rect(100, 100, 200, 150), Color(1.0f, 0.0f, 0.0f, 1.0f));
    std::cout << "[DEBUG] renderer_->draw_rect() completed" << std::endl;
    
    // Draw a green rectangle outline
    renderer_->draw_rect_outline(Rect(350, 100, 200, 150), Color(0.0f, 1.0f, 0.0f, 1.0f), 3.0f);
    
    // Draw a blue filled circle (as a rectangle for now)
    renderer_->draw_rect(Rect(600, 100, 150, 150), Color(0.0f, 0.0f, 1.0f, 1.0f));
    
    // Draw some text
    renderer_->draw_text("S1U Display Server", Point(120, 120), Color(1.0f, 1.0f, 1.0f, 1.0f), 18.0f);
    renderer_->draw_text("Red Rectangle", Point(120, 270), Color(1.0f, 1.0f, 1.0f, 1.0f), 14.0f);
    renderer_->draw_text("Green Outline", Point(350, 270), Color(1.0f, 1.0f, 1.0f, 1.0f), 14.0f);
    renderer_->draw_text("Blue Circle", Point(600, 270), Color(1.0f, 1.0f, 1.0f, 1.0f), 14.0f);
    
    // Draw status bar at bottom
    renderer_->draw_rect(Rect(0, 550, 900, 50), Color(0.2f, 0.2f, 0.4f, 1.0f));
    renderer_->draw_text("FPS: " + std::to_string(static_cast<int>(current_fps_)), Point(20, 570), Color(1.0f, 1.0f, 1.0f, 1.0f), 16.0f);
    renderer_->draw_text("Frame: " + std::to_string(frame_count_), Point(200, 570), Color(1.0f, 1.0f, 1.0f, 1.0f), 16.0f);
    renderer_->draw_text("S1U Status: Ready", Point(400, 570), Color(0.0f, 1.0f, 0.0f, 1.0f), 16.0f);
    
    // Draw some colored squares in a grid pattern
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            float x = 100 + i * 150;
            float y = 300 + j * 80;
            Color square_color(
                std::fmod(i * 0.2f, 1.0f), 
                std::fmod(j * 0.3f, 1.0f), 
                std::fmod((i + j) * 0.2f, 1.0f), 
                1.0f
            );
            renderer_->draw_rect(Rect(x, y, 120, 60), square_color);
            renderer_->draw_text("Box " + std::to_string(i + j * 5), Point(x + 10, y + 40), Color(1.0f, 1.0f, 1.0f, 1.0f), 12.0f);
        }
    }

    renderer_->end_frame();
    renderer_->present();
}

void DisplayServer::update_frame_timing() {
    auto current_time = std::chrono::high_resolution_clock::now();
    auto frame_duration = std::chrono::duration<double>(current_time - frame_start_time_).count();
    
    // Update frame times history
    frame_times_.push_back(frame_duration);
    if (frame_times_.size() > 60) { // Keep last 60 frames
        frame_times_.erase(frame_times_.begin());
    }
    
    // Calculate current FPS
    if (frame_duration > 0.0) {
        current_fps_ = 1.0 / frame_duration;
    }
    
    // Calculate average frame time
    double total_time = 0.0;
    for (double time : frame_times_) {
        total_time += time;
    }
    average_frame_time_ = total_time / frame_times_.size();
    
    // Log performance every 60 frames
    if (frame_count_ % 60 == 0) {
        std::cout << "[S1U] Frame: " << frame_count_ 
                  << " | FPS: " << std::fixed << std::setprecision(1) << current_fps_
                  << " | Avg Frame Time: " << std::fixed << std::setprecision(3) << (average_frame_time_ * 1000.0) << "ms"
                  << " | Draw Calls: " << renderer_->get_draw_calls() << std::endl;
    }
}

bool DisplayServer::load_su1_application(const std::string& app_path) {
    std::cout << "[S1U] Loading SU1 application: " << app_path << std::endl;
    
    if (su1_integration_) {
        return su1_integration_->load_su1_application(app_path);
    }
    
    std::cerr << "[S1U] SU1 integration not available" << std::endl;
    return false;
}

void DisplayServer::unload_su1_application(const std::string& app_name) {
    if (su1_integration_) {
        su1_integration_->unload_su1_application(app_name);
        std::cout << "[S1U] Unloaded SU1 application: " << app_name << std::endl;
    }
}

std::vector<std::string> DisplayServer::get_loaded_su1_apps() const {
    if (su1_integration_) {
        auto apps = su1_integration_->get_loaded_applications();
        std::vector<std::string> app_names;
        for (const auto& app : apps) {
            app_names.push_back(app.name);
        }
        return app_names;
    }
    return {};
}

void DisplayServer::update_su1_applications() {
    if (su1_integration_) {
        su1_integration_->update_su1_applications();
    }
}

// The missing methods that were causing build errors
double DisplayServer::get_current_fps() const {
    return current_fps_;
}

uint64_t DisplayServer::get_frame_count() const {
    return frame_count_;
}

double DisplayServer::get_average_frame_time() const {
    return average_frame_time_;
}

} // namespace s1u

        renderer_->shutdown();
    }

    std::cout << "[S1U] Display Server shutdown complete" << std::endl;
}

void DisplayServer::run() {
    if (running_) return;
    
    running_ = true;
    main_thread_ = std::thread(&DisplayServer::main_loop, this);
    
    std::cout << "[S1U] Display Server main loop started" << std::endl;
}

void DisplayServer::stop() {
    running_ = false;
    
    if (main_thread_.joinable()) {
        main_thread_.join();
    }
}

void DisplayServer::main_loop() {
    std::cout << "[S1U] Entering main render loop..." << std::endl;
    
    while (running_) {
        frame_start_time_ = std::chrono::high_resolution_clock::now();

        // Render frame
        render_frame();

        // Update frame timing
        update_frame_timing();

        frame_count_++;
        
        // Small delay to prevent excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    std::cout << "[S1U] Main render loop ended" << std::endl;
}

void DisplayServer::render_frame() {
    std::cout << "[DEBUG] Entering render_frame()" << std::endl;
    
    if (!renderer_ || !renderer_->get_window()) {
        std::cerr << "[ERROR] Renderer or window is null!" << std::endl;
        return;
    }

    std::cout << "[DEBUG] About to call renderer_->begin_frame()" << std::endl;
    renderer_->begin_frame();
    std::cout << "[DEBUG] renderer_->begin_frame() completed" << std::endl;

    // Clear with a dark blue background
    std::cout << "[DEBUG] About to call renderer_->clear()" << std::endl;
    renderer_->clear(Color(0.1f, 0.1f, 0.3f, 1.0f));
    std::cout << "[DEBUG] renderer_->clear() completed" << std::endl;

    // Draw a simple red rectangle - this should definitely be visible
    std::cout << "[DEBUG] About to call renderer_->draw_rect()" << std::endl;
    renderer_->draw_rect(Rect(100, 100, 200, 150), Color(1.0f, 0.0f, 0.0f, 1.0f));
    std::cout << "[DEBUG] renderer_->draw_rect() completed" << std::endl;
    
    // Draw a green rectangle outline
    renderer_->draw_rect_outline(Rect(350, 100, 200, 150), Color(0.0f, 1.0f, 0.0f, 1.0f), 3.0f);
    
    // Draw a blue filled circle (as a rectangle for now)
    renderer_->draw_rect(Rect(600, 100, 150, 150), Color(0.0f, 0.0f, 1.0f, 1.0f));
    
    // Draw some text
    renderer_->draw_text("S1U Display Server", Point(120, 120), Color(1.0f, 1.0f, 1.0f, 1.0f), 18.0f);
    renderer_->draw_text("Red Rectangle", Point(120, 270), Color(1.0f, 1.0f, 1.0f, 1.0f), 14.0f);
    renderer_->draw_text("Green Outline", Point(350, 270), Color(1.0f, 1.0f, 1.0f, 1.0f), 14.0f);
    renderer_->draw_text("Blue Circle", Point(600, 270), Color(1.0f, 1.0f, 1.0f, 1.0f), 14.0f);
    
    // Draw status bar at bottom
    renderer_->draw_rect(Rect(0, 550, 900, 50), Color(0.2f, 0.2f, 0.4f, 1.0f));
    renderer_->draw_text("FPS: " + std::to_string(static_cast<int>(current_fps_)), Point(20, 570), Color(1.0f, 1.0f, 1.0f, 1.0f), 16.0f);
    renderer_->draw_text("Frame: " + std::to_string(frame_count_), Point(200, 570), Color(1.0f, 1.0f, 1.0f, 1.0f), 16.0f);
    renderer_->draw_text("S1U Status: Ready", Point(400, 570), Color(0.0f, 1.0f, 0.0f, 1.0f), 16.0f);
    
    // Draw some colored squares in a grid pattern
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            float x = 100 + i * 150;
            float y = 300 + j * 80;
            Color square_color(
                std::fmod(i * 0.2f, 1.0f), 
                std::fmod(j * 0.3f, 1.0f), 
                std::fmod((i + j) * 0.2f, 1.0f), 
                1.0f
            );
            renderer_->draw_rect(Rect(x, y, 120, 60), square_color);
            renderer_->draw_text("Box " + std::to_string(i + j * 5), Point(x + 10, y + 40), Color(1.0f, 1.0f, 1.0f, 1.0f), 12.0f);
        }
    }

    renderer_->end_frame();
    renderer_->present();
}

void DisplayServer::update_frame_timing() {
    auto current_time = std::chrono::high_resolution_clock::now();
    auto frame_duration = std::chrono::duration<double>(current_time - frame_start_time_).count();
    
    // Update frame times history
    frame_times_.push_back(frame_duration);
    if (frame_times_.size() > 60) { // Keep last 60 frames
        frame_times_.erase(frame_times_.begin());
    }
    
    // Calculate current FPS
    if (frame_duration > 0.0) {
        current_fps_ = 1.0 / frame_duration;
    }
    
    // Calculate average frame time
    double total_time = 0.0;
    for (double time : frame_times_) {
        total_time += time;
    }
    average_frame_time_ = total_time / frame_times_.size();
    
    // Log performance every 60 frames
    if (frame_count_ % 60 == 0) {
        std::cout << "[S1U] Frame: " << frame_count_ 
                  << " | FPS: " << std::fixed << std::setprecision(1) << current_fps_
                  << " | Avg Frame Time: " << std::fixed << std::setprecision(3) << (average_frame_time_ * 1000.0) << "ms"
                  << " | Draw Calls: " << renderer_->get_draw_calls() << std::endl;
    }
}

bool DisplayServer::load_su1_application(const std::string& app_path) {
    std::cout << "[S1U] Loading SU1 application: " << app_path << std::endl;
    
    if (su1_integration_) {
        return su1_integration_->load_su1_application(app_path);
    }
    
    std::cerr << "[S1U] SU1 integration not available" << std::endl;
    return false;
}

void DisplayServer::unload_su1_application(const std::string& app_name) {
    if (su1_integration_) {
        su1_integration_->unload_su1_application(app_name);
        std::cout << "[S1U] Unloaded SU1 application: " << app_name << std::endl;
    }
}

std::vector<std::string> DisplayServer::get_loaded_su1_apps() const {
    if (su1_integration_) {
        auto apps = su1_integration_->get_loaded_applications();
        std::vector<std::string> app_names;
        for (const auto& app : apps) {
            app_names.push_back(app.name);
        }
        return app_names;
    }
    return {};
}

void DisplayServer::update_su1_applications() {
    if (su1_integration_) {
        su1_integration_->update_su1_applications();
    }
}

// The missing methods that were causing build errors
double DisplayServer::get_current_fps() const {
    return current_fps_;
}

uint64_t DisplayServer::get_frame_count() const {
    return frame_count_;
}

double DisplayServer::get_average_frame_time() const {
    return average_frame_time_;
}

} // namespace s1u
