#include "graphics/shaders.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "core/game.h"
#include "graphics/core.h"

void LoadShadersFromFiles(GLuint& gpuProgramId, UniformMap& uniforms) 
{
    GLuint vertex_shader_id = LoadShader_Vertex("../../assets/shaders/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../assets/shaders/shader_fragment.glsl");

    if (gpuProgramId != 0)
        glDeleteProgram(gpuProgramId);
    
    gpuProgramId = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Defined in "shader_vertex.glsl" and "shader_fragment.glsl".
    uniforms["model"] = glGetUniformLocation(gpuProgramId, "model");
    uniforms["view"] = glGetUniformLocation(gpuProgramId, "view");
    uniforms["projection"] = glGetUniformLocation(gpuProgramId, "projection");
    uniforms["object_id"] = glGetUniformLocation(gpuProgramId, "object_id");
    uniforms["interpolation_type"] = glGetUniformLocation(gpuProgramId, "interpolation_type");
    uniforms["bbox_min"] = glGetUniformLocation(gpuProgramId, "bbox_min");
    uniforms["bbox_max"] = glGetUniformLocation(gpuProgramId, "bbox_max");

    uniforms["wall_texture"] = glGetUniformLocation(gpuProgramId, "wall_texture");

    // Variables in "shader_fragment.glsl" for accessing textures.
    glUseProgram(gpuProgramId);
    glUniform1i(uniforms["wall_texture"], 0);
    glUseProgram(0);
}

GLuint LoadShader_Vertex(const char* filename)
{
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    LoadShader(filename, vertex_shader_id);
    return vertex_shader_id;
}

GLuint LoadShader_Fragment(const char* filename)
{
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    LoadShader(filename, fragment_shader_id);
    return fragment_shader_id;
}

void LoadShader(const char* filename, GLuint shader_id)
{
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);
    glCompileShader(shader_id);

    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    if ( log_length != 0 )
    {
        std::string output;
        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        fprintf(stderr, "%s", output.c_str());
    }
    delete [] log;
}