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

        if (actions == GLFW_PRESS) {
            // F11 key toggles full screen
            if (key == GLFW_KEY_F11) {
                fullScreen = !fullScreen;
                if (fullScreen) {
                    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0,
                                        screenWidth, screenHeight, GLFW_DONT_CARE);
                } else {
                    glfwSetWindowMonitor(window, nullptr, windowX, windowY,
                                        windowWidth, windowHeight, GLFW_DONT_CARE);
                }
            }

            // L key toggles look at mode
            if (key == GLFW_KEY_L) {
                lookAtMode = !lookAtMode;
                cameraLookAt = cowPosition;
                distanceCameraCow = glm::distance(cameraPosition, cowPosition);
            }

            // Space key opens the chest (if the player is close enough)
            if (key == GLFW_KEY_SPACE) {
                for (int i = 0; i < chestCoordinates.size(); i++) {
                    if (glm::distance(glm::vec3(cameraPosition), chestCoordinates[i]) < 5.0f) {
                        chestOpened[i] = true;
                        if (playerLife < maxLife) {
                            playerLife++;
                        }
                        timeStarving = 0.0f;
                    }
                }
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

    if (lookAtMode && distanceCameraCow < distanceCameraCowThreshold) {
        cameraView = normalize(cowPosition - cameraPosition);
    } else {
        // Compute new camera view direction
        double viewX = std::cos(cameraPitch) * std::sin(cameraYaw);
        double viewY = std::sin(cameraPitch);
        double viewZ = std::cos(cameraPitch) * std::cos(cameraYaw);

        // Update camera view vector and look at point
        cameraView = glm::vec4(viewX, viewY, viewZ, 0.0f);
        cameraLookAt = cameraPosition + cameraView;
    }
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
    bool isChestLid = objFilePath.find("chest_lid") != std::string::npos;
    bool isChest = objFilePath.find("chest") != std::string::npos;
    bool isPlayer = objFilePath.find("cube") != std::string::npos;

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

        printf("Creating model: %s\n", objFilePath.c_str());

        if (isChestLid) {
            std::string chestName = "the_chest_lid" + std::to_string(numChests);
            for (tinyobj::shape_t& shape : objModel.shapes) {
                shape.name = chestName;
            }
        } else if (isChest) {
            std::string chestName = "the_chest" + std::to_string(numChests);
            for (tinyobj::shape_t& shape : objModel.shapes) {
                shape.name = chestName;
            }
        }

        if (isChestLid || isChest) 
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

void Game::drawChestBase(glm::mat4 model, int chestIndex) {
    // Draw the chest base
    glUniformMatrix4fv(uniforms.at("model"), 1 , GL_FALSE , glm::value_ptr(model));
    glUniform1i(uniforms.at("object_id"), CHEST);
    glUniform1i(uniforms.at("interpolation_type"), PHONG_INTERPOLATION);

    std::string chestName = "the_chest" + std::to_string(chestIndex);
    DrawVirtualObject(const_cast<UniformMap&>(uniforms), virtualScene, chestName.c_str());
}

void Game::drawChestLid(glm::mat4 model, int chestIndex) {
    glUniformMatrix4fv(uniforms.at("model"), 1 , GL_FALSE , glm::value_ptr(model));
    glUniform1i(uniforms.at("object_id"), CHEST_LID);
    glUniform1i(uniforms.at("interpolation_type"), PHONG_INTERPOLATION);

    std::string chestLidName = "the_chest_lid" + std::to_string(chestIndex);
    DrawVirtualObject(const_cast<UniformMap&>(uniforms), virtualScene, chestLidName.c_str());
}

void Game::renderPlayerLife(GLFWwindow* window) {
    const float scale = 2.5f;

    std::string buffer = "[";

    for (int i = 0; i < playerLife; i++) {
        buffer += "+++";
    }
    for (int i = 0; i < maxLife - playerLife; i++) {
        buffer += "   ";
    }
    buffer += "]";

    float lineheight = TextRendering_LineHeight(window, scale);
    float charwidth = TextRendering_CharWidth(window, scale);

    TextRendering_PrintString(window, buffer, -1.0f, -lineheight - 0.85, scale);
}

void Game::renderVictory(GLFWwindow* window) {
    // Set the screen background color to light blue
    glClearColor(0.55f, 0.55f, 0.9f, 1.0f);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the victory message
    const float scale = 7.0f;

    std::string buffer = "VICTORY!";

    float lineheight = TextRendering_LineHeight(window, scale);
    float charwidth = TextRendering_CharWidth(window, scale);

    int width, height;

    if (fullScreen) {
        glfwGetWindowSize(window, &width, &height);
    } else {
        width = windowWidth;
        height = windowHeight;
    }

    TextRendering_PrintString(
        window, 
        buffer, 
        -(charwidth * buffer.length()) / 2.0f, 
        lineheight, 
        scale
    );

    // Render the message to play again (press R) or exit (press ESC)
    const float exitScale = 3.0f;
    std::string exitBuffer = "Press ESC to exit";

    float exitLineHeight = TextRendering_LineHeight(window, exitScale);
    float exitCharWidth = TextRendering_CharWidth(window, exitScale);

    TextRendering_PrintString(
        window, 
        exitBuffer, 
        -(charwidth * buffer.length()) / 2.0f, 
        lineheight - exitLineHeight - 0.2, 
        exitScale
    );
}

void Game::renderGameOver(GLFWwindow* window) {
        // Set the screen background color to light blue
    glClearColor(0.9f, 0.55f, 0.55f, 1.0f);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the victory message
    const float scale = 7.0f;

    std::string buffer = "GAME OVER";

    float lineheight = TextRendering_LineHeight(window, scale);
    float charwidth = TextRendering_CharWidth(window, scale);

    int width, height;

    if (fullScreen) {
        glfwGetWindowSize(window, &width, &height);
    } else {
        width = windowWidth;
        height = windowHeight;
    }

    TextRendering_PrintString(
        window, 
        buffer, 
        -(charwidth * buffer.length()) / 2.0f, 
        lineheight, 
        scale
    );

    // Render the message to play again (press R) or exit (press ESC)
    const float exitScale = 3.0f;
    std::string exitBuffer = "Press ESC to exit";

    float exitLineHeight = TextRendering_LineHeight(window, exitScale);
    float exitCharWidth = TextRendering_CharWidth(window, exitScale);

    TextRendering_PrintString(
        window, 
        exitBuffer, 
        -(charwidth * buffer.length()) / 2.0f, 
        lineheight - exitLineHeight - 0.2, 
        exitScale
    );
}

// Cálculo da Curva de Bezier 2D
glm::vec4 bezierCurve2D(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t) {
    float x = pow(1-t,3) * p0.x 
              + 3 * t * pow(1-t,2) * p1.x 
              + 3 * (1-t) * pow(t,2) * p2.x 
              + pow(t,3) * p3.x;
    float z = pow(1-t,3) * p0.y 
              + 3 * t * pow(1-t,2) * p1.y 
              + 3 * (1-t) * pow(t,2) * p2.y 
              + pow(t,3)*p3.y;

    return {x, 1.2f, z, 1.0f};
}

// Cálculo da Curva de Bezier 3D
glm::vec4 bezierCurve3D(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t) {
    float x = pow(1-t,3) * p0.x 
              + 3 * t * pow(1-t,2) * p1.x 
              + 3 * pow(t,2) * (1-t) * p2.x 
              + pow(t,3) * p3.x;
    float y = pow(1-t,3) * p0.y 
              + 3 * t * pow(1-t,2) * p1.y 
              + 3 * pow(t,2) * (1-t) * p2.y 
              + pow(t,3) * p3.y;
    float z = pow(1-t,3) * p0.z 
              + 3 * t * pow(1-t,2) * p1.z 
              + 3 * pow(t,2) * (1-t) * p2.z 
              + pow(t,3) * p3.z;
    return {x, y, z, 1.0f};
}

void Game::gameLoop() {
    float chestLidRotation = 0.0f;
    float chestLidZTranslation = 0.0f;
    float chestLidYTranslation = 0.0f;
    
    float t = 0.0f;     // Parâmetro "t" da curva de Bézier
    bool turn = false;  // Controle do sentido (ida ou volta)

    glm::vec2 p0(0.0f, -90.0f); // Ponto inicial
    glm::vec2 p1(2.0f, -92.0f); // Primeiro ponto de controle
    glm::vec2 p2(4.0f, -94.0f); // Segundo ponto de controle
    glm::vec2 p3(6.0f, -96.0f); // Ponto final

    double lastTime = glfwGetTime();
    double currentTime = lastTime;

    glm::vec4 lastCowPosition = cowPosition;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (gameOver) {
            renderGameOver(window);
            glfwSwapBuffers(window);
            continue;
        }
        if (victory) {
            renderVictory(window);
            glfwSwapBuffers(window);
            continue;
        }

        // Atualiza os pontos de controle da curva de Bézier para alternar entre ida e volta
        if (t >= 1.0f) {
            t = 0.0f; // Reinicia o parâmetro "t"

            if (turn) {
                // Caminho de volta
                p0 = glm::vec2(10.0f, -100.0f);
                p1 = glm::vec2(8.0f, -96.0f);
                p2 = glm::vec2(6.0f, -92.0f);
                p3 = glm::vec2(4.0f, -90.0f);
            } else {
                // Caminho de ida
                p0 = glm::vec2(4.0f, -90.0f);
                p1 = glm::vec2(6.0f, -92.0f);
                p2 = glm::vec2(8.0f, -96.0f);
                p3 = glm::vec2(10.0f, -100.0f);
            }
            turn = !turn; // Alterna a direção
        }

        // Atualiza a posição da vaca
        lastCowPosition = cowPosition;
        cowPosition = bezierCurve2D(p0, p1, p2, p3, t);

        // Update time
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Atualiza o parâmetro "t" da curva de Bézier
        t += 0.1f * deltaTime;

        // Atualiza AABB e bounding sphere da vaca
        glm::vec4 cowTranslation = cowPosition - lastCowPosition;

        virtualScene["the_cow"]->translate(cowTranslation.x, cowTranslation.y, cowTranslation.z);

        distanceCameraCow = glm::distance(cameraPosition, cowPosition);
        if (lookAtMode && distanceCameraCow < distanceCameraCowThreshold) {
            cameraView = normalize(cowPosition - cameraPosition);
        }

        if (virtualScene["Cube"]->intersects(*virtualScene["the_cow"])) {
            victory = true;
            continue;
        }

        // Sets the background color
        initialRendering(0.0f, 0.0f, 0.1f);
        glUseProgram(gpuProgramId);

        setCameraView();
        setProjection();
        
        glm::mat4 model = Matrix_Identity();

        timeStarving += deltaTime;
        if (timeStarving > starvationLimit) {
            playerLife--;
            timeStarving = 0.0f;
            if (playerLife == 0) {
                gameOver = true;
            }
        }

        glm::mat4 cowModel = Matrix_Translate(cowPosition.x, cowPosition.y, cowPosition.z)
                             * Matrix_Scale(2.0f, 2.0f, 2.0f);

        for (int i = 1; i <= numChests; i++) {
            std::string chestName = "the_chest" + std::to_string(i);
            std::string chestLidName = "the_chest_lid" + std::to_string(i);

            glm::vec4 chestBaseAABBMin = virtualScene[chestName]->getAABB().getMin();
            glm::vec4 chestBaseAABBMax = virtualScene[chestName]->getAABB().getMax();

            glm::vec4 chestLidAABBMin = virtualScene[chestLidName]->getAABB().getMin();
            glm::vec4 chestLidAABBMax = virtualScene[chestLidName]->getAABB().getMax();

            float chestDepth = chestBaseAABBMax.z - chestBaseAABBMin.z;
            float chestBaseHeight = chestBaseAABBMax.y - chestBaseAABBMin.y;
            float chestLidHeight = chestLidAABBMax.y - chestLidAABBMin.y;

            // Desloca o baú de modo que a dobradiça fique no centro
            float chestLidOffsetX = chestDepth / 2.0f;
            float chestLidOffsetY = (chestBaseHeight - 2.0f * chestLidHeight) / 2.0f;
            
            if (chestLidRotation < M_PI_2) {   // Abrir até 90 graus
                chestLidRotation += deltaTime; // Velocidade de abertura
            }

            glm::vec3 coord = chestCoordinates[i-1];
            glm::mat4 chestBaseModel = Matrix_Translate(coord.x, coord.y, coord.z);

            glm::mat4 chestLidModel;

            if (chestOpened[i-1]) {
                chestLidModel = Matrix_Translate(coord.x, coord.y, coord.z)
                              * Matrix_Translate(-chestLidOffsetX, -chestLidOffsetY, 0.0f)
                              * Matrix_Rotate_Z(chestLidRotation)
                              * Matrix_Translate(chestLidOffsetX, chestLidOffsetY, 0.0f);
            } else {
                chestLidModel = Matrix_Translate(coord.x, coord.y, coord.z)
                              * Matrix_Translate(-chestLidOffsetX, -chestLidOffsetY, 0.0f)
                              * Matrix_Translate(chestLidOffsetX, chestLidOffsetY, 0.0f);
            }                     
            drawChestBase(chestBaseModel, i);
            drawChestLid(chestLidModel, i);
        }
        drawCow(cowModel);
        drawPlane(model);
        drawMaze(model);

        renderPlayerLife(window);

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
    for (glm::vec3 coord : chestCoordinates) {
        model = Matrix_Translate(coord.x, coord.y, coord.z);
        numChests++;
        createModel("../../assets/models/chest.obj", model);
        createModel("../../assets/models/chest_lid.obj", model);
    }

    // ----------------------------- CUBE (PLAYER) ----------------------------- //
    model = Matrix_Translate(cameraPosition.x, cameraPosition.y, cameraPosition.z)
            * Matrix_Rotate_Y(-cameraYaw);

    createModel("../../assets/models/cube.obj", model);

    setRenderConfig();

    TextRendering_Init();

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