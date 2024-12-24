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

#include <utils/math_utils.h>
#include <core/gameobject.h>
#include <graphics/objmodel.h>
#include <graphics/renderer.h>
#include <graphics/shaders.h>
#include <graphics/core.h>
#include <physics/bbox.h>
#include <physics/collisions.h>
#include <core/game.h>

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
        if (key == GLFW_KEY_W || key == GLFW_KEY_S || 
            key == GLFW_KEY_A || key == GLFW_KEY_D) {
            glm::vec4 displacement;
            if (key == GLFW_KEY_W) {
                displacement = cameraView;
            }
            if (key == GLFW_KEY_S) {
                displacement = -cameraView;
            }
            if (key == GLFW_KEY_A) {
                displacement = -cameraRight;
            }
            if (key == GLFW_KEY_D) {
                displacement = cameraRight;
            }
            displacement.y = 0;
            displacement = normalize(displacement);
            
            if (mods & GLFW_MOD_SHIFT) {
                displacement *= runningSpeed;
            } else {
                displacement *= walkingSpeed;
            }
            cameraPosition += displacement;
            virtualScene["Cube"]->translate(displacement.x, displacement.y, displacement.z);
            if (checkCollisionWithStaticObjects(virtualScene["Cube"], virtualScene)) {
                cameraPosition -= displacement;
                virtualScene["Cube"]->translate(-displacement.x, -displacement.y, -displacement.z);
            }
        }
        // F11: toggle full screen
        if (key == GLFW_KEY_F11) {
            if (!fullScreen) {
                glfwSetWindowMonitor(window, nullptr, 0, 0,
                                     fullScreenWidth, fullScreenHeight, GLFW_DONT_CARE);
            } else {
                glfwSetWindowMonitor(window, nullptr, windowX, windowY,
                                     normalWindowWidth, normalWindowHeight, GLFW_DONT_CARE);
            }
            fullScreen = !fullScreen;
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
    double viewX = cos(cameraPitch) * sin(cameraYaw);
    double viewY = sin(cameraPitch);
    double viewZ = cos(cameraPitch) * cos(cameraYaw);

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

void Game::gameLoop() {
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(gpuProgramId);

        glm::mat4 view = Matrix_Camera_View(cameraPosition, cameraView, cameraUp);
        glm::mat4 projection = Matrix_Perspective(fov, screenRatio, nearPlane, farPlane);
        glm::mat4 model = Matrix_Identity();
        glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw the cow
        model = Matrix_Translate(4.0f,1.05f,-100.0f)
                * Matrix_Scale(2.0f,2.0f,2.0f);
        glUniformMatrix4fv(modelUniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(objectIdUniform, COW);
        glUniform1i(interpolationTypeUniform, PHONG_INTERPOLATION);
        DrawVirtualObject(virtualScene, "the_cow");

        // Draw the plane
        model = Matrix_Identity();
        glUniformMatrix4fv(modelUniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(objectIdUniform, PLANE);
        glUniform1i(interpolationTypeUniform, PHONG_INTERPOLATION);
        DrawVirtualObject(virtualScene, "Plane01");

        // Draw the maze
        model = Matrix_Identity();
        glUniformMatrix4fv(modelUniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(objectIdUniform, MAZE);
        glUniform1i(interpolationTypeUniform, GOURAUD_INTERPOLATION);
        DrawVirtualObject(virtualScene, "maze");

        // Draw the cube (player)
        model = Matrix_Translate(cameraPosition.x, cameraPosition.y, cameraPosition.z)
                * Matrix_Rotate_Y(-cameraYaw)
                * Matrix_Scale(0.1f, 0.1f, 0.1f);
        glUniformMatrix4fv(modelUniform, 1 , GL_FALSE , glm::value_ptr(model));
        glUniform1i(objectIdUniform, CUBE);
        glUniform1i(interpolationTypeUniform, GOURAUD_INTERPOLATION);
        DrawVirtualObject(virtualScene, "Cube");
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Game::run() {
    if (!window) {
        fprintf(stderr, "ERROR: window is not initialized.\n");
        std::exit(EXIT_FAILURE);
    }
    LoadShadersFromFiles(gpuProgramId, modelUniform, viewUniform, projectionUniform, 
                         objectIdUniform, interpolationTypeUniform);
    
    glm::mat4 model = Matrix_Identity();

    model = Matrix_Translate(4.0f,1.05f,-100.0f)
            * Matrix_Scale(2.0f,2.0f,2.0f);
    ObjModel cowModel("../../assets/models/cow.obj");
    ComputeNormals(&cowModel);
    BuildSceneTriangles(virtualScene, &cowModel, model);

    ObjModel mazeModel("../../assets/models/maze.obj");
    ComputeNormals(&mazeModel);
    BuildSceneTriangles(virtualScene, &mazeModel, Matrix_Identity());

    model = Matrix_Translate(cameraPosition.x, cameraPosition.y, cameraPosition.z)
            * Matrix_Rotate_Y(-cameraYaw)
            * Matrix_Scale(0.1f, 0.1f, 0.1f);
    ObjModel cubeModel("../../assets/models/cube.obj");
    ComputeNormals(&cubeModel);
    BuildSceneTriangles(virtualScene, &cubeModel, model);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

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