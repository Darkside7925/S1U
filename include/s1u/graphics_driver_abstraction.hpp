#pragma once

#include "s1u/core.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace s1u {

class GraphicsDriverAbstraction {
public:
    struct GraphicsCapabilities {
        std::string api_version;
        bool supports_opengl;
        bool supports_vulkan;
        bool supports_opengles;
        bool supports_raytracing;
        bool supports_variable_rate_shading;
        bool supports_mesh_shaders;
        bool supports_sampler_feedback;
        int max_texture_size;
        int max_renderbuffer_size;
        int max_viewport_width;
        int max_viewport_height;
        int max_framebuffer_samples;
        bool supports_multi_monitor;
        bool supports_hdr;
        bool supports_variable_refresh_rate;
        std::vector<std::string> supported_extensions;
        std::unordered_map<std::string, std::string> driver_info;
    };

    struct RenderContext {
        void* native_context;
        std::string api_type;
        int context_version_major;
        int context_version_minor;
        bool is_current;
        std::chrono::steady_clock::time_point creation_time;
        std::unordered_map<std::string, std::string> context_properties;
    };

    struct GraphicsSurface {
        void* native_surface;
        int width;
        int height;
        int refresh_rate;
        bool is_hdr_capable;
        bool is_variable_refresh_capable;
        std::string color_format;
        std::chrono::steady_clock::time_point creation_time;
        std::unordered_map<std::string, std::string> surface_properties;
    };

    struct ShaderProgram {
        void* native_program;
        std::string shader_type;
        std::string shader_source;
        bool is_compiled;
        std::chrono::steady_clock::time_point creation_time;
        std::vector<std::string> uniforms;
        std::vector<std::string> attributes;
        std::unordered_map<std::string, std::string> shader_properties;
    };

    struct TextureObject {
        void* native_texture;
        int width;
        int height;
        int depth;
        std::string format;
        std::string type;
        bool is_mipmapped;
        bool is_compressed;
        int memory_usage_kb;
        std::chrono::steady_clock::time_point creation_time;
        std::unordered_map<std::string, std::string> texture_properties;
    };

    struct BufferObject {
        void* native_buffer;
        size_t size_bytes;
        std::string usage;
        std::string memory_type;
        bool is_mapped;
        int memory_usage_kb;
        std::chrono::steady_clock::time_point creation_time;
        std::unordered_map<std::string, std::string> buffer_properties;
    };

    struct FramebufferObject {
        void* native_framebuffer;
        int width;
        int height;
        std::vector<TextureObject*> color_attachments;
        TextureObject* depth_attachment;
        TextureObject* stencil_attachment;
        bool is_multisampled;
        int samples;
        std::chrono::steady_clock::time_point creation_time;
        std::unordered_map<std::string, std::string> framebuffer_properties;
    };

    GraphicsDriverAbstraction();
    ~GraphicsDriverAbstraction();

    bool initialize();
    void shutdown();

    bool initialize_driver(const std::string& driver_name);

    // Resource creation
    RenderContext* create_render_context(const std::string& api_type, int major_version = 4, int minor_version = 6);
    GraphicsSurface* create_graphics_surface(int width, int height, int refresh_rate = 60, bool hdr = false, bool variable_refresh = false);
    ShaderProgram* create_shader_program(const std::string& shader_type, const std::string& source);
    TextureObject* create_texture(int width, int height, const std::string& format = "RGBA8", const std::string& type = "2d");
    BufferObject* create_buffer(size_t size_bytes, const std::string& usage = "vertex", const std::string& memory_type = "static");
    FramebufferObject* create_framebuffer(int width, int height, bool multisampled = false, int samples = 4);

    // Capabilities and information
    const GraphicsCapabilities& get_capabilities() const;
    std::string get_current_driver() const;
    std::vector<std::string> get_supported_drivers() const;

    // Performance monitoring
    int get_total_textures_created() const;
    int get_total_buffers_created() const;
    int get_total_shaders_compiled() const;
    int get_total_draw_calls() const;
    double get_average_frame_time_ms() const;
    int get_current_fps() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace s1u
