#include "graphics/textures.h"
#include "utils/file_utils.h"

/* Loads a texture image and creates an OpenGL texture object. */
void LoadTextureImage(const char* filename, GLuint& numLoadedTextures, GLint wrappingMode)
{
    printf("Loading image \"%s\"... ", filename);

    // Loads image data from disk
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == nullptr )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    printf("OK (%dx%d).\n", width, height);

    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Texture mapping parameters
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, wrappingMode);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, wrappingMode);

    // Texture sampling parameters
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Send texture data to OpenGL
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = numLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    numLoadedTextures += 1;
}

void LoadTexturesFromFiles(const std::string& texturesDirPath) {
    GLuint numLoadedTextures = 0;
    const std::vector<std::string> textureFiles = getFiles(texturesDirPath);

    for (const auto& textureFile : textureFiles) {
        LoadTextureImage((texturesDirPath + "/" + textureFile).c_str(), numLoadedTextures, GL_REPEAT);
    }

    int numTextures = textureFiles.size();

    if (numLoadedTextures != numTextures) {
        fprintf(stderr, "ERROR: Failed to load all texture images.\n");
        std::exit(EXIT_FAILURE);
    }
}