#pragma once
#include "Input.hpp"
#include <mutex>

class GLWidget
{
    GLFWwindow* window{nullptr};
public:
    GLWidget(int width, int height, std::string_view title)
    {
        static std::once_flag gloable_init;
        std::call_once(gloable_init, [&] () 
        {
            glfwInit();
        });
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        window = glfwCreateWindow(width, height, title.data(), NULL, NULL);
        if (!window)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);
        // glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_move_callback);
        glfwSetScrollCallback(window, mouse_scroll_callback);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }
    }

    virtual void application() = 0;
    virtual void render() = 0;

    void render_loop()
    {
        while (!glfwWindowShouldClose(window))
        {
            keyboard_input_callback(window);
            INPUT.update_time();
            render();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    ~GLWidget()
    {
        glfwTerminate();
    }
};