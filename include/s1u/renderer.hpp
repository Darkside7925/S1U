#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "s1u/core.hpp"

namespace s1u {

// Forward declarations
class Window;
class Texture;
class Shader;

// Basic types are defined in core.hpp

// Renderer class for OpenGL-based rendering
class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initialization and shutdown
    bool initialize(uint32_t width, uint32_t height, const std::string& title);
    void shutdown();

    // Window management
    GLFWwindow* get_window() const { return window_; }
    void set_window_size(uint32_t width, uint32_t height);
    Size get_window_size() const;
    bool should_close() const;

    // Frame control
    void begin_frame();
    void end_frame();
    void present();

    // Rendering methods
    void clear(const Color& color);
    void draw_rect(const Rect& rect, const Color& color);
    void draw_rect_outline(const Rect& rect, const Color& color, float thickness = 1.0f);
    void draw_circle(const Point& center, float radius, const Color& color);
    void draw_line(const Point& start, const Point& end, const Color& color, float thickness = 1.0f);
    void draw_text(const std::string& text, const Point& position, const Color& color, float size = 16.0f);

    // Texture management
    std::shared_ptr<Texture> create_texture(const std::string& path);
    void draw_texture(const std::shared_ptr<Texture>& texture, const Rect& rect);

    // Shader management
    std::shared_ptr<Shader> create_shader(const std::string& vertex_source, const std::string& fragment_source);
    void use_shader(const std::shared_ptr<Shader>& shader);

    // Viewport and projection
    void set_viewport(const Rect& viewport);
    void set_projection(float left, float right, float bottom, float top, float near = -1.0f, float far = 1.0f);

    // Effects
    void enable_blur(bool enable, float radius = 5.0f);
    void enable_glow(bool enable, const Color& color = Color(1.0f, 1.0f, 1.0f, 1.0f), float intensity = 1.0f);
    void enable_liquid_glass(bool enable, float intensity = 0.5f);
    
    // Glass theming
    void enable_glass_theme(bool enable, float opacity = 0.3f, float blur = 10.0f, float border = 1.0f, float highlight = 0.8f);
    void draw_glass_rect(const Rect& rect, const Color& color, float opacity = 0.3f, float blur = 10.0f);

    // Performance
    uint32_t get_draw_calls() const { return draw_calls_; }
    void reset_draw_calls() { draw_calls_ = 0; }

    // Vsync control
    void set_vsync(bool enabled);
    bool is_vsync_enabled() const { return vsync_enabled_; }

private:
    // OpenGL initialization
    bool initialize_opengl();
    bool initialize_shaders();
    bool initialize_buffers();

    // Shader compilation
    GLuint compile_shader(GLenum type, const std::string& source);
    GLuint link_program(GLuint vertex_shader, GLuint fragment_shader);
    uint32_t create_shader_program(const char* vertex_source, const char* fragment_source);
    void update_projection_matrix();
    void reset_effects();

    // GLFW window
    GLFWwindow* window_;
    uint32_t window_width_;
    uint32_t window_height_;
    std::string window_title_;

    // OpenGL objects
    GLuint vao_;
    GLuint vbo_;
    GLuint ebo_;

    // Shaders
    GLuint shader_program_;

    // Uniform locations
    GLint mvp_location_;
    GLint color_location_;
    GLint texture_location_;
    GLint opacity_location_;
    GLint blur_location_;
    GLint glass_location_;

    // State
    bool initialized_;
    bool vsync_enabled_;
    uint32_t draw_calls_;

    // Effects state
    bool blur_enabled_;
    float blur_radius_;
    bool glow_enabled_;
    Color glow_color_;
    float glow_intensity_;
    bool liquid_glass_enabled_;
    float liquid_glass_intensity_;
    
    // Glass theming state
    bool glass_theme_enabled_;
    float glass_opacity_;
    float glass_blur_;
    float glass_border_;
    float glass_highlight_;

    // Hardware detection and fallbacks
    bool use_software_fallback_;
    bool use_integrated_graphics_;
    bool use_amd_optimizations_;
    bool use_nvidia_optimizations_;

    // Projection matrix
    glm::mat4 projection_matrix_;
};

} // namespace s1u

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "s1u/core.hpp"

namespace s1u {

// Forward declarations
class Window;
class Texture;
class Shader;

// Basic types are defined in core.hpp

// Renderer class for OpenGL-based rendering
class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initialization and shutdown
    bool initialize(uint32_t width, uint32_t height, const std::string& title);
    void shutdown();

    // Window management
    GLFWwindow* get_window() const { return window_; }
    void set_window_size(uint32_t width, uint32_t height);
    Size get_window_size() const;
    bool should_close() const;

    // Frame control
    void begin_frame();
    void end_frame();
    void present();

    // Rendering methods
    void clear(const Color& color);
    void draw_rect(const Rect& rect, const Color& color);
    void draw_rect_outline(const Rect& rect, const Color& color, float thickness = 1.0f);
    void draw_circle(const Point& center, float radius, const Color& color);
    void draw_line(const Point& start, const Point& end, const Color& color, float thickness = 1.0f);
    void draw_text(const std::string& text, const Point& position, const Color& color, float size = 16.0f);

    // Texture management
    std::shared_ptr<Texture> create_texture(const std::string& path);
    void draw_texture(const std::shared_ptr<Texture>& texture, const Rect& rect);

    // Shader management
    std::shared_ptr<Shader> create_shader(const std::string& vertex_source, const std::string& fragment_source);
    void use_shader(const std::shared_ptr<Shader>& shader);

    // Viewport and projection
    void set_viewport(const Rect& viewport);
    void set_projection(float left, float right, float bottom, float top, float near = -1.0f, float far = 1.0f);

    // Effects
    void enable_blur(bool enable, float radius = 5.0f);
    void enable_glow(bool enable, const Color& color = Color(1.0f, 1.0f, 1.0f, 1.0f), float intensity = 1.0f);
    void enable_liquid_glass(bool enable, float intensity = 0.5f);
    
    // Glass theming
    void enable_glass_theme(bool enable, float opacity = 0.3f, float blur = 10.0f, float border = 1.0f, float highlight = 0.8f);
    void draw_glass_rect(const Rect& rect, const Color& color, float opacity = 0.3f, float blur = 10.0f);

    // Performance
    uint32_t get_draw_calls() const { return draw_calls_; }
    void reset_draw_calls() { draw_calls_ = 0; }

    // Vsync control
    void set_vsync(bool enabled);
    bool is_vsync_enabled() const { return vsync_enabled_; }

private:
    // OpenGL initialization
    bool initialize_opengl();
    bool initialize_shaders();
    bool initialize_buffers();

    // Shader compilation
    GLuint compile_shader(GLenum type, const std::string& source);
    GLuint link_program(GLuint vertex_shader, GLuint fragment_shader);
    uint32_t create_shader_program(const char* vertex_source, const char* fragment_source);
    void update_projection_matrix();
    void reset_effects();

    // GLFW window
    GLFWwindow* window_;
    uint32_t window_width_;
    uint32_t window_height_;
    std::string window_title_;

    // OpenGL objects
    GLuint vao_;
    GLuint vbo_;
    GLuint ebo_;

    // Shaders
    GLuint shader_program_;

    // Uniform locations
    GLint mvp_location_;
    GLint color_location_;
    GLint texture_location_;
    GLint opacity_location_;
    GLint blur_location_;
    GLint glass_location_;

    // State
    bool initialized_;
    bool vsync_enabled_;
    uint32_t draw_calls_;

    // Effects state
    bool blur_enabled_;
    float blur_radius_;
    bool glow_enabled_;
    Color glow_color_;
    float glow_intensity_;
    bool liquid_glass_enabled_;
    float liquid_glass_intensity_;
    
    // Glass theming state
    bool glass_theme_enabled_;
    float glass_opacity_;
    float glass_blur_;
    float glass_border_;
    float glass_highlight_;

    // Hardware detection and fallbacks
    bool use_software_fallback_;
    bool use_integrated_graphics_;
    bool use_amd_optimizations_;
    bool use_nvidia_optimizations_;

    // Projection matrix
    glm::mat4 projection_matrix_;
};

} // namespace s1u

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "s1u/core.hpp"

namespace s1u {

// Forward declarations
class Window;
class Texture;
class Shader;

// Basic types are defined in core.hpp

// Renderer class for OpenGL-based rendering
class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initialization and shutdown
    bool initialize(uint32_t width, uint32_t height, const std::string& title);
    void shutdown();

    // Window management
    GLFWwindow* get_window() const { return window_; }
    void set_window_size(uint32_t width, uint32_t height);
    Size get_window_size() const;
    bool should_close() const;

    // Frame control
    void begin_frame();
    void end_frame();
    void present();

    // Rendering methods
    void clear(const Color& color);
    void draw_rect(const Rect& rect, const Color& color);
    void draw_rect_outline(const Rect& rect, const Color& color, float thickness = 1.0f);
    void draw_circle(const Point& center, float radius, const Color& color);
    void draw_line(const Point& start, const Point& end, const Color& color, float thickness = 1.0f);
    void draw_text(const std::string& text, const Point& position, const Color& color, float size = 16.0f);

    // Texture management
    std::shared_ptr<Texture> create_texture(const std::string& path);
    void draw_texture(const std::shared_ptr<Texture>& texture, const Rect& rect);

    // Shader management
    std::shared_ptr<Shader> create_shader(const std::string& vertex_source, const std::string& fragment_source);
    void use_shader(const std::shared_ptr<Shader>& shader);

    // Viewport and projection
    void set_viewport(const Rect& viewport);
    void set_projection(float left, float right, float bottom, float top, float near = -1.0f, float far = 1.0f);

    // Effects
    void enable_blur(bool enable, float radius = 5.0f);
    void enable_glow(bool enable, const Color& color = Color(1.0f, 1.0f, 1.0f, 1.0f), float intensity = 1.0f);
    void enable_liquid_glass(bool enable, float intensity = 0.5f);
    
    // Glass theming
    void enable_glass_theme(bool enable, float opacity = 0.3f, float blur = 10.0f, float border = 1.0f, float highlight = 0.8f);
    void draw_glass_rect(const Rect& rect, const Color& color, float opacity = 0.3f, float blur = 10.0f);

    // Performance
    uint32_t get_draw_calls() const { return draw_calls_; }
    void reset_draw_calls() { draw_calls_ = 0; }

    // Vsync control
    void set_vsync(bool enabled);
    bool is_vsync_enabled() const { return vsync_enabled_; }

private:
    // OpenGL initialization
    bool initialize_opengl();
    bool initialize_shaders();
    bool initialize_buffers();

    // Shader compilation
    GLuint compile_shader(GLenum type, const std::string& source);
    GLuint link_program(GLuint vertex_shader, GLuint fragment_shader);
    uint32_t create_shader_program(const char* vertex_source, const char* fragment_source);
    void update_projection_matrix();
    void reset_effects();

    // GLFW window
    GLFWwindow* window_;
    uint32_t window_width_;
    uint32_t window_height_;
    std::string window_title_;

    // OpenGL objects
    GLuint vao_;
    GLuint vbo_;
    GLuint ebo_;

    // Shaders
    GLuint shader_program_;

    // Uniform locations
    GLint mvp_location_;
    GLint color_location_;
    GLint texture_location_;
    GLint opacity_location_;
    GLint blur_location_;
    GLint glass_location_;

    // State
    bool initialized_;
    bool vsync_enabled_;
    uint32_t draw_calls_;

    // Effects state
    bool blur_enabled_;
    float blur_radius_;
    bool glow_enabled_;
    Color glow_color_;
    float glow_intensity_;
    bool liquid_glass_enabled_;
    float liquid_glass_intensity_;
    
    // Glass theming state
    bool glass_theme_enabled_;
    float glass_opacity_;
    float glass_blur_;
    float glass_border_;
    float glass_highlight_;

    // Hardware detection and fallbacks
    bool use_software_fallback_;
    bool use_integrated_graphics_;
    bool use_amd_optimizations_;
    bool use_nvidia_optimizations_;

    // Projection matrix
    glm::mat4 projection_matrix_;
};

} // namespace s1u
