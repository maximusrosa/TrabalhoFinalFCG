#ifndef THE_GAME_H
#define THE_GAME_H

#include <iostream>
#include <string>
#include <memory>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "utils/math_utils.h"
#include "core/gameobject.h"
#include "graphics/objmodel.h"
#include "graphics/renderer.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "physics/bounding.h"
#include "physics/collisions.h"

class Game {
public:
    Game(Game const&) = delete;
    Game& operator=(Game const&) = delete;

    static std::shared_ptr<Game> getInstance(const std::string& title, int width=800, int height=600) {
        static std::shared_ptr<Game> instance{new Game(title, width, height)};
        return instance;
    }

    void run();

    void createWindow(const std::string& title, int width, int height);
    virtual void keyCallback(int key, int scancode, int actions, int mods);
    virtual void mouseButtonCallback(int button, int action, int mods);
    virtual void cursorPosCallback(double xpos, double ypos);
    virtual void framebufferSizeCallback(int width, int height);

    ~Game();

private:
    Game(const std::string& title, int width, int height) {
        normalWindowWidth = width;
        normalWindowHeight = height;

        createWindow(title, width, height);

        videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        fullScreenWidth = videoMode->width;
        fullScreenHeight = videoMode->height;

        // Make window centered
        windowX = (fullScreenWidth - width) / 2;
        windowY = (fullScreenHeight - height) / 2;

        normalWindowRatio = (float)width / height;
        screenRatio = (float)fullScreenWidth / fullScreenHeight;
    }

    GLFWwindow* window;
    const GLFWvidmode* videoMode;
    std::map<std::string, GameObject*> virtualScene;
    GLuint gpuProgramId = 0;

    int normalWindowWidth;
    int normalWindowHeight;
    int windowX;
    int windowY;
    int fullScreenWidth;
    int fullScreenHeight;
    float normalWindowRatio;
    float screenRatio;
    bool fullScreen = false;

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

    float mouseSensitivityX = 0.005f;
    float mouseSensitivityY = 0.005f;

    float nearPlane = -0.1f;
    float farPlane = -100.0f;
    float fov = M_PI / 3.0f;

    GLint modelUniform;
    GLint viewUniform;
    GLint projectionUniform;
    GLint objectIdUniform;
    GLint interpolationTypeUniform;

    void gameLoop();

    static void keyCallback(GLFWwindow* window, int key, int scancode,
                            int actions, int mods) {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->keyCallback(key, scancode, actions, mods);
    }

    static void mouseButtonCallback(GLFWwindow* window, int button, 
                                    int action, int mods) {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->mouseButtonCallback(button, action, mods);
    }

    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->cursorPosCallback(xpos, ypos);
    }

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->framebufferSizeCallback(width, height);
    }

    static void errorCallback(int error, const char* description) {
        fprintf(stderr, "Error: %s\n", description);
    }
};

#endif // THE_GAME_H