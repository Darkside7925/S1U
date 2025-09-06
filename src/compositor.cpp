#include "s1u/compositor.hpp"
#include "s1u/window_manager.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

Compositor::Compositor()
    : initialized_(false)
    , su1_composition_mode_(false)
    , frame_count_(0)
    , current_fps_(0.0)
    , average_frame_time_(0.0) {
    
    // Initialize frame timing
    last_frame_time_ = std::chrono::high_resolution_clock::now();
    frame_start_time_ = last_frame_time_;
}

Compositor::~Compositor() {
    shutdown();
}

bool Compositor::initialize(std::shared_ptr<Renderer> renderer, const CompositorSettings& settings) {
    std::cout << "[S1U] Initializing Compositor..." << std::endl;
    
    if (!renderer) {
        std::cerr << "[S1U] Cannot initialize compositor: renderer is null" << std::endl;
        return false;
    }
    
    renderer_ = renderer;
    settings_ = settings;
    
    try {
        // Setup render targets
        setup_render_targets();
        
        // Initialize shaders
        initialize_shaders();
        
        initialized_ = true;
        std::cout << "[S1U] Compositor initialized successfully!" << std::endl;
        std::cout << "[S1U] Vsync: " << (settings.enable_vsync ? "Enabled" : "Disabled") << std::endl;
        std::cout << "[S1U] Target FPS: " << settings.max_fps << std::endl;
        std::cout << "[S1U] MSAA: " << settings.msaa_samples << "x" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[S1U] Compositor initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void Compositor::shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "[S1U] Shutting down Compositor..." << std::endl;
    
    // Cleanup render targets
    if (main_target_.fbo) glDeleteFramebuffers(1, &main_target_.fbo);
    if (main_target_.texture) glDeleteTextures(1, &main_target_.texture);
    if (main_target_.depth_buffer) glDeleteRenderbuffers(1, &main_target_.depth_buffer);
    
    if (effect_target_.fbo) glDeleteFramebuffers(1, &effect_target_.fbo);
    if (effect_target_.texture) glDeleteTextures(1, &effect_target_.texture);
    if (effect_target_.depth_buffer) glDeleteRenderbuffers(1, &effect_target_.depth_buffer);
    
    if (blur_target_.fbo) glDeleteFramebuffers(1, &blur_target_.fbo);
    if (blur_target_.texture) glDeleteTextures(1, &blur_target_.texture);
    if (blur_target_.depth_buffer) glDeleteRenderbuffers(1, &blur_target_.depth_buffer);
    
    // Cleanup shaders
    if (composition_shader_) glDeleteProgram(composition_shader_);
    if (effect_shader_) glDeleteProgram(effect_shader_);
    if (blur_shader_) glDeleteProgram(blur_shader_);
    if (shadow_shader_) glDeleteProgram(shadow_shader_);
    
    initialized_ = false;
}

void Compositor::set_settings(const CompositorSettings& settings) {
    settings_ = settings;
    
    // Apply vsync setting
    if (renderer_) {
        renderer_->set_vsync(settings.enable_vsync);
    }
}

const CompositorSettings& Compositor::get_settings() const {
    return settings_;
}

void Compositor::add_window(std::shared_ptr<Window> window) {
    if (window) {
        windows_.push_back(window);
    }
}

void Compositor::remove_window(std::shared_ptr<Window> window) {
    auto it = std::find(windows_.begin(), windows_.end(), window);
    if (it != windows_.end()) {
        windows_.erase(it);
    }
}

void Compositor::update_window(std::shared_ptr<Window> window) {
    // Update window state if needed
    // This is called when window properties change
}

void Compositor::render_window(std::shared_ptr<Window> window) {
    if (!window || !renderer_) return;
    
    // Render individual window
    window->render(renderer_);
}

void Compositor::begin_composition() {
    if (!initialized_ || !renderer_) return;
    
    frame_start_time_ = std::chrono::high_resolution_clock::now();
    
    // Bind main render target
    glBindFramebuffer(GL_FRAMEBUFFER, main_target_.fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Compositor::compose_frame() {
    if (!initialized_ || !renderer_) return;
    
    // Render background
    render_background();
    
    // Render all windows
    render_windows();
    
    // Apply post-processing effects
    apply_post_effects();
}

void Compositor::end_composition() {
    if (!initialized_ || !renderer_) return;
    
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Final composition
    final_composition();
}

void Compositor::present_frame() {
    if (!initialized_ || !renderer_) return;
    
    // Present the composed frame
    renderer_->present();
    
    // Update frame timing
    update_frame_timing();
    
    frame_count_++;
}

void Compositor::enable_effect(CompositorEffect effect, bool enable) {
    enabled_effects_[effect] = enable;
}

void Compositor::set_effect_parameters(CompositorEffect effect, const std::vector<float>& parameters) {
    effect_parameters_[effect] = parameters;
}

bool Compositor::is_effect_enabled(CompositorEffect effect) const {
    auto it = enabled_effects_.find(effect);
    return (it != enabled_effects_.end()) ? it->second : false;
}

void Compositor::set_target_fps(uint32_t fps) {
    settings_.max_fps = fps;
}

void Compositor::set_vsync(bool enabled) {
    settings_.enable_vsync = enabled;
    if (renderer_) {
        renderer_->set_vsync(enabled);
    }
}

void Compositor::set_triple_buffering(bool enabled) {
    settings_.enable_triple_buffering = enabled;
}

void Compositor::set_adaptive_vsync(bool enabled) {
    settings_.enable_adaptive_vsync = enabled;
}

void Compositor::set_su1_composition_mode(bool enabled) {
    su1_composition_mode_ = enabled;
    std::cout << "[S1U] SU1 composition mode: " << (enabled ? "Enabled" : "Disabled") << std::endl;
}

void Compositor::render_su1_applications() {
    if (!su1_composition_mode_) return;
    
    // Render SU1 applications with special effects
    for (auto& [name, window] : su1_windows_) {
        if (window) {
            render_su1_window(window);
        }
    }
}

double Compositor::get_current_fps() const {
    return current_fps_;
}

uint64_t Compositor::get_frame_count() const {
    return frame_count_;
}

double Compositor::get_average_frame_time() const {
    return average_frame_time_;
}

uint32_t Compositor::get_draw_calls() const {
    return renderer_ ? renderer_->get_draw_calls() : 0;
}

uint32_t Compositor::get_triangle_count() const {
    // Simple triangle count estimation
    return get_draw_calls() * 2; // Assuming 2 triangles per draw call on average
}

// Private methods
void Compositor::setup_render_targets() {
    // Get window size from renderer
    Size window_size = renderer_->get_window_size();
    uint32_t width = static_cast<uint32_t>(window_size.width);
    uint32_t height = static_cast<uint32_t>(window_size.height);
    
    // Create main render target
    glGenFramebuffers(1, &main_target_.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, main_target_.fbo);
    
    // Create color texture
    glGenTextures(1, &main_target_.texture);
    glBindTexture(GL_TEXTURE_2D, main_target_.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, main_target_.texture, 0);
    
    // Create depth buffer
    glGenRenderbuffers(1, &main_target_.depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, main_target_.depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, main_target_.depth_buffer);
    
    main_target_.width = width;
    main_target_.height = height;
    
    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[S1U] Main framebuffer is not complete!" << std::endl;
        return;
    }
    
    // Create effect render target
    glGenFramebuffers(1, &effect_target_.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, effect_target_.fbo);
    
    glGenTextures(1, &effect_target_.texture);
    glBindTexture(GL_TEXTURE_2D, effect_target_.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, effect_target_.texture, 0);
    
    glGenRenderbuffers(1, &effect_target_.depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, effect_target_.depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, effect_target_.depth_buffer);
    
    effect_target_.width = width;
    effect_target_.height = height;
    
    // Create blur render target
    glGenFramebuffers(1, &blur_target_.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, blur_target_.fbo);
    
    glGenTextures(1, &blur_target_.texture);
    glBindTexture(GL_TEXTURE_2D, blur_target_.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur_target_.texture, 0);
    
    glGenRenderbuffers(1, &blur_target_.depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, blur_target_.depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, blur_target_.depth_buffer);
    
    blur_target_.width = width;
    blur_target_.height = height;
    
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    std::cout << "[S1U] Render targets setup complete" << std::endl;
}

void Compositor::initialize_shaders() {
    // Create basic composition shader
    const char* vertex_source = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    const char* fragment_source = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        
        uniform sampler2D uTexture;
        
        void main() {
            FragColor = texture(uTexture, TexCoord);
        }
    )";
    
    composition_shader_ = create_shader_program(vertex_source, fragment_source);
    
    // Create effect shader
    effect_shader_ = create_shader_program(vertex_source, fragment_source);
    
    // Create blur shader
    blur_shader_ = create_shader_program(vertex_source, fragment_source);
    
    // Create shadow shader
    shadow_shader_ = create_shader_program(vertex_source, fragment_source);
    
    std::cout << "[S1U] Shaders initialized" << std::endl;
}

uint32_t Compositor::create_shader_program(const char* vertex_source, const char* fragment_source) {
    // Compile vertex shader
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
    glCompileShader(vertex_shader);
    
    // Check for errors
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Vertex shader compilation failed: " << info_log << std::endl;
        return 0;
    }
    
    // Compile fragment shader
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
    glCompileShader(fragment_shader);
    
    // Check for errors
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Fragment shader compilation failed: " << info_log << std::endl;
        return 0;
    }
    
    // Link program
    uint32_t program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    // Check for errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        std::cerr << "[S1U] Shader program linking failed: " << info_log << std::endl;
        return 0;
    }
    
    // Cleanup
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return program;
}

void Compositor::render_background() {
    if (!renderer_) return;
    
    // Render desktop background
    Rect bg_rect(0, 0, 1920, 1080);
    Color bg_color(0.1f, 0.1f, 0.15f, 1.0f);
    renderer_->draw_rect(bg_rect, bg_color);
}

void Compositor::render_windows() {
    if (!renderer_) return;
    
    // Render all windows
    for (auto& window : windows_) {
        if (window && window->is_visible()) {
            window->render(renderer_);
        }
    }
}

void Compositor::apply_post_effects() {
    if (!renderer_) return;
    
    // Apply blur effect if enabled
    if (is_effect_enabled(CompositorEffect::Blur)) {
        render_blur_effect();
    }
    
    // Apply glow effect if enabled
    if (is_effect_enabled(CompositorEffect::Glow)) {
        render_glow_effect();
    }
    
    // Apply shadow effect if enabled
    if (is_effect_enabled(CompositorEffect::Shadow)) {
        render_shadow_effect();
    }
    
    // Apply liquid glass effect if enabled
    if (is_effect_enabled(CompositorEffect::Liquid)) {
        render_liquid_glass_effect();
    }
}

void Compositor::final_composition() {
    // Copy the composed frame to the screen
    // This is a simplified implementation
}

void Compositor::render_blur_effect() {
    // Render blur effect using the blur render target
    // This is a simplified implementation
}

void Compositor::render_glow_effect() {
    // Render glow effect
    // This is a simplified implementation
}

void Compositor::render_shadow_effect() {
    // Render shadow effect
    // This is a simplified implementation
}

void Compositor::render_transparency_effect() {
    // Render transparency effect
    // This is a simplified implementation
}

void Compositor::render_reflection_effect() {
    // Render reflection effect
    // This is a simplified implementation
}

void Compositor::render_liquid_glass_effect() {
    // Render liquid glass effect
    // This is a simplified implementation
}

void Compositor::update_frame_timing() {
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
}

void Compositor::limit_frame_rate() {
    if (settings_.enable_frame_limiting) {
        double target_frame_time = 1.0 / settings_.max_fps;
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(current_time - frame_start_time_).count();
        
        if (elapsed < target_frame_time) {
            double sleep_time = target_frame_time - elapsed;
            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>(sleep_time * 1000000)));
        }
    }
}

void Compositor::sync_with_gpu() {
    if (settings_.enable_gpu_sync) {
        // Wait for GPU to finish rendering
        glFinish();
    }
}

void Compositor::render_su1_window(std::shared_ptr<Window> window) {
    if (!window || !renderer_) return;
    
    // Render SU1 window with special effects
    window->render(renderer_);
    
    // Apply SU1-specific effects
    apply_su1_effects(window);
}

void Compositor::apply_su1_effects(std::shared_ptr<Window> window) {
    // Apply special effects for SU1 applications
    // This could include quantum effects, neural patterns, etc.
    
    // For now, just add a subtle glow effect
    if (is_effect_enabled(CompositorEffect::Glow)) {
        // Apply glow effect to SU1 windows
    }
}

} // namespace s1u

#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

Compositor::Compositor()
    : initialized_(false)
    , su1_composition_mode_(false)
    , frame_count_(0)
    , current_fps_(0.0)
    , average_frame_time_(0.0) {
    
    // Initialize frame timing
    last_frame_time_ = std::chrono::high_resolution_clock::now();
    frame_start_time_ = last_frame_time_;
}

Compositor::~Compositor() {
    shutdown();
}

bool Compositor::initialize(std::shared_ptr<Renderer> renderer, const CompositorSettings& settings) {
    std::cout << "[S1U] Initializing Compositor..." << std::endl;
    
    if (!renderer) {
        std::cerr << "[S1U] Cannot initialize compositor: renderer is null" << std::endl;
        return false;
    }
    
    renderer_ = renderer;
    settings_ = settings;
    
    try {
        // Setup render targets
        setup_render_targets();
        
        // Initialize shaders
        initialize_shaders();
        
        initialized_ = true;
        std::cout << "[S1U] Compositor initialized successfully!" << std::endl;
        std::cout << "[S1U] Vsync: " << (settings.enable_vsync ? "Enabled" : "Disabled") << std::endl;
        std::cout << "[S1U] Target FPS: " << settings.max_fps << std::endl;
        std::cout << "[S1U] MSAA: " << settings.msaa_samples << "x" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[S1U] Compositor initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void Compositor::shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "[S1U] Shutting down Compositor..." << std::endl;
    
    // Cleanup render targets
    if (main_target_.fbo) glDeleteFramebuffers(1, &main_target_.fbo);
    if (main_target_.texture) glDeleteTextures(1, &main_target_.texture);
    if (main_target_.depth_buffer) glDeleteRenderbuffers(1, &main_target_.depth_buffer);
    
    if (effect_target_.fbo) glDeleteFramebuffers(1, &effect_target_.fbo);
    if (effect_target_.texture) glDeleteTextures(1, &effect_target_.texture);
    if (effect_target_.depth_buffer) glDeleteRenderbuffers(1, &effect_target_.depth_buffer);
    
    if (blur_target_.fbo) glDeleteFramebuffers(1, &blur_target_.fbo);
    if (blur_target_.texture) glDeleteTextures(1, &blur_target_.texture);
    if (blur_target_.depth_buffer) glDeleteRenderbuffers(1, &blur_target_.depth_buffer);
    
    // Cleanup shaders
    if (composition_shader_) glDeleteProgram(composition_shader_);
    if (effect_shader_) glDeleteProgram(effect_shader_);
    if (blur_shader_) glDeleteProgram(blur_shader_);
    if (shadow_shader_) glDeleteProgram(shadow_shader_);
    
    initialized_ = false;
}

void Compositor::set_settings(const CompositorSettings& settings) {
    settings_ = settings;
    
    // Apply vsync setting
    if (renderer_) {
        renderer_->set_vsync(settings.enable_vsync);
    }
}

const CompositorSettings& Compositor::get_settings() const {
    return settings_;
}

void Compositor::add_window(std::shared_ptr<Window> window) {
    if (window) {
        windows_.push_back(window);
    }
}

void Compositor::remove_window(std::shared_ptr<Window> window) {
    auto it = std::find(windows_.begin(), windows_.end(), window);
    if (it != windows_.end()) {
        windows_.erase(it);
    }
}

void Compositor::update_window(std::shared_ptr<Window> window) {
    // Update window state if needed
    // This is called when window properties change
}

void Compositor::render_window(std::shared_ptr<Window> window) {
    if (!window || !renderer_) return;
    
    // Render individual window
    window->render(renderer_);
}

void Compositor::begin_composition() {
    if (!initialized_ || !renderer_) return;
    
    frame_start_time_ = std::chrono::high_resolution_clock::now();
    
    // Bind main render target
    glBindFramebuffer(GL_FRAMEBUFFER, main_target_.fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Compositor::compose_frame() {
    if (!initialized_ || !renderer_) return;
    
    // Render background
    render_background();
    
    // Render all windows
    render_windows();
    
    // Apply post-processing effects
    apply_post_effects();
}

void Compositor::end_composition() {
    if (!initialized_ || !renderer_) return;
    
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Final composition
    final_composition();
}

void Compositor::present_frame() {
    if (!initialized_ || !renderer_) return;
    
    // Present the composed frame
    renderer_->present();
    
    // Update frame timing
    update_frame_timing();
    
    frame_count_++;
}

void Compositor::enable_effect(CompositorEffect effect, bool enable) {
    enabled_effects_[effect] = enable;
}

void Compositor::set_effect_parameters(CompositorEffect effect, const std::vector<float>& parameters) {
    effect_parameters_[effect] = parameters;
}

bool Compositor::is_effect_enabled(CompositorEffect effect) const {
    auto it = enabled_effects_.find(effect);
    return (it != enabled_effects_.end()) ? it->second : false;
}

void Compositor::set_target_fps(uint32_t fps) {
    settings_.max_fps = fps;
}

void Compositor::set_vsync(bool enabled) {
    settings_.enable_vsync = enabled;
    if (renderer_) {
        renderer_->set_vsync(enabled);
    }
}

void Compositor::set_triple_buffering(bool enabled) {
    settings_.enable_triple_buffering = enabled;
}

void Compositor::set_adaptive_vsync(bool enabled) {
    settings_.enable_adaptive_vsync = enabled;
}

void Compositor::set_su1_composition_mode(bool enabled) {
    su1_composition_mode_ = enabled;
    std::cout << "[S1U] SU1 composition mode: " << (enabled ? "Enabled" : "Disabled") << std::endl;
}

void Compositor::render_su1_applications() {
    if (!su1_composition_mode_) return;
    
    // Render SU1 applications with special effects
    for (auto& [name, window] : su1_windows_) {
        if (window) {
            render_su1_window(window);
        }
    }
}

double Compositor::get_current_fps() const {
    return current_fps_;
}

uint64_t Compositor::get_frame_count() const {
    return frame_count_;
}

double Compositor::get_average_frame_time() const {
    return average_frame_time_;
}

uint32_t Compositor::get_draw_calls() const {
    return renderer_ ? renderer_->get_draw_calls() : 0;
}

uint32_t Compositor::get_triangle_count() const {
    // Simple triangle count estimation
    return get_draw_calls() * 2; // Assuming 2 triangles per draw call on average
}

// Private methods
void Compositor::setup_render_targets() {
    // Get window size from renderer
    Size window_size = renderer_->get_window_size();
    uint32_t width = static_cast<uint32_t>(window_size.width);
    uint32_t height = static_cast<uint32_t>(window_size.height);
    
    // Create main render target
    glGenFramebuffers(1, &main_target_.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, main_target_.fbo);
    
    // Create color texture
    glGenTextures(1, &main_target_.texture);
    glBindTexture(GL_TEXTURE_2D, main_target_.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, main_target_.texture, 0);
    
    // Create depth buffer
    glGenRenderbuffers(1, &main_target_.depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, main_target_.depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, main_target_.depth_buffer);
    
    main_target_.width = width;
    main_target_.height = height;
    
    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[S1U] Main framebuffer is not complete!" << std::endl;
        return;
    }
    
    // Create effect render target
    glGenFramebuffers(1, &effect_target_.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, effect_target_.fbo);
    
    glGenTextures(1, &effect_target_.texture);
    glBindTexture(GL_TEXTURE_2D, effect_target_.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, effect_target_.texture, 0);
    
    glGenRenderbuffers(1, &effect_target_.depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, effect_target_.depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, effect_target_.depth_buffer);
    
    effect_target_.width = width;
    effect_target_.height = height;
    
    // Create blur render target
    glGenFramebuffers(1, &blur_target_.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, blur_target_.fbo);
    
    glGenTextures(1, &blur_target_.texture);
    glBindTexture(GL_TEXTURE_2D, blur_target_.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur_target_.texture, 0);
    
    glGenRenderbuffers(1, &blur_target_.depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, blur_target_.depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, blur_target_.depth_buffer);
    
    blur_target_.width = width;
    blur_target_.height = height;
    
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    std::cout << "[S1U] Render targets setup complete" << std::endl;
}

void Compositor::initialize_shaders() {
    // Create basic composition shader
    const char* vertex_source = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    const char* fragment_source = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        
        uniform sampler2D uTexture;
        
        void main() {
            FragColor = texture(uTexture, TexCoord);
        }
    )";
    
    composition_shader_ = create_shader_program(vertex_source, fragment_source);
    
    // Create effect shader
    effect_shader_ = create_shader_program(vertex_source, fragment_source);
    
    // Create blur shader
    blur_shader_ = create_shader_program(vertex_source, fragment_source);
    
    // Create shadow shader
    shadow_shader_ = create_shader_program(vertex_source, fragment_source);
    
    std::cout << "[S1U] Shaders initialized" << std::endl;
}

uint32_t Compositor::create_shader_program(const char* vertex_source, const char* fragment_source) {
    // Compile vertex shader
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
    glCompileShader(vertex_shader);
    
    // Check for errors
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Vertex shader compilation failed: " << info_log << std::endl;
        return 0;
    }
    
    // Compile fragment shader
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
    glCompileShader(fragment_shader);
    
    // Check for errors
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Fragment shader compilation failed: " << info_log << std::endl;
        return 0;
    }
    
    // Link program
    uint32_t program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    // Check for errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        std::cerr << "[S1U] Shader program linking failed: " << info_log << std::endl;
        return 0;
    }
    
    // Cleanup
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return program;
}

void Compositor::render_background() {
    if (!renderer_) return;
    
    // Render desktop background
    Rect bg_rect(0, 0, 1920, 1080);
    Color bg_color(0.1f, 0.1f, 0.15f, 1.0f);
    renderer_->draw_rect(bg_rect, bg_color);
}

void Compositor::render_windows() {
    if (!renderer_) return;
    
    // Render all windows
    for (auto& window : windows_) {
        if (window && window->is_visible()) {
            window->render(renderer_);
        }
    }
}

void Compositor::apply_post_effects() {
    if (!renderer_) return;
    
    // Apply blur effect if enabled
    if (is_effect_enabled(CompositorEffect::Blur)) {
        render_blur_effect();
    }
    
    // Apply glow effect if enabled
    if (is_effect_enabled(CompositorEffect::Glow)) {
        render_glow_effect();
    }
    
    // Apply shadow effect if enabled
    if (is_effect_enabled(CompositorEffect::Shadow)) {
        render_shadow_effect();
    }
    
    // Apply liquid glass effect if enabled
    if (is_effect_enabled(CompositorEffect::Liquid)) {
        render_liquid_glass_effect();
    }
}

void Compositor::final_composition() {
    // Copy the composed frame to the screen
    // This is a simplified implementation
}

void Compositor::render_blur_effect() {
    // Render blur effect using the blur render target
    // This is a simplified implementation
}

void Compositor::render_glow_effect() {
    // Render glow effect
    // This is a simplified implementation
}

void Compositor::render_shadow_effect() {
    // Render shadow effect
    // This is a simplified implementation
}

void Compositor::render_transparency_effect() {
    // Render transparency effect
    // This is a simplified implementation
}

void Compositor::render_reflection_effect() {
    // Render reflection effect
    // This is a simplified implementation
}

void Compositor::render_liquid_glass_effect() {
    // Render liquid glass effect
    // This is a simplified implementation
}

void Compositor::update_frame_timing() {
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
}

void Compositor::limit_frame_rate() {
    if (settings_.enable_frame_limiting) {
        double target_frame_time = 1.0 / settings_.max_fps;
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(current_time - frame_start_time_).count();
        
        if (elapsed < target_frame_time) {
            double sleep_time = target_frame_time - elapsed;
            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>(sleep_time * 1000000)));
        }
    }
}

void Compositor::sync_with_gpu() {
    if (settings_.enable_gpu_sync) {
        // Wait for GPU to finish rendering
        glFinish();
    }
}

void Compositor::render_su1_window(std::shared_ptr<Window> window) {
    if (!window || !renderer_) return;
    
    // Render SU1 window with special effects
    window->render(renderer_);
    
    // Apply SU1-specific effects
    apply_su1_effects(window);
}

void Compositor::apply_su1_effects(std::shared_ptr<Window> window) {
    // Apply special effects for SU1 applications
    // This could include quantum effects, neural patterns, etc.
    
    // For now, just add a subtle glow effect
    if (is_effect_enabled(CompositorEffect::Glow)) {
        // Apply glow effect to SU1 windows
    }
}

} // namespace s1u

#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

Compositor::Compositor()
    : initialized_(false)
    , su1_composition_mode_(false)
    , frame_count_(0)
    , current_fps_(0.0)
    , average_frame_time_(0.0) {
    
    // Initialize frame timing
    last_frame_time_ = std::chrono::high_resolution_clock::now();
    frame_start_time_ = last_frame_time_;
}

Compositor::~Compositor() {
    shutdown();
}

bool Compositor::initialize(std::shared_ptr<Renderer> renderer, const CompositorSettings& settings) {
    std::cout << "[S1U] Initializing Compositor..." << std::endl;
    
    if (!renderer) {
        std::cerr << "[S1U] Cannot initialize compositor: renderer is null" << std::endl;
        return false;
    }
    
    renderer_ = renderer;
    settings_ = settings;
    
    try {
        // Setup render targets
        setup_render_targets();
        
        // Initialize shaders
        initialize_shaders();
        
        initialized_ = true;
        std::cout << "[S1U] Compositor initialized successfully!" << std::endl;
        std::cout << "[S1U] Vsync: " << (settings.enable_vsync ? "Enabled" : "Disabled") << std::endl;
        std::cout << "[S1U] Target FPS: " << settings.max_fps << std::endl;
        std::cout << "[S1U] MSAA: " << settings.msaa_samples << "x" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[S1U] Compositor initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void Compositor::shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "[S1U] Shutting down Compositor..." << std::endl;
    
    // Cleanup render targets
    if (main_target_.fbo) glDeleteFramebuffers(1, &main_target_.fbo);
    if (main_target_.texture) glDeleteTextures(1, &main_target_.texture);
    if (main_target_.depth_buffer) glDeleteRenderbuffers(1, &main_target_.depth_buffer);
    
    if (effect_target_.fbo) glDeleteFramebuffers(1, &effect_target_.fbo);
    if (effect_target_.texture) glDeleteTextures(1, &effect_target_.texture);
    if (effect_target_.depth_buffer) glDeleteRenderbuffers(1, &effect_target_.depth_buffer);
    
    if (blur_target_.fbo) glDeleteFramebuffers(1, &blur_target_.fbo);
    if (blur_target_.texture) glDeleteTextures(1, &blur_target_.texture);
    if (blur_target_.depth_buffer) glDeleteRenderbuffers(1, &blur_target_.depth_buffer);
    
    // Cleanup shaders
    if (composition_shader_) glDeleteProgram(composition_shader_);
    if (effect_shader_) glDeleteProgram(effect_shader_);
    if (blur_shader_) glDeleteProgram(blur_shader_);
    if (shadow_shader_) glDeleteProgram(shadow_shader_);
    
    initialized_ = false;
}

void Compositor::set_settings(const CompositorSettings& settings) {
    settings_ = settings;
    
    // Apply vsync setting
    if (renderer_) {
        renderer_->set_vsync(settings.enable_vsync);
    }
}

const CompositorSettings& Compositor::get_settings() const {
    return settings_;
}

void Compositor::add_window(std::shared_ptr<Window> window) {
    if (window) {
        windows_.push_back(window);
    }
}

void Compositor::remove_window(std::shared_ptr<Window> window) {
    auto it = std::find(windows_.begin(), windows_.end(), window);
    if (it != windows_.end()) {
        windows_.erase(it);
    }
}

void Compositor::update_window(std::shared_ptr<Window> window) {
    // Update window state if needed
    // This is called when window properties change
}

void Compositor::render_window(std::shared_ptr<Window> window) {
    if (!window || !renderer_) return;
    
    // Render individual window
    window->render(renderer_);
}

void Compositor::begin_composition() {
    if (!initialized_ || !renderer_) return;
    
    frame_start_time_ = std::chrono::high_resolution_clock::now();
    
    // Bind main render target
    glBindFramebuffer(GL_FRAMEBUFFER, main_target_.fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Compositor::compose_frame() {
    if (!initialized_ || !renderer_) return;
    
    // Render background
    render_background();
    
    // Render all windows
    render_windows();
    
    // Apply post-processing effects
    apply_post_effects();
}

void Compositor::end_composition() {
    if (!initialized_ || !renderer_) return;
    
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Final composition
    final_composition();
}

void Compositor::present_frame() {
    if (!initialized_ || !renderer_) return;
    
    // Present the composed frame
    renderer_->present();
    
    // Update frame timing
    update_frame_timing();
    
    frame_count_++;
}

void Compositor::enable_effect(CompositorEffect effect, bool enable) {
    enabled_effects_[effect] = enable;
}

void Compositor::set_effect_parameters(CompositorEffect effect, const std::vector<float>& parameters) {
    effect_parameters_[effect] = parameters;
}

bool Compositor::is_effect_enabled(CompositorEffect effect) const {
    auto it = enabled_effects_.find(effect);
    return (it != enabled_effects_.end()) ? it->second : false;
}

void Compositor::set_target_fps(uint32_t fps) {
    settings_.max_fps = fps;
}

void Compositor::set_vsync(bool enabled) {
    settings_.enable_vsync = enabled;
    if (renderer_) {
        renderer_->set_vsync(enabled);
    }
}

void Compositor::set_triple_buffering(bool enabled) {
    settings_.enable_triple_buffering = enabled;
}

void Compositor::set_adaptive_vsync(bool enabled) {
    settings_.enable_adaptive_vsync = enabled;
}

void Compositor::set_su1_composition_mode(bool enabled) {
    su1_composition_mode_ = enabled;
    std::cout << "[S1U] SU1 composition mode: " << (enabled ? "Enabled" : "Disabled") << std::endl;
}

void Compositor::render_su1_applications() {
    if (!su1_composition_mode_) return;
    
    // Render SU1 applications with special effects
    for (auto& [name, window] : su1_windows_) {
        if (window) {
            render_su1_window(window);
        }
    }
}

double Compositor::get_current_fps() const {
    return current_fps_;
}

uint64_t Compositor::get_frame_count() const {
    return frame_count_;
}

double Compositor::get_average_frame_time() const {
    return average_frame_time_;
}

uint32_t Compositor::get_draw_calls() const {
    return renderer_ ? renderer_->get_draw_calls() : 0;
}

uint32_t Compositor::get_triangle_count() const {
    // Simple triangle count estimation
    return get_draw_calls() * 2; // Assuming 2 triangles per draw call on average
}

// Private methods
void Compositor::setup_render_targets() {
    // Get window size from renderer
    Size window_size = renderer_->get_window_size();
    uint32_t width = static_cast<uint32_t>(window_size.width);
    uint32_t height = static_cast<uint32_t>(window_size.height);
    
    // Create main render target
    glGenFramebuffers(1, &main_target_.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, main_target_.fbo);
    
    // Create color texture
    glGenTextures(1, &main_target_.texture);
    glBindTexture(GL_TEXTURE_2D, main_target_.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, main_target_.texture, 0);
    
    // Create depth buffer
    glGenRenderbuffers(1, &main_target_.depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, main_target_.depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, main_target_.depth_buffer);
    
    main_target_.width = width;
    main_target_.height = height;
    
    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[S1U] Main framebuffer is not complete!" << std::endl;
        return;
    }
    
    // Create effect render target
    glGenFramebuffers(1, &effect_target_.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, effect_target_.fbo);
    
    glGenTextures(1, &effect_target_.texture);
    glBindTexture(GL_TEXTURE_2D, effect_target_.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, effect_target_.texture, 0);
    
    glGenRenderbuffers(1, &effect_target_.depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, effect_target_.depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, effect_target_.depth_buffer);
    
    effect_target_.width = width;
    effect_target_.height = height;
    
    // Create blur render target
    glGenFramebuffers(1, &blur_target_.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, blur_target_.fbo);
    
    glGenTextures(1, &blur_target_.texture);
    glBindTexture(GL_TEXTURE_2D, blur_target_.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur_target_.texture, 0);
    
    glGenRenderbuffers(1, &blur_target_.depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, blur_target_.depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, blur_target_.depth_buffer);
    
    blur_target_.width = width;
    blur_target_.height = height;
    
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    std::cout << "[S1U] Render targets setup complete" << std::endl;
}

void Compositor::initialize_shaders() {
    // Create basic composition shader
    const char* vertex_source = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    const char* fragment_source = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        
        uniform sampler2D uTexture;
        
        void main() {
            FragColor = texture(uTexture, TexCoord);
        }
    )";
    
    composition_shader_ = create_shader_program(vertex_source, fragment_source);
    
    // Create effect shader
    effect_shader_ = create_shader_program(vertex_source, fragment_source);
    
    // Create blur shader
    blur_shader_ = create_shader_program(vertex_source, fragment_source);
    
    // Create shadow shader
    shadow_shader_ = create_shader_program(vertex_source, fragment_source);
    
    std::cout << "[S1U] Shaders initialized" << std::endl;
}

uint32_t Compositor::create_shader_program(const char* vertex_source, const char* fragment_source) {
    // Compile vertex shader
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
    glCompileShader(vertex_shader);
    
    // Check for errors
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Vertex shader compilation failed: " << info_log << std::endl;
        return 0;
    }
    
    // Compile fragment shader
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
    glCompileShader(fragment_shader);
    
    // Check for errors
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Fragment shader compilation failed: " << info_log << std::endl;
        return 0;
    }
    
    // Link program
    uint32_t program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    // Check for errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        std::cerr << "[S1U] Shader program linking failed: " << info_log << std::endl;
        return 0;
    }
    
    // Cleanup
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return program;
}

void Compositor::render_background() {
    if (!renderer_) return;
    
    // Render desktop background
    Rect bg_rect(0, 0, 1920, 1080);
    Color bg_color(0.1f, 0.1f, 0.15f, 1.0f);
    renderer_->draw_rect(bg_rect, bg_color);
}

void Compositor::render_windows() {
    if (!renderer_) return;
    
    // Render all windows
    for (auto& window : windows_) {
        if (window && window->is_visible()) {
            window->render(renderer_);
        }
    }
}

void Compositor::apply_post_effects() {
    if (!renderer_) return;
    
    // Apply blur effect if enabled
    if (is_effect_enabled(CompositorEffect::Blur)) {
        render_blur_effect();
    }
    
    // Apply glow effect if enabled
    if (is_effect_enabled(CompositorEffect::Glow)) {
        render_glow_effect();
    }
    
    // Apply shadow effect if enabled
    if (is_effect_enabled(CompositorEffect::Shadow)) {
        render_shadow_effect();
    }
    
    // Apply liquid glass effect if enabled
    if (is_effect_enabled(CompositorEffect::Liquid)) {
        render_liquid_glass_effect();
    }
}

void Compositor::final_composition() {
    // Copy the composed frame to the screen
    // This is a simplified implementation
}

void Compositor::render_blur_effect() {
    // Render blur effect using the blur render target
    // This is a simplified implementation
}

void Compositor::render_glow_effect() {
    // Render glow effect
    // This is a simplified implementation
}

void Compositor::render_shadow_effect() {
    // Render shadow effect
    // This is a simplified implementation
}

void Compositor::render_transparency_effect() {
    // Render transparency effect
    // This is a simplified implementation
}

void Compositor::render_reflection_effect() {
    // Render reflection effect
    // This is a simplified implementation
}

void Compositor::render_liquid_glass_effect() {
    // Render liquid glass effect
    // This is a simplified implementation
}

void Compositor::update_frame_timing() {
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
}

void Compositor::limit_frame_rate() {
    if (settings_.enable_frame_limiting) {
        double target_frame_time = 1.0 / settings_.max_fps;
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(current_time - frame_start_time_).count();
        
        if (elapsed < target_frame_time) {
            double sleep_time = target_frame_time - elapsed;
            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>(sleep_time * 1000000)));
        }
    }
}

void Compositor::sync_with_gpu() {
    if (settings_.enable_gpu_sync) {
        // Wait for GPU to finish rendering
        glFinish();
    }
}

void Compositor::render_su1_window(std::shared_ptr<Window> window) {
    if (!window || !renderer_) return;
    
    // Render SU1 window with special effects
    window->render(renderer_);
    
    // Apply SU1-specific effects
    apply_su1_effects(window);
}

void Compositor::apply_su1_effects(std::shared_ptr<Window> window) {
    // Apply special effects for SU1 applications
    // This could include quantum effects, neural patterns, etc.
    
    // For now, just add a subtle glow effect
    if (is_effect_enabled(CompositorEffect::Glow)) {
        // Apply glow effect to SU1 windows
    }
}

} // namespace s1u
