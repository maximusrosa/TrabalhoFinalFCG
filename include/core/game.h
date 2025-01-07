#ifndef THE_GAME_H
#define THE_GAME_H

#include <glm/gtc/matrix_transform.hpp>
#include <stack>
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <ctime>
#include <vector>

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
#include "graphics/textures.h"
#include "graphics/core.h"
#include "physics/bounding.h"
#include "physics/collisions.h"
#include "utils/file_utils.h"

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

    void setCameraView();
    void setProjection();

    void createModel(const std::string& objFilePath, glm::mat4 model);

    void drawCow(glm::mat4 model);
    void drawPlane(glm::mat4 model);
    void drawMaze(glm::mat4 model);
    void drawChest(glm::mat4 model);

    ~Game();

private:
    Game(const std::string& title, int width, int height) {
        windowWidth = width;
        windowHeight = height;

        createWindow(title, width, height);

        videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        screenWidth = videoMode->width;
        screenHeight = videoMode->height;

        // Make window centered
        windowX = (screenWidth - width) / 2;
        windowY = (screenHeight - height) / 2;

        windowRatio = (float)width / height;
        screenRatio = (float)screenWidth / screenHeight;
    }

    GLFWwindow* window{};
    const GLFWvidmode* videoMode;

    VirtualScene virtualScene;

    int windowWidth, windowHeight;
    int windowX, windowY;
    int screenWidth, screenHeight;
    float windowRatio;
    float screenRatio;
    bool fullScreen = false;

    glm::vec4 cameraPosition = glm::vec4(4.0f, 2.0f, 0.0f, 1.0f);
    glm::vec4 cameraLookAt = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 cameraView = cameraLookAt - cameraPosition;
    glm::vec4 cameraUp = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 cameraRight = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

    const float baseSpeed = 10.0f;
    const float speedMultiplier = 1.75f;
    float currentSpeed = baseSpeed;

    float cameraYaw = 0.0f;
    float cameraPitch = 0.0f;

    bool leftMouseButtonPressed = false;

    const float mouseSensitivityX = 0.005f;
    const float mouseSensitivityY = 0.005f;

    const float nearPlane = -0.1f;
    const float farPlane = -100.0f;
    const float fov = M_PI / 3.0f;

    float rotation = M_PI;
    float cowPositionZ = -90.0f; // Posição inicial da vaca no eixo Z
    float cowSpeedZ = 10.0f;     // Velocidade de movimento ao longo do eixo Z

    float deltaTime = 0.0f;

    GLuint gpuProgramId = 0;
    GLuint numLoadedTextures = 0;
    UniformMap uniforms = {};

    void gameLoop();

    static void keyCallback(
        GLFWwindow* window, 
        int key, int scancode,
        int actions, int mods
    ) {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->keyCallback(key, scancode, actions, mods);
    }

    static void mouseButtonCallback(
        GLFWwindow* window, int button,
        int action, int mods
    ) {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->mouseButtonCallback(button, action, mods);
    }

    static void cursorPosCallback(
        GLFWwindow* window, 
        double xpos, double ypos
    ) {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->cursorPosCallback(xpos, ypos);
    }

    static void framebufferSizeCallback(
        GLFWwindow* window, 
        int width, int height
    ) {
        Game* obj = static_cast<Game*>(glfwGetWindowUserPointer(window));
        obj->framebufferSizeCallback(width, height);
    }

    static void errorCallback(int error, const char* description) {
        fprintf(stderr, "Error: %s\n", description);
    }

    static void initialRendering(GLfloat R, GLfloat G, GLfloat B) {
        glClearColor(R, G, B, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    static void setRenderConfig() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
};

#endif // THE_GAME_H