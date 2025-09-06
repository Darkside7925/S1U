#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <GLFW/glfw3.h>

namespace s1u {

// Forward declarations
class Window;

// Input event types
enum class InputEventType {
    KeyPress,
    KeyRelease,
    KeyRepeat,
    MouseMove,
    MouseButtonPress,
    MouseButtonRelease,
    MouseScroll,
    WindowFocus,
    WindowLostFocus,
    WindowResize,
    WindowMove,
    WindowClose,
    TextInput,
    TouchBegin,
    TouchMove,
    TouchEnd,
    Gesture
};

// Key codes (using GLFW key codes)
using KeyCode = int;

// Mouse button codes
enum class MouseButton {
    Left = GLFW_MOUSE_BUTTON_LEFT,
    Right = GLFW_MOUSE_BUTTON_RIGHT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE,
    Button4 = GLFW_MOUSE_BUTTON_4,
    Button5 = GLFW_MOUSE_BUTTON_5,
    Button6 = GLFW_MOUSE_BUTTON_6,
    Button7 = GLFW_MOUSE_BUTTON_7,
    Button8 = GLFW_MOUSE_BUTTON_8
};

// Modifier keys
enum class ModifierKey {
    None = 0,
    Shift = GLFW_MOD_SHIFT,
    Control = GLFW_MOD_CONTROL,
    Alt = GLFW_MOD_ALT,
    Super = GLFW_MOD_SUPER,
    CapsLock = GLFW_MOD_CAPS_LOCK,
    NumLock = GLFW_MOD_NUM_LOCK
};

// Operator overloads for ModifierKey
inline ModifierKey operator|(ModifierKey a, ModifierKey b) {
    return static_cast<ModifierKey>(static_cast<int>(a) | static_cast<int>(b));
}

inline ModifierKey operator&(ModifierKey a, ModifierKey b) {
    return static_cast<ModifierKey>(static_cast<int>(a) & static_cast<int>(b));
}

inline ModifierKey& operator|=(ModifierKey& a, ModifierKey b) {
    a = static_cast<ModifierKey>(static_cast<int>(a) | static_cast<int>(b));
    return a;
}

// Input event base class
class InputEvent {
public:
    InputEvent(InputEventType type) : type_(type) {}
    virtual ~InputEvent() = default;
    
    InputEventType get_type() const { return type_; }
    
private:
    InputEventType type_;
};

// Key event
class KeyEvent : public InputEvent {
public:
    KeyEvent(InputEventType type, KeyCode key, ModifierKey modifiers, bool repeat = false)
        : InputEvent(type), key_(key), modifiers_(modifiers), repeat_(repeat) {}
    
    KeyCode get_key() const { return key_; }
    ModifierKey get_modifiers() const { return modifiers_; }
    bool is_repeat() const { return repeat_; }
    
private:
    KeyCode key_;
    ModifierKey modifiers_;
    bool repeat_;
};

// Mouse event
class MouseEvent : public InputEvent {
public:
    MouseEvent(InputEventType type, double x, double y, MouseButton button = MouseButton::Left)
        : InputEvent(type), x_(x), y_(y), button_(button) {}
    
    double get_x() const { return x_; }
    double get_y() const { return y_; }
    MouseButton get_button() const { return button_; }
    
private:
    double x_, y_;
    MouseButton button_;
};

// Mouse scroll event
class MouseScrollEvent : public InputEvent {
public:
    MouseScrollEvent(double x_offset, double y_offset)
        : InputEvent(InputEventType::MouseScroll), x_offset_(x_offset), y_offset_(y_offset) {}
    
    double get_x_offset() const { return x_offset_; }
    double get_y_offset() const { return y_offset_; }
    
private:
    double x_offset_, y_offset_;
};

// Window event
class WindowEvent : public InputEvent {
public:
    WindowEvent(InputEventType type, uint32_t width = 0, uint32_t height = 0, int32_t x = 0, int32_t y = 0)
        : InputEvent(type), width_(width), height_(height), x_(x), y_(y) {}
    
    uint32_t get_width() const { return width_; }
    uint32_t get_height() const { return height_; }
    int32_t get_x() const { return x_; }
    int32_t get_y() const { return y_; }
    
private:
    uint32_t width_, height_;
    int32_t x_, y_;
};

// Text input event
class TextInputEvent : public InputEvent {
public:
    TextInputEvent(const std::string& text)
        : InputEvent(InputEventType::TextInput), text_(text) {}
    
    const std::string& get_text() const { return text_; }
    
private:
    std::string text_;
};

// Input callback function type
using InputCallback = std::function<void(const std::shared_ptr<InputEvent>&)>;

// Input manager class
class InputManager {
public:
    InputManager();
    ~InputManager();

    // Initialization
    bool initialize();
    void shutdown();

    // Event handling
    void poll_events();
    void process_events();

    // Callback registration
    void register_callback(InputEventType type, InputCallback callback);
    void unregister_callback(InputEventType type, InputCallback callback);

    // Input state queries
    bool is_key_pressed(KeyCode key) const;
    bool is_mouse_button_pressed(MouseButton button) const;
    void get_mouse_position(double& x, double& y) const;
    bool is_window_focused() const;

    // Input capture
    void capture_mouse(bool capture);
    void capture_keyboard(bool capture);
    bool is_mouse_captured() const { return mouse_captured_; }
    bool is_keyboard_captured() const { return keyboard_captured_; }

    // SU1 integration
    void set_su1_input_handler(std::function<void(const std::shared_ptr<InputEvent>&)> handler);
    void forward_event_to_su1(const std::shared_ptr<InputEvent>& event);

    // GLFW callback setup
    void setup_glfw_callbacks(GLFWwindow* window);

private:
    // GLFW callback functions
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void window_focus_callback(GLFWwindow* window, int focused);
    static void window_size_callback(GLFWwindow* window, int width, int height);
    static void window_position_callback(GLFWwindow* window, int xpos, int ypos);
    static void window_close_callback(GLFWwindow* window);
    static void char_callback(GLFWwindow* window, unsigned int codepoint);

    // Event processing
    void process_key_event(int key, int scancode, int action, int mods);
    void process_mouse_button_event(int button, int action, int mods);
    void process_mouse_position_event(double xpos, double ypos);
    void process_mouse_scroll_event(double xoffset, double yoffset);
    void process_window_focus_event(int focused);
    void process_window_size_event(int width, int height);
    void process_window_position_event(int xpos, int ypos);
    void process_window_close_event();
    void process_char_event(unsigned int codepoint);

    // State tracking
    std::unordered_map<KeyCode, bool> key_states_;
    std::unordered_map<MouseButton, bool> mouse_button_states_;
    double mouse_x_, mouse_y_;
    bool window_focused_;
    bool mouse_captured_;
    bool keyboard_captured_;

    // Callbacks
    std::unordered_map<InputEventType, std::vector<InputCallback>> callbacks_;
    std::function<void(const std::shared_ptr<InputEvent>&)> su1_input_handler_;

    // GLFW window reference
    GLFWwindow* glfw_window_;
};

} // namespace s1u
