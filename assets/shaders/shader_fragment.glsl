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
#define CHEST 3
#define CHEST_LID 4
uniform int object_id;

// Identify the type of interpolation to be used
#define GOURAUD_INTERPOLATION 0
#define PHONG_INTERPOLATION 1
uniform int interpolation_type;

uniform vec4 bbox_min;
uniform vec4 bbox_max;

uniform sampler2D gold_texture;
uniform sampler2D grass_texture;
uniform sampler2D lava_texture;
uniform sampler2D stonebrick_texture;
uniform sampler2D galaxy_texture;
uniform sampler2D glowstone_texture;
uniform sampler2D wood_texture;
uniform sampler2D diamond_texture;
uniform sampler2D chest_texture;

out vec4 color;

void main()
{
    // Color "alpha" value (transparency) is set to 1.0
    color.a = 1.0;

    // Texture coordinates
    float u, v;

    if (interpolation_type == GOURAUD_INTERPOLATION)
    {
        color.rgb = vertex_color.rgb;
    }
    else
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

        vec3 I = vec3(1.0, 1.0, 1.0);  // Light intensity (white light)
        vec3 Ia = vec3(0.1686, 0.0039, 0.0863); // Ambient light intensity

        float px = position_model.x;
        float py = position_model.y;
        float pz = position_model.z;

        switch (object_id)
        {
            case MAZE:
            {
                float minx = bbox_min.x;
                float maxx = bbox_max.x;

                float miny = bbox_min.y;
                float maxy = bbox_max.y;

                float minz = bbox_min.z;
                float maxz = bbox_max.z;

                float epsilon = 0.525;

                if (abs(px - minx) < epsilon || abs(px - maxx) < epsilon)
                {
                    u = pz;
                    v = py;
                }
                else if (abs(pz - minz) < epsilon || abs(pz - maxz) < epsilon)
                {
                    u = px;
                    v = py;
                }
                else if (abs(py - miny) < epsilon || abs(py - maxy) < epsilon)
                {
                    u = pz;
                    v = px;
                }
                else
                {
                    // Default case (should not happen, just a safety measure)
                    u = px;
                    v = py;
                }

                Kd = texture(stonebrick_texture, vec2(u,v)).rgb;

                color.rgb = Kd * I * max(dot(n,l),0.05);;
                break;
            }
            case PLANE:
            {
                u = px;
                v = pz;

                Kd = texture(grass_texture, vec2(u, v)).rgb;

                color.rgb = Kd * I * max(dot(n,l),0.05);;
                break;
            }
            case COW:
            {
                u = px;
                v = py;

                Kd = texture(gold_texture, vec2(u,v)).rgb;
                Ks = vec3(0.2314, 0.0039, 0.2039);
                Ka = vec3(0.0902, 0.0039, 0.102);
                q = 64.0;

                vec3 lambert_diffuse_term = Kd * I * max(dot(n,l),0.05);
                vec3 ambient_term = Ka * Ia;
                vec3 blinn_phong_specular_term = Ks * I * pow(max(dot(n,half_vector),0.0),q);

                color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;
                break;
            }
            case CHEST:
            {
                u = texcoords.x;
                v = texcoords.y;

                Kd = texture(chest_texture, vec2(u,v)).rgb;
                Ks = vec3(0.0118, 0.0078, 0.0);
                Ka = vec3(0.0275, 0.0118, 0.0039);
                q = 64.0;

                vec3 lambert_diffuse_term = Kd * I * max(dot(n,l),0.05);
                vec3 ambient_term = Ka * Ia;
                vec3 blinn_phong_specular_term = Ks * I * pow(max(dot(n,half_vector),0.0),q);

                color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;
                break;
            }
            case CHEST_LID:
            {
                u = texcoords.x;
                v = texcoords.y;

                Kd = texture(chest_texture, vec2(u,v)).rgb;
                Ks = vec3(0.0118, 0.0078, 0.0);
                Ka = vec3(0.0275, 0.0118, 0.0039);
                q = 64.0;

                vec3 lambert_diffuse_term = Kd * I * max(dot(n,l),0.05);
                vec3 ambient_term = Ka * Ia;
                vec3 blinn_phong_specular_term = Ks * I * pow(max(dot(n,half_vector),0.0),q);

                color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;
                break;
            }
            default:
            {
                color.rgb = vec3(0.0, 0.0, 0.0);
                break;
            }
        }
        // Gamma correction
        color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
    }
}