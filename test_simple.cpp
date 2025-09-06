#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    std::cout << "Testing simple GLFW window creation..." << std::endl;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW initialized successfully" << std::endl;
    
    // Create a simple window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Simple Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    std::cout << "GLFW window created successfully" << std::endl;
    
    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Show the window
    glfwShowWindow(window);
    
    std::cout << "Window should be visible now. Press ESC to exit." << std::endl;
    
    // Simple loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Check for ESC key
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
        
        // Small delay
        glfwWaitEventsTimeout(0.016); // ~60 FPS
    }
    
    std::cout << "Cleaning up..." << std::endl;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}

#include <iostream>

int main() {
    std::cout << "Testing simple GLFW window creation..." << std::endl;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW initialized successfully" << std::endl;
    
    // Create a simple window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Simple Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    std::cout << "GLFW window created successfully" << std::endl;
    
    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Show the window
    glfwShowWindow(window);
    
    std::cout << "Window should be visible now. Press ESC to exit." << std::endl;
    
    // Simple loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Check for ESC key
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
        
        // Small delay
        glfwWaitEventsTimeout(0.016); // ~60 FPS
    }
    
    std::cout << "Cleaning up..." << std::endl;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}

#include <iostream>

int main() {
    std::cout << "Testing simple GLFW window creation..." << std::endl;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW initialized successfully" << std::endl;
    
    // Create a simple window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Simple Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    std::cout << "GLFW window created successfully" << std::endl;
    
    // Make the window's context current
    glfwMakeContextCurrent(window);
    
    // Show the window
    glfwShowWindow(window);
    
    std::cout << "Window should be visible now. Press ESC to exit." << std::endl;
    
    // Simple loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Check for ESC key
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
        
        // Small delay
        glfwWaitEventsTimeout(0.016); // ~60 FPS
    }
    
    std::cout << "Cleaning up..." << std::endl;
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}

