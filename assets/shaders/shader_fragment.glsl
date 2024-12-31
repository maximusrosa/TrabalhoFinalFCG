#version 330 core

in vec4 position_world;
in vec4 position_model;
in vec4 normal;
in vec2 texcoords;
in vec4 vertex_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identify the object to be rendered
#define COW 0
#define PLANE 1
#define MAZE 2
uniform int object_id;

// Identify the type of interpolation to be used
#define GOURAUD_INTERPOLATION 0
#define PHONG_INTERPOLATION 1
uniform int interpolation_type;

// Define the texture coordinates mapping type
#define PREDEFINED 0
#define PLANAR_PROJECTION 1

uniform vec4 bbox_min;
uniform vec4 bbox_max;

uniform sampler2D wall_texture;
uniform sampler2D gold_texture;

out vec4 color;

vec3 textureMapping(vec4 position_model, vec4 bbox_min, vec4 bbox_max, sampler2D texture_image, int mapping_type) {
    float u = 0.0;
    float v = 0.0;

    if ( mapping_type == PLANAR_PROJECTION )
    {
        float minx = bbox_min.x;
        float maxx = bbox_max.x;
        float miny = bbox_min.y;
        float maxy = bbox_max.y;
        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        float px = position_model.x;
        float py = position_model.y;
        float pz = position_model.z;


        u = (py - miny) / (maxy - miny);
        v = (pz - minz) / (maxz - minz);
    }
    else if ( mapping_type == PREDEFINED )
    {
        // Texture coordinates obtained from the OBJ file.
        u = texcoords.x;
        v = texcoords.y;
    }

    return texture(texture_image, vec2(u, v)).rgb;
}

void main() {
    color.a = 1.0;

    if (interpolation_type == GOURAUD_INTERPOLATION) {

        color.rgb = vertex_color.rgb;

    } else { // PHONG_INTERPOLATION
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;
        vec4 p = position_world;
        vec4 n = normalize(normal);
        vec4 l = normalize(camera_position - p);

        vec3 I = vec3(1.0, 1.0, 1.0); // É igual pra todos?

        switch (object_id)
        {
            case MAZE:
            { // Lambert Shading Model

                // Obtemos a refletância difusa a partir da leitura da imagem de textura
                vec3 Kd = textureMapping(position_model, bbox_min, bbox_max, wall_texture, PREDEFINED);
                vec3 lambert_diffuse_term = Kd * (max(dot(n, l), 0.0) + 0.1);

                color.rgb = lambert_diffuse_term;

                break;
            }

            case COW:
            { // Blinn-Phong Shading Model
                vec3 Kd = textureMapping(position_model, bbox_min, bbox_max, gold_texture, PLANAR_PROJECTION);

                vec3 Ka = vec3(0.05, 0.05, 0.05);
                vec3 Ia = vec3(0.25, 0.15, 0.2);

                vec3 Ks = vec3(0.8, 0.8, 0.8);
                vec4 d = l;
                vec4 half_vector = normalize(l + d);
                float q = 32.0;

                vec3 lambert_diffuse_term = Kd * I * max(dot(n, l), 0.0);
                vec3 ambient_term = Ka * Ia;
                vec3 blinn_phong_specular_term = Ks * I * pow(max(dot(n, half_vector), 0.0), q);

                color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;

                //vec3 lambert_diffuse_term = Kd * (max(dot(n, l), 0.0) + 0.1);
                //color.rgb = lambert_diffuse_term;

                break;
            }

            case PLANE:
            { // Blinn-Phong Shading Model
                vec3 Kd = vec3(0.0627, 0.2588, 0.0745);

                vec3 Ka = vec3(0.0, 0.0, 0.0);
                vec3 Ia = vec3(0.25, 0.15, 0.2);

                vec3 Ks = vec3(0.0, 0.0, 0.0);
                vec4 d = l;
                vec4 half_vector = normalize(l + d);
                float q = 1.0;

                vec3 lambert_diffuse_term = Kd * I * max(dot(n, l), 0.0);
                vec3 ambient_term = Ka * Ia;
                vec3 blinn_phong_specular_term = Ks * I * pow(max(dot(n, half_vector), 0.0), q);

                color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;

                break;
            }

            default:
            { // Lambert Shading Model
                vec3 Kd = vec3(0.0, 0.0, 0.0);
                vec3 lambert_diffuse_term = Kd * (max(dot(n, l), 0.0) + 0.1);

                color.rgb = lambert_diffuse_term;

                break;
            }
        }

        // Gamma correction
        color.rgb = pow(color.rgb, vec3(1.0, 1.0, 1.0) / 2.2);
    }
}