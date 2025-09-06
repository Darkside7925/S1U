#include "s1u/renderer.hpp"
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

Renderer::Renderer()
    : window_(nullptr)
    , window_width_(800)
    , window_height_(600)
    , window_title_("S1U Renderer")
    , initialized_(false)
    , vsync_enabled_(true)
    , draw_calls_(0)
    , vao_(0)
    , vbo_(0)
    , ebo_(0)
    , shader_program_(0)
    , use_software_fallback_(false)
    , use_integrated_graphics_(false)
    , use_amd_optimizations_(false)
    , use_nvidia_optimizations_(false) {
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize(uint32_t width, uint32_t height, const std::string& title) {
    window_width_ = width;
    window_height_ = height;
    window_title_ = title;
    
    try {
        if (!glfwInit()) {
            std::cerr << "[S1U] Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        if (!initialize_opengl()) {
            std::cerr << "[S1U] Failed to initialize OpenGL" << std::endl;
            return false;
        }
        
        if (!initialize_shaders()) {
            std::cerr << "[S1U] Failed to initialize shaders" << std::endl;
            return false;
        }
        
        if (!initialize_buffers()) {
            std::cerr << "[S1U] Failed to initialize buffers" << std::endl;
            return false;
        }
        
        initialized_ = true;
        std::cout << "[S1U] OpenGL Renderer initialized successfully!" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[S1U] Renderer initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void Renderer::shutdown() {
    if (initialized_) {
        if (vao_) glDeleteVertexArrays(1, &vao_);
        if (vbo_) glDeleteBuffers(1, &vbo_);
        if (ebo_) glDeleteBuffers(1, &ebo_);
        if (shader_program_) glDeleteProgram(shader_program_);
    }
    
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    
    glfwTerminate();
    initialized_ = false;
}

bool Renderer::initialize_opengl() {
    std::cout << "[S1U] ==========================================" << std::endl;
    std::cout << "[S1U] ENHANCED INITIALIZATION CODE PATH ACTIVE!" << std::endl;
    std::cout << "[S1U] ==========================================" << std::endl;
    
    // Set up GLFW error callback
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "[GLFW ERROR] " << error << ": " << description << std::endl;
    });
    
    // Check for GPU driver conflicts and environment variables
    std::cout << "[S1U] Checking GPU environment..." << std::endl;
    
    const char* display = getenv("DISPLAY");
    const char* xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    const char* wayland_display = getenv("WAYLAND_DISPLAY");
    
    std::cout << "[S1U] DISPLAY: " << (display ? display : "Not set") << std::endl;
    std::cout << "[S1U] XDG_RUNTIME_DIR: " << (xdg_runtime_dir ? xdg_runtime_dir : "Not set") << std::endl;
    std::cout << "[S1U] WAYLAND_DISPLAY: " << (wayland_display ? wayland_display : "Not set") << std::endl;
    
    // Check for common GPU driver conflicts
    const char* mesa_gl_version_override = getenv("MESA_GL_VERSION_OVERRIDE");
    const char* mesa_glsl_version_override = getenv("MESA_GLSL_VERSION_OVERRIDE");
    const char* libgl_always_software = getenv("LIBGL_ALWAYS_SOFTWARE");
    
    if (mesa_gl_version_override) {
        std::cout << "[S1U] MESA_GL_VERSION_OVERRIDE: " << mesa_gl_version_override << std::endl;
    }
    if (mesa_glsl_version_override) {
        std::cout << "[S1U] MESA_GLSL_VERSION_OVERRIDE: " << mesa_glsl_version_override << std::endl;
    }
    if (libgl_always_software) {
        std::cout << "[S1U] LIBGL_ALWAYS_SOFTWARE: " << libgl_always_software << std::endl;
    }
    
    // Try multiple OpenGL profiles and versions for maximum compatibility
    std::vector<std::pair<int, int>> opengl_versions = {
        {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0},  // OpenGL 4.x
        {3, 3}, {3, 2}, {3, 1}, {3, 0},                           // OpenGL 3.x
        {2, 1}, {2, 0},                                            // OpenGL 2.x
        {1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {1, 0}           // OpenGL 1.x
    };
    
    std::vector<int> profiles = {
        GLFW_OPENGL_CORE_PROFILE,
        GLFW_OPENGL_COMPAT_PROFILE,
        GLFW_OPENGL_ANY_PROFILE
    };
    
    bool window_created = false;
    
    for (const auto& profile : profiles) {
        for (const auto& version : opengl_versions) {
            std::cout << "[S1U] Trying OpenGL " << version.first << "." << version.second 
                      << " with profile " << (profile == GLFW_OPENGL_CORE_PROFILE ? "CORE" : 
                                            profile == GLFW_OPENGL_COMPAT_PROFILE ? "COMPAT" : "ANY") << std::endl;
            
            // Clear previous hints
            glfwDefaultWindowHints();
            
            // Set OpenGL version
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.first);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.second);
            
            // Set profile
            if (profile != GLFW_OPENGL_ANY_PROFILE) {
                glfwWindowHint(GLFW_OPENGL_PROFILE, profile);
            }
            
                // Set other hints
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            
            // Try to create window
            window_ = glfwCreateWindow(window_width_, window_height_, window_title_.c_str(), nullptr, nullptr);
            if (window_) {
                std::cout << "[S1U] Successfully created window with OpenGL " << version.first << "." << version.second << std::endl;
                window_created = true;
                break;
            } else {
                std::cout << "[S1U] Failed to create window with OpenGL " << version.first << "." << version.second << std::endl;
            }
        }
        if (window_created) break;
    }
    
    if (!window_) {
        std::cerr << "[S1U] Failed to create GLFW window with any OpenGL version/profile" << std::endl;
        return false;
    }
    
    std::cout << "[S1U] GLFW window created successfully: " << window_ << std::endl;
    
    // Make context current
    glfwMakeContextCurrent(window_);
    
    // Ensure window is visible and focused
    glfwShowWindow(window_);
    glfwFocusWindow(window_);
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "[S1U] Failed to initialize GLEW" << std::endl;
        return false;
    }
    
    // Set vsync
    glfwSwapInterval(vsync_enabled_ ? 1 : 0);
    glViewport(0, 0, window_width_, window_height_);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Detect and log GPU information
    const char* vendor = (const char*)glGetString(GL_VENDOR);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    const char* version = (const char*)glGetString(GL_VERSION);
    const char* glsl_version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    
    std::cout << "[S1U] OpenGL initialized successfully!" << std::endl;
    std::cout << "[S1U] Vendor: " << (vendor ? vendor : "Unknown") << std::endl;
    std::cout << "[S1U] GPU: " << (renderer ? renderer : "Unknown") << std::endl;
    std::cout << "[S1U] OpenGL Version: " << (version ? version : "Unknown") << std::endl;
    std::cout << "[S1U] GLSL Version: " << (glsl_version ? glsl_version : "Unknown") << std::endl;
    
    // Check for specific GPU types and set appropriate fallbacks
    if (renderer && strstr(renderer, "Mesa")) {
        std::cout << "[S1U] Detected Mesa driver - using software rendering fallbacks" << std::endl;
        use_software_fallback_ = true;
    } else if (renderer && strstr(renderer, "llvmpipe")) {
        std::cout << "[S1U] Detected LLVMpipe - using software rendering fallbacks" << std::endl;
        use_software_fallback_ = true;
    } else if (renderer && strstr(renderer, "Intel")) {
        std::cout << "[S1U] Detected Intel GPU - using integrated graphics optimizations" << std::endl;
        use_integrated_graphics_ = true;
    } else if (renderer && strstr(renderer, "AMD")) {
        std::cout << "[S1U] Detected AMD GPU - using AMD optimizations" << std::endl;
        use_amd_optimizations_ = true;
    } else if (renderer && strstr(renderer, "NVIDIA")) {
        std::cout << "[S1U] Detected NVIDIA GPU - using NVIDIA optimizations" << std::endl;
        use_nvidia_optimizations_ = true;
    }
    
    // Set appropriate rendering quality based on detected hardware
    if (use_software_fallback_) {
        std::cout << "[S1U] Using software rendering mode for maximum compatibility" << std::endl;
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    }
    
    // Try to set display hints for better compatibility
    if (use_integrated_graphics_) {
        std::cout << "[S1U] Setting integrated graphics optimizations" << std::endl;
        glfwWindowHint(GLFW_SAMPLES, 0); // Disable MSAA for integrated graphics
    }
    
    if (use_amd_optimizations_) {
        std::cout << "[S1U] Setting AMD optimizations" << std::endl;
        glfwWindowHint(GLFW_SAMPLES, 4); // Enable MSAA for AMD
    }
    
    if (use_nvidia_optimizations_) {
        std::cout << "[S1U] Setting NVIDIA optimizations" << std::endl;
        glfwWindowHint(GLFW_SAMPLES, 8); // Enable MSAA for NVIDIA
    }
    
    return true;
}

bool Renderer::initialize_shaders() {
    const char* vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        uniform mat4 uProjection;
        uniform mat4 uModel;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    const char* fragment_shader_source = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        
        uniform vec4 uColor;
        uniform float uOpacity;
        
        void main() {
            vec4 finalColor = uColor;
            finalColor.a *= uOpacity;
            FragColor = finalColor;
        }
    )";
    
    // Compile vertex shader
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Vertex shader compilation failed: " << info_log << std::endl;
        return false;
    }
    
    // Compile fragment shader
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Fragment shader compilation failed: " << info_log << std::endl;
        return false;
    }
    
    // Link shader program
    shader_program_ = glCreateProgram();
    if (shader_program_ == 0) {
        std::cerr << "[S1U] Error: Failed to create shader program!" << std::endl;
        return false;
    }
    
    glAttachShader(shader_program_, vertex_shader);
    glAttachShader(shader_program_, fragment_shader);
    glLinkProgram(shader_program_);
    
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program_, 512, nullptr, info_log);
        std::cerr << "[S1U] Shader program linking failed: " << info_log << std::endl;
        return false;
    }
    
    // Clean up shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    std::cout << "[S1U] Shader program created successfully: " << shader_program_ << std::endl;
    std::cout << "[S1U] Shaders initialized successfully!" << std::endl;
    return true;
}

bool Renderer::initialize_buffers() {
    // Create a simple quad for rendering rectangles
    float vertices[] = {
        // positions        // texture coords
         0.5f,  0.5f,     1.0f, 1.0f,   // top right
         0.5f, -0.5f,     1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f,     0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f,     0.0f, 1.0f    // top left
    };
    
    unsigned int indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };
    
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    
    // Check if OpenGL objects were created successfully
    if (vao_ == 0 || vbo_ == 0 || ebo_ == 0) {
        std::cerr << "[S1U] Error: Failed to create OpenGL objects!" << std::endl;
        return false;
    }
    
    std::cout << "[S1U] OpenGL objects created: VAO=" << vao_ << ", VBO=" << vbo_ << ", EBO=" << ebo_ << std::endl;
    
    glBindVertexArray(vao_);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Check for OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[S1U] OpenGL error during buffer setup: " << err << std::endl;
        return false;
    }
    
    glBindVertexArray(0);
    
    std::cout << "[S1U] Buffers initialized successfully!" << std::endl;
    return true;
}

void Renderer::begin_frame() {
    std::cout << "[DEBUG] Renderer::begin_frame() - entering" << std::endl;
    
    if (!initialized_ || !window_) {
        std::cout << "[DEBUG] Renderer::begin_frame() - early return, initialized=" << initialized_ << ", window=" << window_ << std::endl;
        return;
    }
    
    // Ensure the OpenGL context is current for this window
    std::cout << "[DEBUG] Renderer::begin_frame() - ensuring context is current" << std::endl;
    glfwMakeContextCurrent(window_);
    std::cout << "[DEBUG] Renderer::begin_frame() - context made current" << std::endl;
    
    std::cout << "[DEBUG] Renderer::begin_frame() - about to call glClear" << std::endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    std::cout << "[DEBUG] Renderer::begin_frame() - glClear completed" << std::endl;
    
    std::cout << "[DEBUG] Renderer::begin_frame() - about to call glClearColor" << std::endl;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    std::cout << "[DEBUG] Renderer::begin_frame() - glClearColor completed" << std::endl;
    
    draw_calls_ = 0;
    std::cout << "[DEBUG] Renderer::begin_frame() - completed successfully" << std::endl;
}

void Renderer::end_frame() {
    if (!initialized_) return;
    // Nothing needed for modern OpenGL
}

void Renderer::present() {
    if (!initialized_ || !window_) return;
    glfwSwapBuffers(window_);
}

void Renderer::clear(const Color& color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::draw_rect(const Rect& rect, const Color& color) {
    if (!initialized_) return;
    
    if (shader_program_ == 0) {
        std::cerr << "[S1U] Error: shader_program_ is 0!" << std::endl;
        return;
    }
    
    glUseProgram(shader_program_);
    
    // Set up projection matrix (orthographic for 2D)
    glm::mat4 projection = glm::ortho(0.0f, (float)window_width_, (float)window_height_, 0.0f, -1.0f, 1.0f);
    
    // Set up model matrix for this rectangle
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(rect.width, rect.height, 1.0f));
    
    // Get uniform locations with error checking
    GLint proj_loc = glGetUniformLocation(shader_program_, "uProjection");
    GLint model_loc = glGetUniformLocation(shader_program_, "uModel");
    GLint color_loc = glGetUniformLocation(shader_program_, "uColor");
    GLint opacity_loc = glGetUniformLocation(shader_program_, "uOpacity");
    
    if (proj_loc == -1 || model_loc == -1 || color_loc == -1 || opacity_loc == -1) {
        std::cerr << "[S1U] Error: Failed to get uniform locations!" << std::endl;
        return;
    }
    
    // Set uniforms
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4f(color_loc, color.r, color.g, color.b, color.a);
    glUniform1f(opacity_loc, 1.0f);
    
    // Check for OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[S1U] OpenGL error before draw: " << err << std::endl;
        return;
    }
    
    // Draw
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Check for OpenGL errors after draw
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[S1U] OpenGL error after draw: " << err << std::endl;
    }
    
    draw_calls_++;
}

void Renderer::draw_glass_rect(const Rect& rect, const Color& color, float opacity, float blur) {
    if (!initialized_) return;
    
    glUseProgram(shader_program_);
    
    // Set up projection matrix (orthographic for 2D)
    glm::mat4 projection = glm::ortho(0.0f, (float)window_width_, (float)window_height_, 0.0f, -1.0f, 1.0f);
    
    // Set up model matrix for this rectangle
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(rect.width, rect.height, 1.0f));
    
    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform4f(glGetUniformLocation(shader_program_, "uColor"), color.r, color.g, color.b, color.a);
    glUniform1f(glGetUniformLocation(shader_program_, "uOpacity"), opacity);
    
    // Draw
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    draw_calls_++;
}

void Renderer::draw_rect_outline(const Rect& rect, const Color& color, float thickness) {
    if (!initialized_) return;
    
    // Draw four thin rectangles for the outline
    // Top edge
    draw_rect(Rect{rect.x, rect.y, rect.width, thickness}, color);
    // Bottom edge
    draw_rect(Rect{rect.x, rect.y + rect.height - thickness, rect.width, thickness}, color);
    // Left edge
    draw_rect(Rect{rect.x, rect.y, thickness, rect.height}, color);
    // Right edge
    draw_rect(Rect{rect.x + rect.width - thickness, rect.y, thickness, rect.height}, color);
}

void Renderer::draw_text(const std::string& text, const Point& position, const Color& color, float size) {
    if (!initialized_) return;
    
    // Simple text rendering using small rectangles for each character
    float char_width = size * 0.6f;
    float char_height = size;
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        if (c == ' ') continue;
        
        Rect char_rect;
        char_rect.x = position.x + i * char_width;
        char_rect.y = position.y;
        char_rect.width = char_width * 0.8f;
        char_rect.height = char_height * 0.8f;
        
        draw_rect(char_rect, color);
    }
}

void Renderer::set_window_size(uint32_t width, uint32_t height) {
    if (window_) {
        glfwSetWindowSize(window_, width, height);
        window_width_ = width;
        window_height_ = height;
        glViewport(0, 0, width, height);
    }
}

Size Renderer::get_window_size() const {
    return Size(window_width_, window_height_);
}

bool Renderer::should_close() const {
    return window_ ? glfwWindowShouldClose(window_) : true;
}

void Renderer::set_vsync(bool enabled) {
    vsync_enabled_ = enabled;
    if (window_) {
        glfwSwapInterval(enabled ? 1 : 0);
    }
}

void Renderer::set_viewport(const Rect& viewport) {
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
}

void Renderer::enable_glass_theme(bool enable, float opacity, float blur, float border, float highlight) {
    // This is handled in draw_glass_rect now
}

} // namespace s1u

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

Renderer::Renderer()
    : window_(nullptr)
    , window_width_(800)
    , window_height_(600)
    , window_title_("S1U Renderer")
    , initialized_(false)
    , vsync_enabled_(true)
    , draw_calls_(0)
    , vao_(0)
    , vbo_(0)
    , ebo_(0)
    , shader_program_(0)
    , use_software_fallback_(false)
    , use_integrated_graphics_(false)
    , use_amd_optimizations_(false)
    , use_nvidia_optimizations_(false) {
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize(uint32_t width, uint32_t height, const std::string& title) {
    window_width_ = width;
    window_height_ = height;
    window_title_ = title;
    
    try {
        if (!glfwInit()) {
            std::cerr << "[S1U] Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        if (!initialize_opengl()) {
            std::cerr << "[S1U] Failed to initialize OpenGL" << std::endl;
            return false;
        }
        
        if (!initialize_shaders()) {
            std::cerr << "[S1U] Failed to initialize shaders" << std::endl;
            return false;
        }
        
        if (!initialize_buffers()) {
            std::cerr << "[S1U] Failed to initialize buffers" << std::endl;
            return false;
        }
        
        initialized_ = true;
        std::cout << "[S1U] OpenGL Renderer initialized successfully!" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[S1U] Renderer initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void Renderer::shutdown() {
    if (initialized_) {
        if (vao_) glDeleteVertexArrays(1, &vao_);
        if (vbo_) glDeleteBuffers(1, &vbo_);
        if (ebo_) glDeleteBuffers(1, &ebo_);
        if (shader_program_) glDeleteProgram(shader_program_);
    }
    
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    
    glfwTerminate();
    initialized_ = false;
}

bool Renderer::initialize_opengl() {
    std::cout << "[S1U] ==========================================" << std::endl;
    std::cout << "[S1U] ENHANCED INITIALIZATION CODE PATH ACTIVE!" << std::endl;
    std::cout << "[S1U] ==========================================" << std::endl;
    
    // Set up GLFW error callback
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "[GLFW ERROR] " << error << ": " << description << std::endl;
    });
    
    // Check for GPU driver conflicts and environment variables
    std::cout << "[S1U] Checking GPU environment..." << std::endl;
    
    const char* display = getenv("DISPLAY");
    const char* xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    const char* wayland_display = getenv("WAYLAND_DISPLAY");
    
    std::cout << "[S1U] DISPLAY: " << (display ? display : "Not set") << std::endl;
    std::cout << "[S1U] XDG_RUNTIME_DIR: " << (xdg_runtime_dir ? xdg_runtime_dir : "Not set") << std::endl;
    std::cout << "[S1U] WAYLAND_DISPLAY: " << (wayland_display ? wayland_display : "Not set") << std::endl;
    
    // Check for common GPU driver conflicts
    const char* mesa_gl_version_override = getenv("MESA_GL_VERSION_OVERRIDE");
    const char* mesa_glsl_version_override = getenv("MESA_GLSL_VERSION_OVERRIDE");
    const char* libgl_always_software = getenv("LIBGL_ALWAYS_SOFTWARE");
    
    if (mesa_gl_version_override) {
        std::cout << "[S1U] MESA_GL_VERSION_OVERRIDE: " << mesa_gl_version_override << std::endl;
    }
    if (mesa_glsl_version_override) {
        std::cout << "[S1U] MESA_GLSL_VERSION_OVERRIDE: " << mesa_glsl_version_override << std::endl;
    }
    if (libgl_always_software) {
        std::cout << "[S1U] LIBGL_ALWAYS_SOFTWARE: " << libgl_always_software << std::endl;
    }
    
    // Try multiple OpenGL profiles and versions for maximum compatibility
    std::vector<std::pair<int, int>> opengl_versions = {
        {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0},  // OpenGL 4.x
        {3, 3}, {3, 2}, {3, 1}, {3, 0},                           // OpenGL 3.x
        {2, 1}, {2, 0},                                            // OpenGL 2.x
        {1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {1, 0}           // OpenGL 1.x
    };
    
    std::vector<int> profiles = {
        GLFW_OPENGL_CORE_PROFILE,
        GLFW_OPENGL_COMPAT_PROFILE,
        GLFW_OPENGL_ANY_PROFILE
    };
    
    bool window_created = false;
    
    for (const auto& profile : profiles) {
        for (const auto& version : opengl_versions) {
            std::cout << "[S1U] Trying OpenGL " << version.first << "." << version.second 
                      << " with profile " << (profile == GLFW_OPENGL_CORE_PROFILE ? "CORE" : 
                                            profile == GLFW_OPENGL_COMPAT_PROFILE ? "COMPAT" : "ANY") << std::endl;
            
            // Clear previous hints
            glfwDefaultWindowHints();
            
            // Set OpenGL version
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.first);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.second);
            
            // Set profile
            if (profile != GLFW_OPENGL_ANY_PROFILE) {
                glfwWindowHint(GLFW_OPENGL_PROFILE, profile);
            }
            
                // Set other hints
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            
            // Try to create window
            window_ = glfwCreateWindow(window_width_, window_height_, window_title_.c_str(), nullptr, nullptr);
            if (window_) {
                std::cout << "[S1U] Successfully created window with OpenGL " << version.first << "." << version.second << std::endl;
                window_created = true;
                break;
            } else {
                std::cout << "[S1U] Failed to create window with OpenGL " << version.first << "." << version.second << std::endl;
            }
        }
        if (window_created) break;
    }
    
    if (!window_) {
        std::cerr << "[S1U] Failed to create GLFW window with any OpenGL version/profile" << std::endl;
        return false;
    }
    
    std::cout << "[S1U] GLFW window created successfully: " << window_ << std::endl;
    
    // Make context current
    glfwMakeContextCurrent(window_);
    
    // Ensure window is visible and focused
    glfwShowWindow(window_);
    glfwFocusWindow(window_);
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "[S1U] Failed to initialize GLEW" << std::endl;
        return false;
    }
    
    // Set vsync
    glfwSwapInterval(vsync_enabled_ ? 1 : 0);
    glViewport(0, 0, window_width_, window_height_);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Detect and log GPU information
    const char* vendor = (const char*)glGetString(GL_VENDOR);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    const char* version = (const char*)glGetString(GL_VERSION);
    const char* glsl_version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    
    std::cout << "[S1U] OpenGL initialized successfully!" << std::endl;
    std::cout << "[S1U] Vendor: " << (vendor ? vendor : "Unknown") << std::endl;
    std::cout << "[S1U] GPU: " << (renderer ? renderer : "Unknown") << std::endl;
    std::cout << "[S1U] OpenGL Version: " << (version ? version : "Unknown") << std::endl;
    std::cout << "[S1U] GLSL Version: " << (glsl_version ? glsl_version : "Unknown") << std::endl;
    
    // Check for specific GPU types and set appropriate fallbacks
    if (renderer && strstr(renderer, "Mesa")) {
        std::cout << "[S1U] Detected Mesa driver - using software rendering fallbacks" << std::endl;
        use_software_fallback_ = true;
    } else if (renderer && strstr(renderer, "llvmpipe")) {
        std::cout << "[S1U] Detected LLVMpipe - using software rendering fallbacks" << std::endl;
        use_software_fallback_ = true;
    } else if (renderer && strstr(renderer, "Intel")) {
        std::cout << "[S1U] Detected Intel GPU - using integrated graphics optimizations" << std::endl;
        use_integrated_graphics_ = true;
    } else if (renderer && strstr(renderer, "AMD")) {
        std::cout << "[S1U] Detected AMD GPU - using AMD optimizations" << std::endl;
        use_amd_optimizations_ = true;
    } else if (renderer && strstr(renderer, "NVIDIA")) {
        std::cout << "[S1U] Detected NVIDIA GPU - using NVIDIA optimizations" << std::endl;
        use_nvidia_optimizations_ = true;
    }
    
    // Set appropriate rendering quality based on detected hardware
    if (use_software_fallback_) {
        std::cout << "[S1U] Using software rendering mode for maximum compatibility" << std::endl;
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    }
    
    // Try to set display hints for better compatibility
    if (use_integrated_graphics_) {
        std::cout << "[S1U] Setting integrated graphics optimizations" << std::endl;
        glfwWindowHint(GLFW_SAMPLES, 0); // Disable MSAA for integrated graphics
    }
    
    if (use_amd_optimizations_) {
        std::cout << "[S1U] Setting AMD optimizations" << std::endl;
        glfwWindowHint(GLFW_SAMPLES, 4); // Enable MSAA for AMD
    }
    
    if (use_nvidia_optimizations_) {
        std::cout << "[S1U] Setting NVIDIA optimizations" << std::endl;
        glfwWindowHint(GLFW_SAMPLES, 8); // Enable MSAA for NVIDIA
    }
    
    return true;
}

bool Renderer::initialize_shaders() {
    const char* vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        uniform mat4 uProjection;
        uniform mat4 uModel;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    const char* fragment_shader_source = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        
        uniform vec4 uColor;
        uniform float uOpacity;
        
        void main() {
            vec4 finalColor = uColor;
            finalColor.a *= uOpacity;
            FragColor = finalColor;
        }
    )";
    
    // Compile vertex shader
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Vertex shader compilation failed: " << info_log << std::endl;
        return false;
    }
    
    // Compile fragment shader
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Fragment shader compilation failed: " << info_log << std::endl;
        return false;
    }
    
    // Link shader program
    shader_program_ = glCreateProgram();
    if (shader_program_ == 0) {
        std::cerr << "[S1U] Error: Failed to create shader program!" << std::endl;
        return false;
    }
    
    glAttachShader(shader_program_, vertex_shader);
    glAttachShader(shader_program_, fragment_shader);
    glLinkProgram(shader_program_);
    
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program_, 512, nullptr, info_log);
        std::cerr << "[S1U] Shader program linking failed: " << info_log << std::endl;
        return false;
    }
    
    // Clean up shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    std::cout << "[S1U] Shader program created successfully: " << shader_program_ << std::endl;
    std::cout << "[S1U] Shaders initialized successfully!" << std::endl;
    return true;
}

bool Renderer::initialize_buffers() {
    // Create a simple quad for rendering rectangles
    float vertices[] = {
        // positions        // texture coords
         0.5f,  0.5f,     1.0f, 1.0f,   // top right
         0.5f, -0.5f,     1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f,     0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f,     0.0f, 1.0f    // top left
    };
    
    unsigned int indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };
    
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    
    // Check if OpenGL objects were created successfully
    if (vao_ == 0 || vbo_ == 0 || ebo_ == 0) {
        std::cerr << "[S1U] Error: Failed to create OpenGL objects!" << std::endl;
        return false;
    }
    
    std::cout << "[S1U] OpenGL objects created: VAO=" << vao_ << ", VBO=" << vbo_ << ", EBO=" << ebo_ << std::endl;
    
    glBindVertexArray(vao_);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Check for OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[S1U] OpenGL error during buffer setup: " << err << std::endl;
        return false;
    }
    
    glBindVertexArray(0);
    
    std::cout << "[S1U] Buffers initialized successfully!" << std::endl;
    return true;
}

void Renderer::begin_frame() {
    std::cout << "[DEBUG] Renderer::begin_frame() - entering" << std::endl;
    
    if (!initialized_ || !window_) {
        std::cout << "[DEBUG] Renderer::begin_frame() - early return, initialized=" << initialized_ << ", window=" << window_ << std::endl;
        return;
    }
    
    // Ensure the OpenGL context is current for this window
    std::cout << "[DEBUG] Renderer::begin_frame() - ensuring context is current" << std::endl;
    glfwMakeContextCurrent(window_);
    std::cout << "[DEBUG] Renderer::begin_frame() - context made current" << std::endl;
    
    std::cout << "[DEBUG] Renderer::begin_frame() - about to call glClear" << std::endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    std::cout << "[DEBUG] Renderer::begin_frame() - glClear completed" << std::endl;
    
    std::cout << "[DEBUG] Renderer::begin_frame() - about to call glClearColor" << std::endl;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    std::cout << "[DEBUG] Renderer::begin_frame() - glClearColor completed" << std::endl;
    
    draw_calls_ = 0;
    std::cout << "[DEBUG] Renderer::begin_frame() - completed successfully" << std::endl;
}

void Renderer::end_frame() {
    if (!initialized_) return;
    // Nothing needed for modern OpenGL
}

void Renderer::present() {
    if (!initialized_ || !window_) return;
    glfwSwapBuffers(window_);
}

void Renderer::clear(const Color& color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::draw_rect(const Rect& rect, const Color& color) {
    if (!initialized_) return;
    
    if (shader_program_ == 0) {
        std::cerr << "[S1U] Error: shader_program_ is 0!" << std::endl;
        return;
    }
    
    glUseProgram(shader_program_);
    
    // Set up projection matrix (orthographic for 2D)
    glm::mat4 projection = glm::ortho(0.0f, (float)window_width_, (float)window_height_, 0.0f, -1.0f, 1.0f);
    
    // Set up model matrix for this rectangle
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(rect.width, rect.height, 1.0f));
    
    // Get uniform locations with error checking
    GLint proj_loc = glGetUniformLocation(shader_program_, "uProjection");
    GLint model_loc = glGetUniformLocation(shader_program_, "uModel");
    GLint color_loc = glGetUniformLocation(shader_program_, "uColor");
    GLint opacity_loc = glGetUniformLocation(shader_program_, "uOpacity");
    
    if (proj_loc == -1 || model_loc == -1 || color_loc == -1 || opacity_loc == -1) {
        std::cerr << "[S1U] Error: Failed to get uniform locations!" << std::endl;
        return;
    }
    
    // Set uniforms
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4f(color_loc, color.r, color.g, color.b, color.a);
    glUniform1f(opacity_loc, 1.0f);
    
    // Check for OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[S1U] OpenGL error before draw: " << err << std::endl;
        return;
    }
    
    // Draw
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Check for OpenGL errors after draw
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[S1U] OpenGL error after draw: " << err << std::endl;
    }
    
    draw_calls_++;
}

void Renderer::draw_glass_rect(const Rect& rect, const Color& color, float opacity, float blur) {
    if (!initialized_) return;
    
    glUseProgram(shader_program_);
    
    // Set up projection matrix (orthographic for 2D)
    glm::mat4 projection = glm::ortho(0.0f, (float)window_width_, (float)window_height_, 0.0f, -1.0f, 1.0f);
    
    // Set up model matrix for this rectangle
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(rect.width, rect.height, 1.0f));
    
    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform4f(glGetUniformLocation(shader_program_, "uColor"), color.r, color.g, color.b, color.a);
    glUniform1f(glGetUniformLocation(shader_program_, "uOpacity"), opacity);
    
    // Draw
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    draw_calls_++;
}

void Renderer::draw_rect_outline(const Rect& rect, const Color& color, float thickness) {
    if (!initialized_) return;
    
    // Draw four thin rectangles for the outline
    // Top edge
    draw_rect(Rect{rect.x, rect.y, rect.width, thickness}, color);
    // Bottom edge
    draw_rect(Rect{rect.x, rect.y + rect.height - thickness, rect.width, thickness}, color);
    // Left edge
    draw_rect(Rect{rect.x, rect.y, thickness, rect.height}, color);
    // Right edge
    draw_rect(Rect{rect.x + rect.width - thickness, rect.y, thickness, rect.height}, color);
}

void Renderer::draw_text(const std::string& text, const Point& position, const Color& color, float size) {
    if (!initialized_) return;
    
    // Simple text rendering using small rectangles for each character
    float char_width = size * 0.6f;
    float char_height = size;
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        if (c == ' ') continue;
        
        Rect char_rect;
        char_rect.x = position.x + i * char_width;
        char_rect.y = position.y;
        char_rect.width = char_width * 0.8f;
        char_rect.height = char_height * 0.8f;
        
        draw_rect(char_rect, color);
    }
}

void Renderer::set_window_size(uint32_t width, uint32_t height) {
    if (window_) {
        glfwSetWindowSize(window_, width, height);
        window_width_ = width;
        window_height_ = height;
        glViewport(0, 0, width, height);
    }
}

Size Renderer::get_window_size() const {
    return Size(window_width_, window_height_);
}

bool Renderer::should_close() const {
    return window_ ? glfwWindowShouldClose(window_) : true;
}

void Renderer::set_vsync(bool enabled) {
    vsync_enabled_ = enabled;
    if (window_) {
        glfwSwapInterval(enabled ? 1 : 0);
    }
}

void Renderer::set_viewport(const Rect& viewport) {
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
}

void Renderer::enable_glass_theme(bool enable, float opacity, float blur, float border, float highlight) {
    // This is handled in draw_glass_rect now
}

} // namespace s1u

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

Renderer::Renderer()
    : window_(nullptr)
    , window_width_(800)
    , window_height_(600)
    , window_title_("S1U Renderer")
    , initialized_(false)
    , vsync_enabled_(true)
    , draw_calls_(0)
    , vao_(0)
    , vbo_(0)
    , ebo_(0)
    , shader_program_(0)
    , use_software_fallback_(false)
    , use_integrated_graphics_(false)
    , use_amd_optimizations_(false)
    , use_nvidia_optimizations_(false) {
}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::initialize(uint32_t width, uint32_t height, const std::string& title) {
    window_width_ = width;
    window_height_ = height;
    window_title_ = title;
    
    try {
        if (!glfwInit()) {
            std::cerr << "[S1U] Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        if (!initialize_opengl()) {
            std::cerr << "[S1U] Failed to initialize OpenGL" << std::endl;
            return false;
        }
        
        if (!initialize_shaders()) {
            std::cerr << "[S1U] Failed to initialize shaders" << std::endl;
            return false;
        }
        
        if (!initialize_buffers()) {
            std::cerr << "[S1U] Failed to initialize buffers" << std::endl;
            return false;
        }
        
        initialized_ = true;
        std::cout << "[S1U] OpenGL Renderer initialized successfully!" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[S1U] Renderer initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void Renderer::shutdown() {
    if (initialized_) {
        if (vao_) glDeleteVertexArrays(1, &vao_);
        if (vbo_) glDeleteBuffers(1, &vbo_);
        if (ebo_) glDeleteBuffers(1, &ebo_);
        if (shader_program_) glDeleteProgram(shader_program_);
    }
    
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    
    glfwTerminate();
    initialized_ = false;
}

bool Renderer::initialize_opengl() {
    std::cout << "[S1U] ==========================================" << std::endl;
    std::cout << "[S1U] ENHANCED INITIALIZATION CODE PATH ACTIVE!" << std::endl;
    std::cout << "[S1U] ==========================================" << std::endl;
    
    // Set up GLFW error callback
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "[GLFW ERROR] " << error << ": " << description << std::endl;
    });
    
    // Check for GPU driver conflicts and environment variables
    std::cout << "[S1U] Checking GPU environment..." << std::endl;
    
    const char* display = getenv("DISPLAY");
    const char* xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    const char* wayland_display = getenv("WAYLAND_DISPLAY");
    
    std::cout << "[S1U] DISPLAY: " << (display ? display : "Not set") << std::endl;
    std::cout << "[S1U] XDG_RUNTIME_DIR: " << (xdg_runtime_dir ? xdg_runtime_dir : "Not set") << std::endl;
    std::cout << "[S1U] WAYLAND_DISPLAY: " << (wayland_display ? wayland_display : "Not set") << std::endl;
    
    // Check for common GPU driver conflicts
    const char* mesa_gl_version_override = getenv("MESA_GL_VERSION_OVERRIDE");
    const char* mesa_glsl_version_override = getenv("MESA_GLSL_VERSION_OVERRIDE");
    const char* libgl_always_software = getenv("LIBGL_ALWAYS_SOFTWARE");
    
    if (mesa_gl_version_override) {
        std::cout << "[S1U] MESA_GL_VERSION_OVERRIDE: " << mesa_gl_version_override << std::endl;
    }
    if (mesa_glsl_version_override) {
        std::cout << "[S1U] MESA_GLSL_VERSION_OVERRIDE: " << mesa_glsl_version_override << std::endl;
    }
    if (libgl_always_software) {
        std::cout << "[S1U] LIBGL_ALWAYS_SOFTWARE: " << libgl_always_software << std::endl;
    }
    
    // Try multiple OpenGL profiles and versions for maximum compatibility
    std::vector<std::pair<int, int>> opengl_versions = {
        {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0},  // OpenGL 4.x
        {3, 3}, {3, 2}, {3, 1}, {3, 0},                           // OpenGL 3.x
        {2, 1}, {2, 0},                                            // OpenGL 2.x
        {1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}, {1, 0}           // OpenGL 1.x
    };
    
    std::vector<int> profiles = {
        GLFW_OPENGL_CORE_PROFILE,
        GLFW_OPENGL_COMPAT_PROFILE,
        GLFW_OPENGL_ANY_PROFILE
    };
    
    bool window_created = false;
    
    for (const auto& profile : profiles) {
        for (const auto& version : opengl_versions) {
            std::cout << "[S1U] Trying OpenGL " << version.first << "." << version.second 
                      << " with profile " << (profile == GLFW_OPENGL_CORE_PROFILE ? "CORE" : 
                                            profile == GLFW_OPENGL_COMPAT_PROFILE ? "COMPAT" : "ANY") << std::endl;
            
            // Clear previous hints
            glfwDefaultWindowHints();
            
            // Set OpenGL version
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.first);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.second);
            
            // Set profile
            if (profile != GLFW_OPENGL_ANY_PROFILE) {
                glfwWindowHint(GLFW_OPENGL_PROFILE, profile);
            }
            
                // Set other hints
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            
            // Try to create window
            window_ = glfwCreateWindow(window_width_, window_height_, window_title_.c_str(), nullptr, nullptr);
            if (window_) {
                std::cout << "[S1U] Successfully created window with OpenGL " << version.first << "." << version.second << std::endl;
                window_created = true;
                break;
            } else {
                std::cout << "[S1U] Failed to create window with OpenGL " << version.first << "." << version.second << std::endl;
            }
        }
        if (window_created) break;
    }
    
    if (!window_) {
        std::cerr << "[S1U] Failed to create GLFW window with any OpenGL version/profile" << std::endl;
        return false;
    }
    
    std::cout << "[S1U] GLFW window created successfully: " << window_ << std::endl;
    
    // Make context current
    glfwMakeContextCurrent(window_);
    
    // Ensure window is visible and focused
    glfwShowWindow(window_);
    glfwFocusWindow(window_);
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "[S1U] Failed to initialize GLEW" << std::endl;
        return false;
    }
    
    // Set vsync
    glfwSwapInterval(vsync_enabled_ ? 1 : 0);
    glViewport(0, 0, window_width_, window_height_);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Detect and log GPU information
    const char* vendor = (const char*)glGetString(GL_VENDOR);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    const char* version = (const char*)glGetString(GL_VERSION);
    const char* glsl_version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    
    std::cout << "[S1U] OpenGL initialized successfully!" << std::endl;
    std::cout << "[S1U] Vendor: " << (vendor ? vendor : "Unknown") << std::endl;
    std::cout << "[S1U] GPU: " << (renderer ? renderer : "Unknown") << std::endl;
    std::cout << "[S1U] OpenGL Version: " << (version ? version : "Unknown") << std::endl;
    std::cout << "[S1U] GLSL Version: " << (glsl_version ? glsl_version : "Unknown") << std::endl;
    
    // Check for specific GPU types and set appropriate fallbacks
    if (renderer && strstr(renderer, "Mesa")) {
        std::cout << "[S1U] Detected Mesa driver - using software rendering fallbacks" << std::endl;
        use_software_fallback_ = true;
    } else if (renderer && strstr(renderer, "llvmpipe")) {
        std::cout << "[S1U] Detected LLVMpipe - using software rendering fallbacks" << std::endl;
        use_software_fallback_ = true;
    } else if (renderer && strstr(renderer, "Intel")) {
        std::cout << "[S1U] Detected Intel GPU - using integrated graphics optimizations" << std::endl;
        use_integrated_graphics_ = true;
    } else if (renderer && strstr(renderer, "AMD")) {
        std::cout << "[S1U] Detected AMD GPU - using AMD optimizations" << std::endl;
        use_amd_optimizations_ = true;
    } else if (renderer && strstr(renderer, "NVIDIA")) {
        std::cout << "[S1U] Detected NVIDIA GPU - using NVIDIA optimizations" << std::endl;
        use_nvidia_optimizations_ = true;
    }
    
    // Set appropriate rendering quality based on detected hardware
    if (use_software_fallback_) {
        std::cout << "[S1U] Using software rendering mode for maximum compatibility" << std::endl;
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    }
    
    // Try to set display hints for better compatibility
    if (use_integrated_graphics_) {
        std::cout << "[S1U] Setting integrated graphics optimizations" << std::endl;
        glfwWindowHint(GLFW_SAMPLES, 0); // Disable MSAA for integrated graphics
    }
    
    if (use_amd_optimizations_) {
        std::cout << "[S1U] Setting AMD optimizations" << std::endl;
        glfwWindowHint(GLFW_SAMPLES, 4); // Enable MSAA for AMD
    }
    
    if (use_nvidia_optimizations_) {
        std::cout << "[S1U] Setting NVIDIA optimizations" << std::endl;
        glfwWindowHint(GLFW_SAMPLES, 8); // Enable MSAA for NVIDIA
    }
    
    return true;
}

bool Renderer::initialize_shaders() {
    const char* vertex_shader_source = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        
        uniform mat4 uProjection;
        uniform mat4 uModel;
        
        out vec2 TexCoord;
        
        void main() {
            gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    const char* fragment_shader_source = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        
        uniform vec4 uColor;
        uniform float uOpacity;
        
        void main() {
            vec4 finalColor = uColor;
            finalColor.a *= uOpacity;
            FragColor = finalColor;
        }
    )";
    
    // Compile vertex shader
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Vertex shader compilation failed: " << info_log << std::endl;
        return false;
    }
    
    // Compile fragment shader
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        std::cerr << "[S1U] Fragment shader compilation failed: " << info_log << std::endl;
        return false;
    }
    
    // Link shader program
    shader_program_ = glCreateProgram();
    if (shader_program_ == 0) {
        std::cerr << "[S1U] Error: Failed to create shader program!" << std::endl;
        return false;
    }
    
    glAttachShader(shader_program_, vertex_shader);
    glAttachShader(shader_program_, fragment_shader);
    glLinkProgram(shader_program_);
    
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program_, 512, nullptr, info_log);
        std::cerr << "[S1U] Shader program linking failed: " << info_log << std::endl;
        return false;
    }
    
    // Clean up shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    std::cout << "[S1U] Shader program created successfully: " << shader_program_ << std::endl;
    std::cout << "[S1U] Shaders initialized successfully!" << std::endl;
    return true;
}

bool Renderer::initialize_buffers() {
    // Create a simple quad for rendering rectangles
    float vertices[] = {
        // positions        // texture coords
         0.5f,  0.5f,     1.0f, 1.0f,   // top right
         0.5f, -0.5f,     1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f,     0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f,     0.0f, 1.0f    // top left
    };
    
    unsigned int indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };
    
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    
    // Check if OpenGL objects were created successfully
    if (vao_ == 0 || vbo_ == 0 || ebo_ == 0) {
        std::cerr << "[S1U] Error: Failed to create OpenGL objects!" << std::endl;
        return false;
    }
    
    std::cout << "[S1U] OpenGL objects created: VAO=" << vao_ << ", VBO=" << vbo_ << ", EBO=" << ebo_ << std::endl;
    
    glBindVertexArray(vao_);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Check for OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[S1U] OpenGL error during buffer setup: " << err << std::endl;
        return false;
    }
    
    glBindVertexArray(0);
    
    std::cout << "[S1U] Buffers initialized successfully!" << std::endl;
    return true;
}

void Renderer::begin_frame() {
    std::cout << "[DEBUG] Renderer::begin_frame() - entering" << std::endl;
    
    if (!initialized_ || !window_) {
        std::cout << "[DEBUG] Renderer::begin_frame() - early return, initialized=" << initialized_ << ", window=" << window_ << std::endl;
        return;
    }
    
    // Ensure the OpenGL context is current for this window
    std::cout << "[DEBUG] Renderer::begin_frame() - ensuring context is current" << std::endl;
    glfwMakeContextCurrent(window_);
    std::cout << "[DEBUG] Renderer::begin_frame() - context made current" << std::endl;
    
    std::cout << "[DEBUG] Renderer::begin_frame() - about to call glClear" << std::endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    std::cout << "[DEBUG] Renderer::begin_frame() - glClear completed" << std::endl;
    
    std::cout << "[DEBUG] Renderer::begin_frame() - about to call glClearColor" << std::endl;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    std::cout << "[DEBUG] Renderer::begin_frame() - glClearColor completed" << std::endl;
    
    draw_calls_ = 0;
    std::cout << "[DEBUG] Renderer::begin_frame() - completed successfully" << std::endl;
}

void Renderer::end_frame() {
    if (!initialized_) return;
    // Nothing needed for modern OpenGL
}

void Renderer::present() {
    if (!initialized_ || !window_) return;
    glfwSwapBuffers(window_);
}

void Renderer::clear(const Color& color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::draw_rect(const Rect& rect, const Color& color) {
    if (!initialized_) return;
    
    if (shader_program_ == 0) {
        std::cerr << "[S1U] Error: shader_program_ is 0!" << std::endl;
        return;
    }
    
    glUseProgram(shader_program_);
    
    // Set up projection matrix (orthographic for 2D)
    glm::mat4 projection = glm::ortho(0.0f, (float)window_width_, (float)window_height_, 0.0f, -1.0f, 1.0f);
    
    // Set up model matrix for this rectangle
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(rect.width, rect.height, 1.0f));
    
    // Get uniform locations with error checking
    GLint proj_loc = glGetUniformLocation(shader_program_, "uProjection");
    GLint model_loc = glGetUniformLocation(shader_program_, "uModel");
    GLint color_loc = glGetUniformLocation(shader_program_, "uColor");
    GLint opacity_loc = glGetUniformLocation(shader_program_, "uOpacity");
    
    if (proj_loc == -1 || model_loc == -1 || color_loc == -1 || opacity_loc == -1) {
        std::cerr << "[S1U] Error: Failed to get uniform locations!" << std::endl;
        return;
    }
    
    // Set uniforms
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4f(color_loc, color.r, color.g, color.b, color.a);
    glUniform1f(opacity_loc, 1.0f);
    
    // Check for OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[S1U] OpenGL error before draw: " << err << std::endl;
        return;
    }
    
    // Draw
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Check for OpenGL errors after draw
    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "[S1U] OpenGL error after draw: " << err << std::endl;
    }
    
    draw_calls_++;
}

void Renderer::draw_glass_rect(const Rect& rect, const Color& color, float opacity, float blur) {
    if (!initialized_) return;
    
    glUseProgram(shader_program_);
    
    // Set up projection matrix (orthographic for 2D)
    glm::mat4 projection = glm::ortho(0.0f, (float)window_width_, (float)window_height_, 0.0f, -1.0f, 1.0f);
    
    // Set up model matrix for this rectangle
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(rect.x + rect.width * 0.5f, rect.y + rect.height * 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(rect.width, rect.height, 1.0f));
    
    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform4f(glGetUniformLocation(shader_program_, "uColor"), color.r, color.g, color.b, color.a);
    glUniform1f(glGetUniformLocation(shader_program_, "uOpacity"), opacity);
    
    // Draw
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    draw_calls_++;
}

void Renderer::draw_rect_outline(const Rect& rect, const Color& color, float thickness) {
    if (!initialized_) return;
    
    // Draw four thin rectangles for the outline
    // Top edge
    draw_rect(Rect{rect.x, rect.y, rect.width, thickness}, color);
    // Bottom edge
    draw_rect(Rect{rect.x, rect.y + rect.height - thickness, rect.width, thickness}, color);
    // Left edge
    draw_rect(Rect{rect.x, rect.y, thickness, rect.height}, color);
    // Right edge
    draw_rect(Rect{rect.x + rect.width - thickness, rect.y, thickness, rect.height}, color);
}

void Renderer::draw_text(const std::string& text, const Point& position, const Color& color, float size) {
    if (!initialized_) return;
    
    // Simple text rendering using small rectangles for each character
    float char_width = size * 0.6f;
    float char_height = size;
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        if (c == ' ') continue;
        
        Rect char_rect;
        char_rect.x = position.x + i * char_width;
        char_rect.y = position.y;
        char_rect.width = char_width * 0.8f;
        char_rect.height = char_height * 0.8f;
        
        draw_rect(char_rect, color);
    }
}

void Renderer::set_window_size(uint32_t width, uint32_t height) {
    if (window_) {
        glfwSetWindowSize(window_, width, height);
        window_width_ = width;
        window_height_ = height;
        glViewport(0, 0, width, height);
    }
}

Size Renderer::get_window_size() const {
    return Size(window_width_, window_height_);
}

bool Renderer::should_close() const {
    return window_ ? glfwWindowShouldClose(window_) : true;
}

void Renderer::set_vsync(bool enabled) {
    vsync_enabled_ = enabled;
    if (window_) {
        glfwSwapInterval(enabled ? 1 : 0);
    }
}

void Renderer::set_viewport(const Rect& viewport) {
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
}

void Renderer::enable_glass_theme(bool enable, float opacity, float blur, float border, float highlight) {
    // This is handled in draw_glass_rect now
}

} // namespace s1u
