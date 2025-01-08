#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
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
#include "graphics/textures.h"
#include "physics/bounding.h"
#include "physics/collisions.h"
#include "physics/animations.h"
#include "utils/file_utils.h"
#include "utils/textrendering.h"

#include "core/game.h"

void Game::createWindow(const std::string& title, int width, int height) {
    int success = glfwInit();
    if (!success) {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(errorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    framebufferSizeCallback(width, height);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Game::keyCallback(int key, int scancode, int actions, int mods) {
    if (key == GLFW_KEY_ESCAPE && actions == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (actions == GLFW_PRESS || actions == GLFW_REPEAT) {
        if (mods & GLFW_MOD_SHIFT) {
            currentSpeed = baseSpeed * speedMultiplier;
        } else {
            currentSpeed = baseSpeed;
        }

        if (key == GLFW_KEY_W) {
            float speed;
            if (actions == GLFW_REPEAT) {
                speed = currentSpeed * deltaTime;
            } else {
                speed = deltaTime;
            }
            glm::vec4 offset = speed * cameraView;
            offset.y = 0;
            cameraPosition += offset;
            virtualScene["Cube"]->translate(offset.x, offset.y, offset.z);
            if (checkCollisionWithStaticObjects(virtualScene["Cube"], virtualScene)) {
                cameraPosition -= offset;
                virtualScene["Cube"]->translate(-offset.x, -offset.y, -offset.z);
            }
        }
        if (key == GLFW_KEY_S) {
            float speed;
            if (actions == GLFW_REPEAT) {
                speed = currentSpeed * deltaTime;
            } else {
                speed = deltaTime;
            }
            glm::vec4 offset = -speed * cameraView;
            offset.y = 0;
            cameraPosition += offset;
            virtualScene["Cube"]->translate(offset.x, offset.y, offset.z);
            if (checkCollisionWithStaticObjects(virtualScene["Cube"], virtualScene)) {
                cameraPosition -= offset;
                virtualScene["Cube"]->translate(-offset.x, -offset.y, -offset.z);
            }
        }
        if (key == GLFW_KEY_A) {
            float speed;
            if (actions == GLFW_REPEAT) {
                speed = currentSpeed * deltaTime;
            } else {
                speed = deltaTime;
            }
            glm::vec4 offset = -speed * cameraRight;
            offset.y = 0;
            cameraPosition += offset;
            virtualScene["Cube"]->translate(offset.x, offset.y, offset.z);
            if (checkCollisionWithStaticObjects(virtualScene["Cube"], virtualScene)) {
                cameraPosition -= offset;
                virtualScene["Cube"]->translate(-offset.x, -offset.y, -offset.z);
            }
        }
        if (key == GLFW_KEY_D) {
            float speed;
            if (actions == GLFW_REPEAT) {
                speed = currentSpeed * deltaTime;
            } else {
                speed = deltaTime;
            }
            glm::vec4 offset = speed * cameraRight;
            offset.y = 0;
            cameraPosition += offset;
            virtualScene["Cube"]->translate(offset.x, offset.y, offset.z);
            if (checkCollisionWithStaticObjects(virtualScene["Cube"], virtualScene)) {
                cameraPosition -= offset;
                virtualScene["Cube"]->translate(-offset.x, -offset.y, -offset.z);
            }
        }

        // F11 key toggles full screen
        if (actions == GLFW_PRESS && key == GLFW_KEY_F11) {
            fullScreen = !fullScreen;
            if (fullScreen) {
                glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0,
                                     screenWidth, screenHeight, GLFW_DONT_CARE);
            } else {
                glfwSetWindowMonitor(window, nullptr, windowX, windowY,
                                     windowWidth, windowHeight, GLFW_DONT_CARE);
            }
        }
    }
}

void Game::mouseButtonCallback(int button, int action, int mods) {
    static double lastCursorPosX = -1.0, lastCursorPosY = -1.0;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &lastCursorPosX, &lastCursorPosY);
        leftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        leftMouseButtonPressed = false;
    }
}

void Game::cursorPosCallback(double xpos, double ypos) {
    static double lastX = xpos, lastY = ypos;

    // Compute mouse delta (change in position) and update last position
    double deltaX = lastX - xpos;
    double deltaY = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    // Update camera yaw and pitch
    cameraYaw += deltaX * mouseSensitivityX;
    cameraPitch += deltaY * mouseSensitivityY;

    // Clamp pitch to prevent screen flip
    if (cameraPitch > 1.5f) cameraPitch = 1.5f;
    if (cameraPitch < -1.5f) cameraPitch = -1.5f;

    // Wrap around yaw (0 to 2*PI)
    if (cameraYaw > 2 * M_PI) cameraYaw -= 2 * M_PI;
    if (cameraYaw < 0) cameraYaw += 2 * M_PI;

    // Compute new camera view direction
    double viewX = std::cos(cameraPitch) * std::sin(cameraYaw);
    double viewY = std::sin(cameraPitch);
    double viewZ = std::cos(cameraPitch) * std::cos(cameraYaw);

    // Update camera view vector and look at point
    cameraView = glm::vec4(viewX, viewY, viewZ, 0.0f);
    cameraLookAt = cameraPosition + cameraView;

    // Recompute camera right and up vector
    cameraRight = normalize(crossproduct(cameraView, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
    cameraUp = normalize(crossproduct(cameraRight, cameraView));
}

void Game::framebufferSizeCallback(int width, int height) {
    glViewport(0, 0, width, height);
    screenRatio = (float)width / height;
}

void Game::setCameraView() {
    glm::mat4 view = Matrix_Camera_View(cameraPosition, cameraView, cameraUp);
    glUniformMatrix4fv(uniforms.at("view"), 1, GL_FALSE, glm::value_ptr(view));
}

void Game::setProjection() {
    glm::mat4 projection = Matrix_Perspective(fov, screenRatio, nearPlane, farPlane);
    glUniformMatrix4fv(uniforms.at("projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void Game::createModel(const std::string& objFilePath, glm::mat4 model) {
    bool isMaze = objFilePath.find("maze") != std::string::npos;
    bool isCow = objFilePath.find("cow") != std::string::npos;

    if (isMaze) {
        const std::string& mazeModelFolder = objFilePath;

        std::vector<std::string> mazeModelFiles = getFiles(mazeModelFolder);

        for (const auto& mazeModelFile : mazeModelFiles) {
            std::string mazeModelFilePath = mazeModelFolder + mazeModelFile;
            ObjModel mazeModel(mazeModelFilePath.c_str());

            ComputeNormals(&mazeModel);
            BuildSceneTriangles(virtualScene, &mazeModel, Matrix_Identity());
        }
    }
    else {
        ObjModel objModel(objFilePath.c_str());
        ComputeNormals(&objModel);

        if (isCow)
            BuildSceneTriangles(virtualScene, &objModel, model, true);
        else
            BuildSceneTriangles(virtualScene, &objModel, model);
    }
}

void Game::drawCow(glm::mat4 model) {
    glUniformMatrix4fv(uniforms.at("model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniforms.at("object_id"), COW);
    glUniform1i(uniforms.at("interpolation_type"), GOURAUD_INTERPOLATION);
    DrawVirtualObject(const_cast<UniformMap&>(uniforms), virtualScene, "the_cow");
}

void Game::drawPlane(glm::mat4 model) {
    glUniformMatrix4fv(uniforms.at("model"), 1 , GL_FALSE , glm::value_ptr(model));
    glUniform1i(uniforms.at("object_id"), PLANE);
    glUniform1i(uniforms.at("interpolation_type"), PHONG_INTERPOLATION);
    DrawVirtualObject(const_cast<UniformMap&>(uniforms), virtualScene, "the_plane");
}

void Game::drawMaze(glm::mat4 model) {
    for (const auto& [name, obj] : virtualScene) {
        bool isMazePart = name.find("maze") != std::string::npos;

        if (isMazePart) {
            glUniformMatrix4fv(uniforms.at("model"), 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(uniforms.at("object_id"), MAZE);
            glUniform1i(uniforms.at("interpolation_type"), PHONG_INTERPOLATION);
            DrawVirtualObject(const_cast<UniformMap&>(uniforms), virtualScene, name.c_str());
        }
    }
}

void Game::drawChestBase(glm::mat4 model) {
    // Draw the chest base
    glUniformMatrix4fv(uniforms.at("model"), 1 , GL_FALSE , glm::value_ptr(model));
    glUniform1i(uniforms.at("object_id"), CHEST);
    glUniform1i(uniforms.at("interpolation_type"), PHONG_INTERPOLATION);

    DrawVirtualObject(const_cast<UniformMap&>(uniforms), virtualScene, "the_chest");
}

void Game::drawChestLid(glm::mat4 model) {
    glUniformMatrix4fv(uniforms.at("model"), 1 , GL_FALSE , glm::value_ptr(model));
    glUniform1i(uniforms.at("object_id"), CHEST_LID);
    glUniform1i(uniforms.at("interpolation_type"), PHONG_INTERPOLATION);

    DrawVirtualObject(const_cast<UniformMap&>(uniforms), virtualScene, "the_chest_lid");
}

void TextRendering_ShowHelloWorld(GLFWwindow* window)
{
    const char* message = "Hello World";
    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, message, 0.5f - (strlen(message) / 2.0f) * charwidth, 0.5f - lineheight / 2.0f, 1.0f);
}

void Game::gameLoop() {
    float cowRotation = 0.0f;    // Rotação inicial da vaca
    float cowPositionZ = -90.0f; // Posição inicial da vaca no eixo Z
    float cowSpeedZ = 10.0f;     // Velocidade de movimento ao longo do eixo Z

    double lastTime = glfwGetTime();
    double currentTime = lastTime;

    while (!glfwWindowShouldClose(window)) {
        // Update time
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Sets the background color
        initialRendering(0.0f, 0.0f, 0.1f);
        TextRendering_Init();

        TextRendering_ShowHelloWorld(window);

        glUseProgram(gpuProgramId);

        setCameraView();
        setProjection();

        glm::mat4 model = Matrix_Identity();

        cowPositionZ += cowSpeedZ * deltaTime; // Velocidade de movimento
        cowRotation += 10.0f * deltaTime; // Velocidade de rotação

        if (cowPositionZ > 10.0f || cowPositionZ < -90.0f) {
            cowSpeedZ = -cowSpeedZ; // Inverter a direção
        }

        glm::mat4 cowModelAnimated = Matrix_Translate(4.0f, 1.2f, cowPositionZ)
                                     * Matrix_Scale(2.0f, 2.0f, 2.0f)
                                     * Matrix_Rotate_Y(cowRotation);

        glm::mat4 chestBase = Matrix_Translate(4.0f, 0.5f, -20.0f)
                              * Matrix_Rotate_Y(M_PI/2) * Matrix_Scale(0.8f, 0.8f, 0.8f);

        glm::mat4 chestLid = Matrix_Translate(4.0f, 0.6f, -20.0f)
                             * Matrix_Rotate_Y(M_PI/2) * Matrix_Scale(0.8f, 0.8f, 0.8f) * Matrix_Rotate_Z(0.5f);

        drawCow(cowModelAnimated);
        drawPlane(model);
        drawMaze(model);
        drawChestBase(chestBase);
        drawChestLid(chestLid);


/*
        // Draw the cube (player)
        model = Matrix_Translate(cameraPosition.x, cameraPosition.y, cameraPosition.z)
                * Matrix_Rotate_Y(-cameraYaw);
        glUniformMatrix4fv(uniforms["model"], 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(uniforms["object_id"], CUBE);
        glUniform1i(uniforms["interpolation_type"], GOURAUD_INTERPOLATION);
        DrawVirtualObject(uniforms, virtualScene, "Cube");
*/

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Game::run() {
    if (!window) {
        fprintf(stderr, "ERROR: window is not initialized.\n");
        std::exit(EXIT_FAILURE);
    }

    LoadShadersFromFiles(gpuProgramId, uniforms);

    LoadTexturesFromFiles(
        "../../assets/textures", 
        numLoadedTextures, 
        GL_REPEAT
    );

    glm::mat4 model = Matrix_Identity();

                         /* Loading the OBJ models */

    // ----------------------------- COW ----------------------------- //
    model = Matrix_Translate(4.0f,1.2f,-90.0f)
            * Matrix_Scale(2.0f,2.0f,2.0f);

    createModel("../../assets/models/cow.obj", model);


    // ----------------------------- MAZE ----------------------------- //
    model = Matrix_Identity();

    createModel("../../assets/models/maze/", model);

    // ----------------------------- CHEST ----------------------------- //
    model = Matrix_Translate(-80.626f, 1.0f, 5.211f)
            * Matrix_Rotate_Y(M_PI/2);
    
    createModel("../../assets/models/chest.obj", model);
    createModel("../../assets/models/chest_lid.obj", model);

    model = Matrix_Translate(4.0f, 1.0f, -20.0f)
            * Matrix_Rotate_Y(M_PI/2);
    
    createModel("../../assets/models/chest.obj", model);
    createModel("../../assets/models/chest_lid.obj", model);

    // ----------------------------- CUBE (PLAYER) ----------------------------- //
    model = Matrix_Translate(cameraPosition.x, cameraPosition.y, cameraPosition.z)
            * Matrix_Rotate_Y(-cameraYaw);

    createModel("../../assets/models/cube.obj", model);

    setRenderConfig();

    gameLoop();

    glfwTerminate();
}

Game::~Game() {
    glfwDestroyWindow(window);
    for (auto& obj : virtualScene) {
        delete obj.second;
    }
    virtualScene.clear();
}