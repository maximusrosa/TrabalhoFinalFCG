#ifndef THE_GAME_H
#define THE_GAME_H

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <glm/mat4x4.hpp>

#include "utils/math_utils.h"
#include "graphics/objmodel.h"
#include "graphics/renderer.h"
#include "graphics/shaders.h"

class Game
{
public:
    Game() : window(nullptr) {}

    Game(const std::string& title, int width, int height);
    Game(const std::string& title);

    void run();

    void createWindow(const std::string& title, int width, int height);
    virtual void keyCallback(int key, int scancode, int actions, int mods);
    virtual void mouseButtonCallback(int button, int action, int mods);
    virtual void cursorPosCallback(double xpos, double ypos);
    virtual void framebufferSizeCallback(int width, int height);

    ~Game();

private:
    GLFWwindow* window;

    std::map<std::string, SceneObject> virtualScene;

    GLuint gpuProgramId = 0;

    float screenRatio = 1.0f;

    glm::vec4 cameraPosition = glm::vec4(0.0f, 2.0f, 0.0f, 1.0f);
    glm::vec4 cameraLookAt = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 cameraView = cameraLookAt - cameraPosition;
    glm::vec4 cameraUp = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 cameraRight = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

    float walkingSpeed = 0.1f;
    float runningSpeed = 0.175f;

    float cameraYaw = 0.0f;
    float cameraPitch = 0.0f;

    bool leftMouseButtonPressed = false;

    float mouseSensitivityX = 0.01f;
    float mouseSensitivityY = 0.01f;

    float nearPlane = -0.1f;
    float farPlane = -100.0f;
    float fov = M_PI / 3.0f;

    GLint modelUniform;
    GLint viewUniform;
    GLint projectionUniform;
    GLint objectIdUniform;

    void gameLoop();

    static void keyCallback(GLFWwindow* window, int key, int scancode, int actions, int mods)
    {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->keyCallback(key, scancode, actions, mods);
    }

    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->mouseButtonCallback(button, action, mods);
    }

    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->cursorPosCallback(xpos, ypos);
    }

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->framebufferSizeCallback(width, height);
    }

    static void errorCallback(int error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }
};

#endif // THE_GAME_H