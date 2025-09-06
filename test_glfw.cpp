#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    std::cout << "Testing basic GLFW..." << std::endl;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW initialized successfully" << std::endl;
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    std::cout << "GLFW window created successfully" << std::endl;
    
    glfwMakeContextCurrent(window);
    glfwShowWindow(window);
    
    std::cout << "Window should be visible now. Press any key to continue..." << std::endl;
    
    // Simple render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Red background
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Check for key press
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    std::cout << "Test completed" << std::endl;
    return 0;
}

#include <iostream>

int main() {
    std::cout << "Testing basic GLFW..." << std::endl;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW initialized successfully" << std::endl;
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    std::cout << "GLFW window created successfully" << std::endl;
    
    glfwMakeContextCurrent(window);
    glfwShowWindow(window);
    
    std::cout << "Window should be visible now. Press any key to continue..." << std::endl;
    
    // Simple render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Red background
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Check for key press
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    std::cout << "Test completed" << std::endl;
    return 0;
}

#include <iostream>

int main() {
    std::cout << "Testing basic GLFW..." << std::endl;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW initialized successfully" << std::endl;
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    std::cout << "GLFW window created successfully" << std::endl;
    
    glfwMakeContextCurrent(window);
    glfwShowWindow(window);
    
    std::cout << "Window should be visible now. Press any key to continue..." << std::endl;
    
    // Simple render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Red background
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Check for key press
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    std::cout << "Test completed" << std::endl;
    return 0;
}

