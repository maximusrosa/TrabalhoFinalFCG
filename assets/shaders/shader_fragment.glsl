#version 330 core

in vec4 position_world;
in vec4 normal;
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

out vec4 color;

void main()
{
    color.a = 1.0;

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
        vec4 v = l;
        vec4 half_vector = normalize(l + v);

        vec3 Kd; // Diffuse reflectance
        vec3 Ks; // Specular reflectance
        vec3 Ka; // Ambient reflectance
        float q; // Specular exponent for Blinn-Phong model

        vec3 I = vec3(0.75, 0.75, 0.75);  // Light intensity (white light)
        vec3 Ia = vec3(0.25, 0.15, 0.2); // Ambient light intensity

        if ( object_id == COW )
        {
            Kd = vec3(0.8,0.4,0.08);
            Ks = vec3(0.8,0.8,0.8);
            Ka = vec3(0.05, 0.05, 0.05);
            q = 32.0;
        }
        else if ( object_id == PLANE )
        {   
            Kd = vec3(0.1, 0.6, 0.3);
            Ks = vec3(0.0, 0.0, 0.0);
            Ka = vec3(0.01, 0.01, 0.01);
            q = 1.0;
        }
        else if ( object_id == MAZE )
        {
            Kd = vec3(0.35,0.35,0.35);
            Ks = vec3(0.01,0.01,0.01);
            Ka = vec3(0.1, 0.1, 0.1);
            q = 2.0;
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

        color.rgb = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;

        // Gamma correction
        color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
    }
} 

