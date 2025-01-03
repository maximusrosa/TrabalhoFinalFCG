#include "../include/physics/animations.h"

// função animation recebe uma lista de matrizes e retorna a matriz resultante da multiplicação de todas as matrizes da lista

glm::mat4 animation(const std::vector<glm::mat4>& transformations) {
    glm::mat4 result = glm::mat4(1.0f);

    for (const auto & transformation : transformations) {
        result = result * transformation;
    }
    return result;
}