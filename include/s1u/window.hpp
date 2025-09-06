#pragma once

#include "core.hpp"

namespace s1u {

// Window class is defined in window_manager.hpp
// WindowState enum is defined in window_manager.hpp

struct WindowAttributes {
    u32 id;
    std::string title;
    Point position;
    Size size;
    Size min_size;
    Size max_size;
    f32 opacity;
    bool visible;
    bool enabled;
    bool focusable;
    bool resizable;
    bool minimizable;
    bool maximizable;
    bool closable;
    bool always_on_top;
    bool skip_taskbar;
    bool skip_pager;
    bool sticky;
    bool modal;
    bool transient;
    bool urgent;
    bool demands_attention;
    bool focused;
    bool grabbed;
    bool shaded;
    bool above;
    bool below;
    bool tooltip;
    bool notification;
    bool dock;
    bool desktop;
    bool menu;
    bool dialog;
    bool splash;
    bool utility;
    bool dropdown;
    bool popup;
    bool toolbar;
    bool status;
    bool progress;
    bool input;
    bool output;
    bool overlay;
    bool background;
    bool cursor;
    bool drag;
    bool drop;
    bool selection;
    bool clipboard;
    bool dnd;
    bool accessibility;
    bool ime;
    bool vulkan;
    bool opengl;
    bool directx;
    bool metal;
    bool wayland;
    bool x11;
    bool win32;
    bool cocoa;
    bool android;
    bool ios;
    bool web;
    bool embedded;
    bool headless;
    bool virtual_window;
    bool remote;
    bool network;
    bool streaming;
    bool recording;
    bool screenshot;
    bool video;
    bool audio;
    bool gamepad;
    bool joystick;
    bool tablet;
    bool pen;
    bool eraser;
    bool stylus;
};

} // namespace s1u
