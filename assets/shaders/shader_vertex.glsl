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
uniform int object_id;

// Identify the type of interpolation to be used
#define GOURAUD_INTERPOLATION 0
#define PHONG_INTERPOLATION 1
uniform int interpolation_type;

out vec4 position_world;
out vec4 normal;
out vec4 vertex_color;

void main()
{
    gl_Position = projection * view * model * model_coefficients;
    position_world = model * model_coefficients;

    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    if (interpolation_type == GOURAUD_INTERPOLATION)
    {
        vec4 origin = vec4(0.0,0.0,0.0,1.0);
        vec4 camera_position = inverse(view) * origin;
        vec4 p = position_world;
        vec4 n = normalize(normal);
        vec4 l = normalize(camera_position - p);
        vec4 v = l;
        vec4 half_vector = normalize(l + v);

        vec3 Kd; // Diffuse reflectance
        vec3 Ks; // Specular reflectance
        vec3 Ka; // Ambient reflectance
        float q; // Specular exponent for Blinn-Phong model

        vec3 I = vec3(1.0,1.0,1.0);  // Light intensity (white light)
        vec3 Ia = vec3(0.2,0.1,0.1); // Ambient light intensity

        if ( object_id == COW )
        {
            Kd = vec3(0.8,0.4,0.08);
            Ks = vec3(0.8,0.8,0.8);
            Ka = vec3(0.05,0.05,0.05);
            q = 32.0;
        }
        else if ( object_id == PLANE )
        {   
            Kd = vec3(0.15,0.3,0.15);
            Ks = vec3(0.1,0.15,0.1);
            Ka = vec3(0.05,0.05,0.05);
            q = 4.0;
        }
        else if ( object_id == MAZE )
        {
            Kd = vec3(0.3,0.3,0.3);
            Ks = vec3(0.1,0.1,0.1);
            Ka = vec3(0.05,0.05,0.05);
            q = 4.0;
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

