#version 330 core

layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identify the object to be rendered
#define COW 0
#define PLANE 1
#define MAZE 2
#define SPHERE 3
uniform int object_id;

// Identify the type of interpolation to be used
#define GOURAUD_INTERPOLATION 0
#define PHONG_INTERPOLATION 1
uniform int interpolation_type;

// Define the texture coordinates mapping type
#define PREDEFINED 0
#define PLANAR_PROJECTION 1
#define SPHERICAL_PROJECTION 2

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

uniform vec4 bbox_min;
uniform vec4 bbox_max;

uniform sampler2D wall_texture;
uniform sampler2D gold_texture;

out vec4 position_world;
out vec4 position_model;
out vec4 normal;
out vec2 texcoords;
out vec4 vertex_color;

vec3 textureMapping(vec4 position_model, vec4 bbox_min, vec4 bbox_max, sampler2D texture_image, int mapping_type) {
    float u = 0.0;
    float v = 0.0;

    switch (mapping_type) {
        case PLANAR_PROJECTION:
            float minx = bbox_min.x;
            float maxx = bbox_max.x;
            float miny = bbox_min.y;
            float maxy = bbox_max.y;
            float minz = bbox_min.z;
            float maxz = bbox_max.z;

            float px = position_model.x;
            float py = position_model.y;
            float pz = position_model.z;

            if (px == minx || px == maxx) {
                u = (pz - minz) / (maxz - minz);
                v = (py - miny) / (maxy - miny);
            } else if (py == miny || py == maxy) {
                u = (px - minx) / (maxx - minx);
                v = (pz - minz) / (maxz - minz);
            } else if (pz == minz || pz == maxz) {
                u = (px - minx) / (maxx - minx);
                v = (py - miny) / (maxy - miny);
            } else {
                u = (py - miny) / (maxy - miny);
                v = (pz - minz) / (maxz - minz);
            }

            break;

        case SPHERICAL_PROJECTION:
            vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
            float raio = 1.0;

            vec4 p_linha = bbox_center + (raio * ((position_model - bbox_center) / length(position_model - bbox_center)));
            vec4 vetor_p = p_linha - bbox_center;

            float tetha = atan(vetor_p.x, vetor_p.z);
            float phi = asin(vetor_p.y / raio);

            u = (tetha + M_PI) / (2 * M_PI);
            v = (phi + M_PI_2) / M_PI;

            break;

        case PREDEFINED:
            u = texcoords.x;
            v = texcoords.y;

            break;
    }

    return texture(texture_image, vec2(u, v)).rgb;
}

void main()
{
    gl_Position = projection * view * model * model_coefficients;

    position_world = model * model_coefficients;
    position_model = model_coefficients;

    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    texcoords = texture_coefficients;

    if (interpolation_type == GOURAUD_INTERPOLATION)
    {
        vec4 origin = vec4(0.0,0.0,0.0,1.0);
        vec4 camera_position = inverse(view) * origin;
        vec4 p = position_world;
        vec4 n = normalize(normal);
        vec4 l = normalize(camera_position - p);
        vec4 d = l;
        vec4 half_vector = normalize(l + d);

        vec3 Kd; // Diffuse reflectance
        vec3 Ks; // Specular reflectance
        vec3 Ka; // Ambient reflectance
        float q; // Specular exponent for Blinn-Phong model

        vec3 I = vec3(0.75, 0.75, 0.75);  // Light intensity (white light)
        vec3 Ia = vec3(0.25, 0.15, 0.2); // Ambient light intensity

        if (object_id == COW)
        {
            Kd = textureMapping(position_model, bbox_min, bbox_max, gold_texture, PLANAR_PROJECTION);
            //Kd = vec3(0.8,0.4,0.08);
            Ks = vec3(0.8,0.8,0.8);
            Ka = vec3(0.05, 0.05, 0.05);
            q = 32.0;
        }
        else if (object_id == SPHERE)
        {
            Kd = textureMapping(position_model, bbox_min, bbox_max, wall_texture, SPHERICAL_PROJECTION);
            Ks = vec3(0.8,0.8,0.8);
            Ka = vec3(0.05, 0.05, 0.05);
            q = 32.0;
        }

        else // Unknown object
        {
            Kd = vec3(0.0,0.0,0.0);
            Ks = vec3(0.0,0.0,0.0);
            Ka = vec3(0.0,0.0,0.0);
            q = 1.0;
        }

        vec3 lambert_diffuse_term = Kd * I * max(dot(n,l),0.0);
        vec3 ambient_term = Ka * Ia;
        vec3 blinn_phong_specular_term = Ks * I * pow(max(dot(n,half_vector),0.0),q);

        vertex_color.a = 1;
        vertex_color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;

        // Gamma correction
        vertex_color.rgb = pow(vertex_color.rgb, vec3(1.0,1.0,1.0)/2.2);
    }
    else
    {
        vertex_color = vec4(0.0,0.0,0.0,1.0);
    }
}