#include "s1u/input_manager.hpp"
#include <iostream>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

InputManager::InputManager()
    : glfw_window_(nullptr)
    , mouse_x_(0.0)
    , mouse_y_(0.0)
    , window_focused_(true)
    , mouse_captured_(false)
    , keyboard_captured_(false) {
}

InputManager::~InputManager() {
    shutdown();
}

bool InputManager::initialize() {
    std::cout << "[S1U] Initializing Input Manager..." << std::endl;
    
    // Initialize key states
    for (int key = 0; key < 512; ++key) {
        key_states_[key] = false;
    }
    
    // Initialize mouse button states
    for (int button = 0; button < 8; ++button) {
        mouse_button_states_[static_cast<MouseButton>(button)] = false;
    }
    
    std::cout << "[S1U] Input Manager initialized successfully!" << std::endl;
    return true;
}

void InputManager::shutdown() {
    std::cout << "[S1U] Shutting down Input Manager..." << std::endl;
    
    // Clear callbacks
    callbacks_.clear();
    su1_input_handler_ = nullptr;
    
    glfw_window_ = nullptr;
}

void InputManager::poll_events() {
    if (glfw_window_) {
        glfwPollEvents();
    }
}

void InputManager::process_events() {
    // Process any queued events
    // In a real implementation, this would process events from a queue
}

void InputManager::register_callback(InputEventType type, InputCallback callback) {
    callbacks_[type].push_back(callback);
}

void InputManager::unregister_callback(InputEventType type, InputCallback callback) {
    auto it = callbacks_.find(type);
    if (it != callbacks_.end()) {
        auto& callbacks = it->second;
        // Note: std::function doesn't support direct comparison
        // This is a simplified implementation - in a real system you'd want to use callback IDs
        // For now, we'll just clear all callbacks of this type
        callbacks.clear();
    }
}

bool InputManager::is_key_pressed(KeyCode key) const {
    auto it = key_states_.find(key);
    return (it != key_states_.end()) ? it->second : false;
}

bool InputManager::is_mouse_button_pressed(MouseButton button) const {
    auto it = mouse_button_states_.find(button);
    return (it != mouse_button_states_.end()) ? it->second : false;
}

void InputManager::get_mouse_position(double& x, double& y) const {
    x = mouse_x_;
    y = mouse_y_;
}

bool InputManager::is_window_focused() const {
    return window_focused_;
}

void InputManager::capture_mouse(bool capture) {
    mouse_captured_ = capture;
    if (glfw_window_) {
        glfwSetInputMode(glfw_window_, GLFW_CURSOR, 
            capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

void InputManager::capture_keyboard(bool capture) {
    keyboard_captured_ = capture;
}

void InputManager::set_su1_input_handler(std::function<void(const std::shared_ptr<InputEvent>&)> handler) {
    su1_input_handler_ = handler;
}

void InputManager::forward_event_to_su1(const std::shared_ptr<InputEvent>& event) {
    if (su1_input_handler_) {
        su1_input_handler_(event);
    }
}

void InputManager::setup_glfw_callbacks(GLFWwindow* window) {
    glfw_window_ = window;
    
    if (!window) {
        std::cerr << "[S1U] Cannot setup GLFW callbacks: window is null" << std::endl;
        return;
    }
    
    // Set user pointer to this instance
    glfwSetWindowUserPointer(window, this);
    
    // Setup callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_position_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetWindowPosCallback(window, window_position_callback);
    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetCharCallback(window, char_callback);
    
    std::cout << "[S1U] GLFW callbacks setup successfully!" << std::endl;
}

// GLFW callback functions
void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_key_event(key, scancode, action, mods);
    }
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_mouse_button_event(button, action, mods);
    }
}

void InputManager::mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_mouse_position_event(xpos, ypos);
    }
}

void InputManager::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_mouse_scroll_event(xoffset, yoffset);
    }
}

void InputManager::window_focus_callback(GLFWwindow* window, int focused) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_focus_event(focused);
    }
}

void InputManager::window_size_callback(GLFWwindow* window, int width, int height) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_size_event(width, height);
    }
}

void InputManager::window_position_callback(GLFWwindow* window, int xpos, int ypos) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_position_event(xpos, ypos);
    }
}

void InputManager::window_close_callback(GLFWwindow* window) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_close_event();
    }
}

void InputManager::char_callback(GLFWwindow* window, unsigned int codepoint) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_char_event(codepoint);
    }
}

// Event processing methods
void InputManager::process_key_event(int key, int scancode, int action, int mods) {
    // Update key state
    if (action == GLFW_PRESS) {
        key_states_[key] = true;
    } else if (action == GLFW_RELEASE) {
        key_states_[key] = false;
    }
    
    // Create event
    InputEventType event_type;
    bool repeat = false;
    
    switch (action) {
        case GLFW_PRESS:
            event_type = InputEventType::KeyPress;
            break;
        case GLFW_RELEASE:
            event_type = InputEventType::KeyRelease;
            break;
        case GLFW_REPEAT:
            event_type = InputEventType::KeyRepeat;
            repeat = true;
            break;
        default:
            return;
    }
    
    // Convert GLFW mods to our ModifierKey enum
    ModifierKey modifiers = ModifierKey::None;
    if (mods & GLFW_MOD_SHIFT) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Shift);
    if (mods & GLFW_MOD_CONTROL) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Control);
    if (mods & GLFW_MOD_ALT) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Alt);
    if (mods & GLFW_MOD_SUPER) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Super);
    
    auto event = std::make_shared<KeyEvent>(event_type, key, modifiers, repeat);
    
    // Notify callbacks
    auto it = callbacks_.find(event_type);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
    
    // Handle special keys
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // Toggle mouse capture
        capture_mouse(!mouse_captured_);
    }
}

void InputManager::process_mouse_button_event(int button, int action, int mods) {
    // Update mouse button state
    MouseButton mouse_button = static_cast<MouseButton>(button);
    
    if (action == GLFW_PRESS) {
        mouse_button_states_[mouse_button] = true;
    } else if (action == GLFW_RELEASE) {
        mouse_button_states_[mouse_button] = false;
    }
    
    // Create event
    InputEventType event_type = (action == GLFW_PRESS) ? InputEventType::MouseButtonPress : InputEventType::MouseButtonRelease;
    auto event = std::make_shared<MouseEvent>(event_type, mouse_x_, mouse_y_, mouse_button);
    
    // Notify callbacks
    auto it = callbacks_.find(event_type);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_mouse_position_event(double xpos, double ypos) {
    mouse_x_ = xpos;
    mouse_y_ = ypos;
    
    // Create event
    auto event = std::make_shared<MouseEvent>(InputEventType::MouseMove, xpos, ypos);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::MouseMove);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_mouse_scroll_event(double xoffset, double yoffset) {
    auto event = std::make_shared<MouseScrollEvent>(xoffset, yoffset);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::MouseScroll);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_focus_event(int focused) {
    window_focused_ = (focused == GLFW_TRUE);
    
    InputEventType event_type = focused ? InputEventType::WindowFocus : InputEventType::WindowLostFocus;
    auto event = std::make_shared<WindowEvent>(event_type);
    
    // Notify callbacks
    auto it = callbacks_.find(event_type);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_size_event(int width, int height) {
    auto event = std::make_shared<WindowEvent>(InputEventType::WindowResize, width, height);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::WindowResize);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_position_event(int xpos, int ypos) {
    auto event = std::make_shared<WindowEvent>(InputEventType::WindowMove, 0, 0, xpos, ypos);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::WindowMove);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_close_event() {
    auto event = std::make_shared<WindowEvent>(InputEventType::WindowClose);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::WindowClose);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_char_event(unsigned int codepoint) {
    // Convert codepoint to string
    std::string text;
    if (codepoint < 0x80) {
        text = static_cast<char>(codepoint);
    } else if (codepoint < 0x800) {
        text = static_cast<char>((codepoint >> 6) | 0xC0);
        text += static_cast<char>((codepoint & 0x3F) | 0x80);
    } else if (codepoint < 0x10000) {
        text = static_cast<char>((codepoint >> 12) | 0xE0);
        text += static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
        text += static_cast<char>((codepoint & 0x3F) | 0x80);
    }
    
    auto event = std::make_shared<TextInputEvent>(text);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::TextInput);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

} // namespace s1u

#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

InputManager::InputManager()
    : glfw_window_(nullptr)
    , mouse_x_(0.0)
    , mouse_y_(0.0)
    , window_focused_(true)
    , mouse_captured_(false)
    , keyboard_captured_(false) {
}

InputManager::~InputManager() {
    shutdown();
}

bool InputManager::initialize() {
    std::cout << "[S1U] Initializing Input Manager..." << std::endl;
    
    // Initialize key states
    for (int key = 0; key < 512; ++key) {
        key_states_[key] = false;
    }
    
    // Initialize mouse button states
    for (int button = 0; button < 8; ++button) {
        mouse_button_states_[static_cast<MouseButton>(button)] = false;
    }
    
    std::cout << "[S1U] Input Manager initialized successfully!" << std::endl;
    return true;
}

void InputManager::shutdown() {
    std::cout << "[S1U] Shutting down Input Manager..." << std::endl;
    
    // Clear callbacks
    callbacks_.clear();
    su1_input_handler_ = nullptr;
    
    glfw_window_ = nullptr;
}

void InputManager::poll_events() {
    if (glfw_window_) {
        glfwPollEvents();
    }
}

void InputManager::process_events() {
    // Process any queued events
    // In a real implementation, this would process events from a queue
}

void InputManager::register_callback(InputEventType type, InputCallback callback) {
    callbacks_[type].push_back(callback);
}

void InputManager::unregister_callback(InputEventType type, InputCallback callback) {
    auto it = callbacks_.find(type);
    if (it != callbacks_.end()) {
        auto& callbacks = it->second;
        // Note: std::function doesn't support direct comparison
        // This is a simplified implementation - in a real system you'd want to use callback IDs
        // For now, we'll just clear all callbacks of this type
        callbacks.clear();
    }
}

bool InputManager::is_key_pressed(KeyCode key) const {
    auto it = key_states_.find(key);
    return (it != key_states_.end()) ? it->second : false;
}

bool InputManager::is_mouse_button_pressed(MouseButton button) const {
    auto it = mouse_button_states_.find(button);
    return (it != mouse_button_states_.end()) ? it->second : false;
}

void InputManager::get_mouse_position(double& x, double& y) const {
    x = mouse_x_;
    y = mouse_y_;
}

bool InputManager::is_window_focused() const {
    return window_focused_;
}

void InputManager::capture_mouse(bool capture) {
    mouse_captured_ = capture;
    if (glfw_window_) {
        glfwSetInputMode(glfw_window_, GLFW_CURSOR, 
            capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

void InputManager::capture_keyboard(bool capture) {
    keyboard_captured_ = capture;
}

void InputManager::set_su1_input_handler(std::function<void(const std::shared_ptr<InputEvent>&)> handler) {
    su1_input_handler_ = handler;
}

void InputManager::forward_event_to_su1(const std::shared_ptr<InputEvent>& event) {
    if (su1_input_handler_) {
        su1_input_handler_(event);
    }
}

void InputManager::setup_glfw_callbacks(GLFWwindow* window) {
    glfw_window_ = window;
    
    if (!window) {
        std::cerr << "[S1U] Cannot setup GLFW callbacks: window is null" << std::endl;
        return;
    }
    
    // Set user pointer to this instance
    glfwSetWindowUserPointer(window, this);
    
    // Setup callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_position_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetWindowPosCallback(window, window_position_callback);
    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetCharCallback(window, char_callback);
    
    std::cout << "[S1U] GLFW callbacks setup successfully!" << std::endl;
}

// GLFW callback functions
void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_key_event(key, scancode, action, mods);
    }
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_mouse_button_event(button, action, mods);
    }
}

void InputManager::mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_mouse_position_event(xpos, ypos);
    }
}

void InputManager::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_mouse_scroll_event(xoffset, yoffset);
    }
}

void InputManager::window_focus_callback(GLFWwindow* window, int focused) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_focus_event(focused);
    }
}

void InputManager::window_size_callback(GLFWwindow* window, int width, int height) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_size_event(width, height);
    }
}

void InputManager::window_position_callback(GLFWwindow* window, int xpos, int ypos) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_position_event(xpos, ypos);
    }
}

void InputManager::window_close_callback(GLFWwindow* window) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_close_event();
    }
}

void InputManager::char_callback(GLFWwindow* window, unsigned int codepoint) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_char_event(codepoint);
    }
}

// Event processing methods
void InputManager::process_key_event(int key, int scancode, int action, int mods) {
    // Update key state
    if (action == GLFW_PRESS) {
        key_states_[key] = true;
    } else if (action == GLFW_RELEASE) {
        key_states_[key] = false;
    }
    
    // Create event
    InputEventType event_type;
    bool repeat = false;
    
    switch (action) {
        case GLFW_PRESS:
            event_type = InputEventType::KeyPress;
            break;
        case GLFW_RELEASE:
            event_type = InputEventType::KeyRelease;
            break;
        case GLFW_REPEAT:
            event_type = InputEventType::KeyRepeat;
            repeat = true;
            break;
        default:
            return;
    }
    
    // Convert GLFW mods to our ModifierKey enum
    ModifierKey modifiers = ModifierKey::None;
    if (mods & GLFW_MOD_SHIFT) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Shift);
    if (mods & GLFW_MOD_CONTROL) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Control);
    if (mods & GLFW_MOD_ALT) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Alt);
    if (mods & GLFW_MOD_SUPER) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Super);
    
    auto event = std::make_shared<KeyEvent>(event_type, key, modifiers, repeat);
    
    // Notify callbacks
    auto it = callbacks_.find(event_type);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
    
    // Handle special keys
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // Toggle mouse capture
        capture_mouse(!mouse_captured_);
    }
}

void InputManager::process_mouse_button_event(int button, int action, int mods) {
    // Update mouse button state
    MouseButton mouse_button = static_cast<MouseButton>(button);
    
    if (action == GLFW_PRESS) {
        mouse_button_states_[mouse_button] = true;
    } else if (action == GLFW_RELEASE) {
        mouse_button_states_[mouse_button] = false;
    }
    
    // Create event
    InputEventType event_type = (action == GLFW_PRESS) ? InputEventType::MouseButtonPress : InputEventType::MouseButtonRelease;
    auto event = std::make_shared<MouseEvent>(event_type, mouse_x_, mouse_y_, mouse_button);
    
    // Notify callbacks
    auto it = callbacks_.find(event_type);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_mouse_position_event(double xpos, double ypos) {
    mouse_x_ = xpos;
    mouse_y_ = ypos;
    
    // Create event
    auto event = std::make_shared<MouseEvent>(InputEventType::MouseMove, xpos, ypos);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::MouseMove);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_mouse_scroll_event(double xoffset, double yoffset) {
    auto event = std::make_shared<MouseScrollEvent>(xoffset, yoffset);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::MouseScroll);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_focus_event(int focused) {
    window_focused_ = (focused == GLFW_TRUE);
    
    InputEventType event_type = focused ? InputEventType::WindowFocus : InputEventType::WindowLostFocus;
    auto event = std::make_shared<WindowEvent>(event_type);
    
    // Notify callbacks
    auto it = callbacks_.find(event_type);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_size_event(int width, int height) {
    auto event = std::make_shared<WindowEvent>(InputEventType::WindowResize, width, height);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::WindowResize);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_position_event(int xpos, int ypos) {
    auto event = std::make_shared<WindowEvent>(InputEventType::WindowMove, 0, 0, xpos, ypos);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::WindowMove);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_close_event() {
    auto event = std::make_shared<WindowEvent>(InputEventType::WindowClose);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::WindowClose);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_char_event(unsigned int codepoint) {
    // Convert codepoint to string
    std::string text;
    if (codepoint < 0x80) {
        text = static_cast<char>(codepoint);
    } else if (codepoint < 0x800) {
        text = static_cast<char>((codepoint >> 6) | 0xC0);
        text += static_cast<char>((codepoint & 0x3F) | 0x80);
    } else if (codepoint < 0x10000) {
        text = static_cast<char>((codepoint >> 12) | 0xE0);
        text += static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
        text += static_cast<char>((codepoint & 0x3F) | 0x80);
    }
    
    auto event = std::make_shared<TextInputEvent>(text);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::TextInput);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

} // namespace s1u

#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace s1u {

InputManager::InputManager()
    : glfw_window_(nullptr)
    , mouse_x_(0.0)
    , mouse_y_(0.0)
    , window_focused_(true)
    , mouse_captured_(false)
    , keyboard_captured_(false) {
}

InputManager::~InputManager() {
    shutdown();
}

bool InputManager::initialize() {
    std::cout << "[S1U] Initializing Input Manager..." << std::endl;
    
    // Initialize key states
    for (int key = 0; key < 512; ++key) {
        key_states_[key] = false;
    }
    
    // Initialize mouse button states
    for (int button = 0; button < 8; ++button) {
        mouse_button_states_[static_cast<MouseButton>(button)] = false;
    }
    
    std::cout << "[S1U] Input Manager initialized successfully!" << std::endl;
    return true;
}

void InputManager::shutdown() {
    std::cout << "[S1U] Shutting down Input Manager..." << std::endl;
    
    // Clear callbacks
    callbacks_.clear();
    su1_input_handler_ = nullptr;
    
    glfw_window_ = nullptr;
}

void InputManager::poll_events() {
    if (glfw_window_) {
        glfwPollEvents();
    }
}

void InputManager::process_events() {
    // Process any queued events
    // In a real implementation, this would process events from a queue
}

void InputManager::register_callback(InputEventType type, InputCallback callback) {
    callbacks_[type].push_back(callback);
}

void InputManager::unregister_callback(InputEventType type, InputCallback callback) {
    auto it = callbacks_.find(type);
    if (it != callbacks_.end()) {
        auto& callbacks = it->second;
        // Note: std::function doesn't support direct comparison
        // This is a simplified implementation - in a real system you'd want to use callback IDs
        // For now, we'll just clear all callbacks of this type
        callbacks.clear();
    }
}

bool InputManager::is_key_pressed(KeyCode key) const {
    auto it = key_states_.find(key);
    return (it != key_states_.end()) ? it->second : false;
}

bool InputManager::is_mouse_button_pressed(MouseButton button) const {
    auto it = mouse_button_states_.find(button);
    return (it != mouse_button_states_.end()) ? it->second : false;
}

void InputManager::get_mouse_position(double& x, double& y) const {
    x = mouse_x_;
    y = mouse_y_;
}

bool InputManager::is_window_focused() const {
    return window_focused_;
}

void InputManager::capture_mouse(bool capture) {
    mouse_captured_ = capture;
    if (glfw_window_) {
        glfwSetInputMode(glfw_window_, GLFW_CURSOR, 
            capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

void InputManager::capture_keyboard(bool capture) {
    keyboard_captured_ = capture;
}

void InputManager::set_su1_input_handler(std::function<void(const std::shared_ptr<InputEvent>&)> handler) {
    su1_input_handler_ = handler;
}

void InputManager::forward_event_to_su1(const std::shared_ptr<InputEvent>& event) {
    if (su1_input_handler_) {
        su1_input_handler_(event);
    }
}

void InputManager::setup_glfw_callbacks(GLFWwindow* window) {
    glfw_window_ = window;
    
    if (!window) {
        std::cerr << "[S1U] Cannot setup GLFW callbacks: window is null" << std::endl;
        return;
    }
    
    // Set user pointer to this instance
    glfwSetWindowUserPointer(window, this);
    
    // Setup callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_position_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetWindowPosCallback(window, window_position_callback);
    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetCharCallback(window, char_callback);
    
    std::cout << "[S1U] GLFW callbacks setup successfully!" << std::endl;
}

// GLFW callback functions
void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_key_event(key, scancode, action, mods);
    }
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_mouse_button_event(button, action, mods);
    }
}

void InputManager::mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_mouse_position_event(xpos, ypos);
    }
}

void InputManager::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_mouse_scroll_event(xoffset, yoffset);
    }
}

void InputManager::window_focus_callback(GLFWwindow* window, int focused) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_focus_event(focused);
    }
}

void InputManager::window_size_callback(GLFWwindow* window, int width, int height) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_size_event(width, height);
    }
}

void InputManager::window_position_callback(GLFWwindow* window, int xpos, int ypos) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_position_event(xpos, ypos);
    }
}

void InputManager::window_close_callback(GLFWwindow* window) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_window_close_event();
    }
}

void InputManager::char_callback(GLFWwindow* window, unsigned int codepoint) {
    InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (manager) {
        manager->process_char_event(codepoint);
    }
}

// Event processing methods
void InputManager::process_key_event(int key, int scancode, int action, int mods) {
    // Update key state
    if (action == GLFW_PRESS) {
        key_states_[key] = true;
    } else if (action == GLFW_RELEASE) {
        key_states_[key] = false;
    }
    
    // Create event
    InputEventType event_type;
    bool repeat = false;
    
    switch (action) {
        case GLFW_PRESS:
            event_type = InputEventType::KeyPress;
            break;
        case GLFW_RELEASE:
            event_type = InputEventType::KeyRelease;
            break;
        case GLFW_REPEAT:
            event_type = InputEventType::KeyRepeat;
            repeat = true;
            break;
        default:
            return;
    }
    
    // Convert GLFW mods to our ModifierKey enum
    ModifierKey modifiers = ModifierKey::None;
    if (mods & GLFW_MOD_SHIFT) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Shift);
    if (mods & GLFW_MOD_CONTROL) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Control);
    if (mods & GLFW_MOD_ALT) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Alt);
    if (mods & GLFW_MOD_SUPER) modifiers = static_cast<ModifierKey>(modifiers | ModifierKey::Super);
    
    auto event = std::make_shared<KeyEvent>(event_type, key, modifiers, repeat);
    
    // Notify callbacks
    auto it = callbacks_.find(event_type);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
    
    // Handle special keys
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // Toggle mouse capture
        capture_mouse(!mouse_captured_);
    }
}

void InputManager::process_mouse_button_event(int button, int action, int mods) {
    // Update mouse button state
    MouseButton mouse_button = static_cast<MouseButton>(button);
    
    if (action == GLFW_PRESS) {
        mouse_button_states_[mouse_button] = true;
    } else if (action == GLFW_RELEASE) {
        mouse_button_states_[mouse_button] = false;
    }
    
    // Create event
    InputEventType event_type = (action == GLFW_PRESS) ? InputEventType::MouseButtonPress : InputEventType::MouseButtonRelease;
    auto event = std::make_shared<MouseEvent>(event_type, mouse_x_, mouse_y_, mouse_button);
    
    // Notify callbacks
    auto it = callbacks_.find(event_type);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_mouse_position_event(double xpos, double ypos) {
    mouse_x_ = xpos;
    mouse_y_ = ypos;
    
    // Create event
    auto event = std::make_shared<MouseEvent>(InputEventType::MouseMove, xpos, ypos);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::MouseMove);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_mouse_scroll_event(double xoffset, double yoffset) {
    auto event = std::make_shared<MouseScrollEvent>(xoffset, yoffset);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::MouseScroll);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_focus_event(int focused) {
    window_focused_ = (focused == GLFW_TRUE);
    
    InputEventType event_type = focused ? InputEventType::WindowFocus : InputEventType::WindowLostFocus;
    auto event = std::make_shared<WindowEvent>(event_type);
    
    // Notify callbacks
    auto it = callbacks_.find(event_type);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_size_event(int width, int height) {
    auto event = std::make_shared<WindowEvent>(InputEventType::WindowResize, width, height);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::WindowResize);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_position_event(int xpos, int ypos) {
    auto event = std::make_shared<WindowEvent>(InputEventType::WindowMove, 0, 0, xpos, ypos);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::WindowMove);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_window_close_event() {
    auto event = std::make_shared<WindowEvent>(InputEventType::WindowClose);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::WindowClose);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

void InputManager::process_char_event(unsigned int codepoint) {
    // Convert codepoint to string
    std::string text;
    if (codepoint < 0x80) {
        text = static_cast<char>(codepoint);
    } else if (codepoint < 0x800) {
        text = static_cast<char>((codepoint >> 6) | 0xC0);
        text += static_cast<char>((codepoint & 0x3F) | 0x80);
    } else if (codepoint < 0x10000) {
        text = static_cast<char>((codepoint >> 12) | 0xE0);
        text += static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
        text += static_cast<char>((codepoint & 0x3F) | 0x80);
    }
    
    auto event = std::make_shared<TextInputEvent>(text);
    
    // Notify callbacks
    auto it = callbacks_.find(InputEventType::TextInput);
    if (it != callbacks_.end()) {
        for (auto& callback : it->second) {
            callback(event);
        }
    }
    
    // Forward to SU1 if handler is set
    forward_event_to_su1(event);
}

} // namespace s1u
