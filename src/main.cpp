#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// OpenGL headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM headers: OpenGL Mathematics
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// tinyobjloader: load models from OBJ files
#include "tiny_obj_loader.h"

// Local headers 
#include "core/game.h"

int main()
{
    auto game = Game::getInstance("Game", 800, 600);
    game->run();
    return EXIT_SUCCESS;
}