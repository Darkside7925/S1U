#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <string>
#include <array>
#include <optional>

namespace s1u {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using f32 = float;
using f64 = double;

struct Point {
    i32 x, y;
    Point(i32 x = 0, i32 y = 0) : x(x), y(y) {}
};

struct Size {
    u32 width, height;
    Size(u32 w = 0, u32 h = 0) : width(w), height(h) {}
};

struct Rect {
    i32 x, y;
    u32 width, height;
    Rect(i32 x = 0, i32 y = 0, u32 w = 0, u32 h = 0) : x(x), y(y), width(w), height(h) {}
};

struct Color {
    u8 r, g, b, a;
    Color(u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 255) : r(r), g(g), b(b), a(a) {}
};

enum class EventType : u32 {
    KeyPress = 1,
    KeyRelease = 2,
    MouseMove = 3,
    MousePress = 4,
    MouseRelease = 5,
    WindowCreate = 6,
    WindowDestroy = 7,
    WindowResize = 8,
    WindowMove = 9,
    WindowFocus = 10,
    WindowUnfocus = 11,
    WindowClose = 12,
    WindowMaximize = 13,
    WindowMinimize = 14,
    WindowRestore = 15,
    WindowFullscreen = 16,
    WindowUnfullscreen = 17,
    WindowShow = 18,
    WindowHide = 19,
    WindowRaise = 20,
    WindowLower = 21,
    WindowStack = 22,
    WindowUnstack = 23,
    WindowGrab = 24,
    WindowUngrab = 25,
    WindowSetTitle = 26,
    WindowSetIcon = 27,
    WindowSetSize = 28,
    WindowSetPosition = 29,
    WindowSetOpacity = 30,
    WindowSetVisible = 31,
    WindowSetEnabled = 32,
    WindowSetFocusable = 33,
    WindowSetResizable = 34,
    WindowSetMinimizable = 35,
    WindowSetMaximizable = 36,
    WindowSetClosable = 37,
    WindowSetAlwaysOnTop = 38,
    WindowSetSkipTaskbar = 39,
    WindowSetSkipPager = 40,
    WindowSetUrgent = 41,
    WindowSetModal = 42,
    WindowSetTransient = 43,
    WindowSetParent = 44,
    WindowSetChildren = 45,
    WindowSetGeometry = 46,
    WindowSetConstraints = 47,
    WindowSetHints = 48,
    WindowSetProtocols = 49,
    WindowSetDecorations = 50,
    WindowSetBorders = 51,
    WindowSetShadows = 52,
    WindowSetBlur = 53,
    WindowSetTransparency = 54,
    WindowSetCompositor = 55,
    WindowSetRenderer = 56,
    WindowSetBuffer = 57,
    WindowSetDamage = 58,
    WindowSetOpaque = 59,
    WindowSetInput = 60,
    WindowSetOutput = 61,
    WindowSetCursor = 62,
    WindowSetPointer = 63,
    WindowSetKeyboard = 64,
    WindowSetTouch = 65,
    WindowSetGesture = 66,
    WindowSetDrag = 67,
    WindowSetDrop = 68,
    WindowSetSelection = 69,
    WindowSetClipboard = 70,
    WindowSetDnd = 71,
    WindowSetAccessibility = 72,
    WindowSetIme = 73,
    WindowSetVulkan = 74,
    WindowSetOpenGL = 75,
    WindowSetDirectX = 76,
    WindowSetMetal = 77,
    WindowSetWayland = 78,
    WindowSetX11 = 79,
    WindowSetWin32 = 80,
    WindowSetCocoa = 81,
    WindowSetAndroid = 82,
    WindowSetIOS = 83,
    WindowSetWeb = 84,
    WindowSetEmbedded = 85,
    WindowSetHeadless = 86,
    WindowSetVirtual = 87,
    WindowSetRemote = 88,
    WindowSetNetwork = 89,
    WindowSetStreaming = 90,
    WindowSetRecording = 91,
    WindowSetScreenshot = 92,
    WindowSetVideo = 93,
    WindowSetAudio = 94,
    WindowSetGamepad = 95,
    WindowSetJoystick = 96,
    WindowSetTablet = 97,
    WindowSetPen = 98,
    WindowSetEraser = 99,
    WindowSetStylus = 100
};

struct Event {
    EventType type;
    u64 timestamp;
    u32 window_id;
    u32 data[8];
    Point position;
    Size size;
    Color color;
    std::string text;
};

class Window;
class Display;
class Compositor;
class Renderer;
class InputManager;
class DriverManager;
class ProtocolServer;

using WindowPtr = std::shared_ptr<Window>;
using DisplayPtr = std::shared_ptr<Display>;
using CompositorPtr = std::shared_ptr<Compositor>;
using RendererPtr = std::shared_ptr<Renderer>;
using InputManagerPtr = std::shared_ptr<InputManager>;
using DriverManagerPtr = std::shared_ptr<DriverManager>;
using ProtocolServerPtr = std::shared_ptr<ProtocolServer>;

}
