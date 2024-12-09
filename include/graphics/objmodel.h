#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <vector>
#include <string>
#include <stdexcept>

// tinyobjloader: load models from OBJ files
#include <tiny_obj_loader.h>

// Represents a model loaded from an OBJ file
struct ObjModel 
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    // This constructor builds a model from an OBJ file
    // https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath=NULL, bool triangulate=true)
    {
        printf("Loading objects from file \"%s\"...\n", filename);

        // If basepath is NULL, we extract the directory from the filename
        std::string fullpath(filename);
        std::string dirname;
        if (basepath == NULL) 
        {
            auto i = fullpath.find_last_of("/");
            if (i != std::string::npos) 
            {
                dirname = fullpath.substr(0, i+1);
                basepath = dirname.c_str();
            }
        }
        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");

        for (size_t shape = 0; shape < shapes.size(); ++shape) 
        {
            if (shapes[shape].name.empty()) 
            {
                fprintf(stderr, "Error: Unnamed object in OBJ file '%s'.\n", filename);
                throw std::runtime_error("Unnamed object in OBJ file.");
            }
            printf("Object '%s' loaded\n", shapes[shape].name.c_str());
        }
    }
};

// Enumerates the types of object models available
enum ObjectModelType 
{
    COW,
    PLANE,
    MAZE
};

#endif // OBJMODEL_H