#include "graphics/renderer.h"

#include <cassert>

#include "graphics/core.h"
#include "graphics/objmodel.h"
#include "core/gameobject.h"

void DrawVirtualObject(UniformMap& uniforms, VirtualScene& virtualScene, const char* objectName) {
    GameObject* object = virtualScene[objectName];
    SceneObject sceneObject = object->getSceneObject();

    glBindVertexArray(sceneObject.vertexArrayObjectId);

    glm::vec4 bbox_min = object->getAABB().getMin();
    glm::vec4 bbox_max = object->getAABB().getMax();

    glUniform4f(uniforms["bbox_min"], bbox_min.x, bbox_min.y, bbox_min.z, 1.0);
    glUniform4f(uniforms["bbox_max"], bbox_max.x, bbox_max.y, bbox_max.z, 1.0);

    glDrawElements(
        sceneObject.renderingMode,
        sceneObject.numIndices,
        GL_UNSIGNED_INT,
        (void*)(sceneObject.baseIndex * sizeof(GLuint))
    );

    glBindVertexArray(0);
}

void BuildSceneTriangles(VirtualScene& virtualScene, ObjModel* model, glm::mat4 modelMatrix) {
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float> model_coefficients;
    std::vector<float> normal_coefficients;
    std::vector<float> texture_coefficients;

    for (size_t shape = 0; shape < model->shapes.size(); ++shape) {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle) {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex) {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(first_index + 3*triangle + vertex);

                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                model_coefficients.push_back( vx );
                model_coefficients.push_back( vy );
                model_coefficients.push_back( vz );
                model_coefficients.push_back( 1.0f ); 

                if ( idx.normal_index != -1 ) {
                    const float nx = model->attrib.normals[3*idx.normal_index + 0];
                    const float ny = model->attrib.normals[3*idx.normal_index + 1];
                    const float nz = model->attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx );
                    normal_coefficients.push_back( ny );
                    normal_coefficients.push_back( nz );
                    normal_coefficients.push_back( 0.0f );
                }

                if ( idx.texcoord_index != -1 ) {
                    const float u = model->attrib.texcoords[2*idx.texcoord_index + 0];
                    const float v = model->attrib.texcoords[2*idx.texcoord_index + 1];
                    texture_coefficients.push_back( u );
                    texture_coefficients.push_back( v );
                }
            }
        }
        size_t last_index = indices.size() - 1;

        SceneObject sceneObject;
        sceneObject.name = model->shapes[shape].name;
        sceneObject.baseIndex = first_index;
        sceneObject.numIndices = last_index - first_index + 1;
        sceneObject.renderingMode = GL_TRIANGLES;
        sceneObject.vertexArrayObjectId = vertex_array_object_id;

        GameObject* theobject = new GameObject(*model, sceneObject, modelMatrix);

        virtualScene[model->shapes[shape].name] = theobject;
    }

    GLuint VBO_model_coefficients_id;

    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());

    GLuint location = 0; // "(location = 0)" in "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 in "shader_vertex.glsl"

    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if ( !normal_coefficients.empty() ) {
        GLuint VBO_normal_coefficients_id;

        glGenBuffers(1, &VBO_normal_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());

        location = 1; // "(location = 1)" in "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 in "shader_vertex.glsl"

        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if ( !texture_coefficients.empty() ) {
        GLuint VBO_texture_coefficients_id;

        glGenBuffers(1, &VBO_texture_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());

        location = 2; // "(location = 2)" in "shader_vertex.glsl"
        number_of_dimensions = 2; // vec2 in "shader_vertex.glsl"

        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());

    glBindVertexArray(0);
}

void ComputeNormals(ObjModel* model) {
    if ( !model->attrib.normals.empty() ) {
        return;
    }
    // Using Goraud model for normals
    size_t num_vertices = model->attrib.vertices.size() / 3;

    std::vector<int> num_triangles_per_vertex(num_vertices, 0);
    std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f,0.0f,0.0f,0.0f));

    for (size_t shape = 0; shape < model->shapes.size(); ++shape) {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle) {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            glm::vec4  vertices[3];
            for (size_t vertex = 0; vertex < 3; ++vertex) {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
            }

            const glm::vec4  a = vertices[0];
            const glm::vec4  b = vertices[1];
            const glm::vec4  c = vertices[2];

            // a, b, c defined in counterclockwise order
            const glm::vec4 n = ComputeTriangleNormal(a, c, b);

            for (size_t vertex = 0; vertex < 3; ++vertex) {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                num_triangles_per_vertex[idx.vertex_index] += 1;
                vertex_normals[idx.vertex_index] += n;
                model->shapes[shape].mesh.indices[3*triangle + vertex].normal_index = idx.vertex_index;
            }
        }
    }
    model->attrib.normals.resize( 3*num_vertices );

    for (size_t i = 0; i < vertex_normals.size(); ++i) {
        glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
        n /= norm(n);
        model->attrib.normals[3*i + 0] = n.x;
        model->attrib.normals[3*i + 1] = n.y;
        model->attrib.normals[3*i + 2] = n.z;
    }
}

void PushMatrix(std::stack<glm::mat4>& matrixStack, const glm::mat4& M) {
    matrixStack.push(M);
}

void PopMatrix(std::stack<glm::mat4>& matrixStack, glm::mat4& M) {
    if ( matrixStack.empty() ) {
        M = Matrix_Identity();
    } else {
        M = matrixStack.top();
        matrixStack.pop();
    }
}