#include "Window.hpp"
#include <iostream>

Window::Window(const char *title, float width, float height)
{
    glfwInit();
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // Remove swap interval in GLFW to avoid framerate cap
    gl3wInit();

    auto resizeCallbackFunction = [](GLFWwindow *window, int width, int height)
    {
        Window *myWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        myWindow->Resize(width, height);
    };

    glfwSetWindowUserPointer(window, reinterpret_cast<void *>(this));
    glfwSetWindowSizeCallback(window, resizeCallbackFunction);
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::Resize(float width, float height)
{
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
}

void Window::UpdateTitle(std::string renderingEngine, float framerate)
{
    std::stringstream title;
    title << "ClosedGL | Using \"" << renderingEngine << "\" Renderer | FPS: " << framerate;
    glfwSetWindowTitle(window, title.str().c_str());
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

float Window::GetHeight()
{
    return height;
}

float Window::GetWidth()
{
    return width;
}
