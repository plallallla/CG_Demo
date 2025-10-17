#pragma once
#include <GLFW/glfw3.h>
#include <cstddef>
#include <glad/glad.h>
#include <iostream>

namespace glHelper
{
    inline GLFWwindow* init(int width, int height, const char* title)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
        GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return nullptr;
        }
        glfwMakeContextCurrent(window);
        auto resize_func = [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); };
        glfwSetFramebufferSizeCallback(window, resize_func);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return nullptr;
        }
        return window;
    }

    inline void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
    }
}