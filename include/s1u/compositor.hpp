#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include "s1u/renderer.hpp"

namespace s1u {

// Forward declarations
class Window;
class Renderer;

// Compositor effects
enum class CompositorEffect {
    None,
    Blur,
    Glow,
    Shadow,
    Transparency,
    Reflection,
    Refraction,
    Distortion,
    Wave,
    Ripple,
    Liquid,
    Glass,
    Holographic,
    Quantum,
    Neural,
    Temporal,
    Spatial,
    Dimensional
};

// Compositor settings
struct CompositorSettings {
    bool enable_vsync = true;
    bool enable_triple_buffering = false;
    bool enable_adaptive_vsync = true;
    bool enable_tear_free = true;
    bool enable_low_latency = false;
    
    // Effect settings
    bool enable_blur = true;
    bool enable_shadows = true;
    bool enable_transparency = true;
    bool enable_reflections = true;
    
    // Performance settings
    uint32_t max_fps = 144;
    uint32_t target_frame_time_ms = 16;
    bool enable_frame_limiting = true;
    bool enable_gpu_sync = true;
    
    // Quality settings
    uint32_t msaa_samples = 4;
    bool enable_anisotropic_filtering = true;
    bool enable_trilinear_filtering = true;
    bool enable_mipmapping = true;
    
    CompositorSettings() = default;
};

// Compositor class for window composition and effects
class Compositor {
public:
    Compositor();
    ~Compositor();

    // Initialization and shutdown
    bool initialize(std::shared_ptr<Renderer> renderer, const CompositorSettings& settings = CompositorSettings{});
    void shutdown();

    // Configuration
    void set_settings(const CompositorSettings& settings);
    const CompositorSettings& get_settings() const;

    // Window management
    void add_window(std::shared_ptr<Window> window);
    void remove_window(std::shared_ptr<Window> window);
    void update_window(std::shared_ptr<Window> window);
    void render_window(std::shared_ptr<Window> window);

    // Composition
    void begin_composition();
    void compose_frame();
    void end_composition();
    void present_frame();

    // Effects
    void enable_effect(CompositorEffect effect, bool enable);
    void set_effect_parameters(CompositorEffect effect, const std::vector<float>& parameters);
    bool is_effect_enabled(CompositorEffect effect) const;

    // Performance
    void set_target_fps(uint32_t fps);
    void set_vsync(bool enabled);
    void set_triple_buffering(bool enabled);
    void set_adaptive_vsync(bool enabled);

    // SU1 integration
    void set_su1_composition_mode(bool enabled);
    void render_su1_applications();

    // Statistics
    double get_current_fps() const;
    double get_average_frame_time() const;
    uint32_t get_draw_calls() const;
    uint32_t get_triangle_count() const;
    uint64_t get_frame_count() const;

private:
    // Composition pipeline
    void setup_render_targets();
    void render_background();
    void render_windows();
    void apply_post_effects();
    void final_composition();

    // Effect rendering
    void render_blur_effect();
    void render_shadow_effect();
    void render_transparency_effect();
    void render_reflection_effect();
    void render_liquid_glass_effect();

    // Performance monitoring
    void update_frame_timing();
    void limit_frame_rate();
    void sync_with_gpu();

    // SU1 specific composition
    void render_su1_window(std::shared_ptr<Window> window);
    void apply_su1_effects(std::shared_ptr<Window> window);

    // Shader management
    void initialize_shaders();
    uint32_t create_shader_program(const char* vertex_source, const char* fragment_source);
    void render_glow_effect();

    // Settings and state
    CompositorSettings settings_;
    bool initialized_;
    std::shared_ptr<Renderer> renderer_;

    // Windows
    std::vector<std::shared_ptr<Window>> windows_;
    std::unordered_map<std::string, std::shared_ptr<Window>> su1_windows_;

    // Effects state
    std::unordered_map<CompositorEffect, bool> enabled_effects_;
    std::unordered_map<CompositorEffect, std::vector<float>> effect_parameters_;

    // Performance tracking
    std::chrono::high_resolution_clock::time_point last_frame_time_;
    std::chrono::high_resolution_clock::time_point frame_start_time_;
    uint64_t frame_count_;
    double current_fps_;
    double average_frame_time_;
    std::vector<double> frame_times_;

    // Render targets
    struct RenderTarget {
        uint32_t fbo;
        uint32_t texture;
        uint32_t depth_buffer;
        uint32_t width;
        uint32_t height;
    };
    RenderTarget main_target_;
    RenderTarget effect_target_;
    RenderTarget blur_target_;

    // Shaders
    uint32_t composition_shader_;
    uint32_t effect_shader_;
    uint32_t blur_shader_;
    uint32_t shadow_shader_;

    // SU1 composition mode
    bool su1_composition_mode_;
};

} // namespace s1u
